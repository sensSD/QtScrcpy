#include <qdebug.h>
#include <qlogging.h>
#include <qtypes.h>

#include "Decoder.h"
#include "DeviceSocket.h"
#include "Frames.h"
#include "libavutil/mem.h"

#define BUFSIZE 0x10000

Decoder::Decoder(QThread *parent) : QThread{parent} {
}

Decoder::~Decoder() {
  stopDecode();
}

bool Decoder::init() {
  // 初始化
  if (avformat_network_init()) {
    return false;
  }
  return true;
}

void Decoder::deInit() {
  avformat_network_deinit();
}

void Decoder::setFrames(Frames *frames) {
  m_frames = frames;
}

void Decoder::setDeviceSocket(DeviceSocket *deviceSocket) {
  m_deviceSocket = deviceSocket;
}

// 参数： Decoder对象，解码数据缓存，解码数据缓存大小
static qint32 readPacket(void *opaque, quint8 *buf, qint32 bufSize) {
  Decoder *decoder = (Decoder *)opaque;
  if (decoder) {
    return decoder->recvData(buf, bufSize);
  }
  return 0;
}

qint32 Decoder::recvData(quint8 *buf, qint32 bufSize) {
  if (!buf) {
    return 0;
  }
  if (m_deviceSocket) {
    // 从deviceSocket获取h264数据
    qint32 len = m_deviceSocket->subThreadRecvData(buf, bufSize);
    qDebug() << "recvData len:" << len;
    if (len == -1) {
      return AVERROR(errno);
    }
    if (len == 0) {
      return AVERROR_EOF;
    }
    return len;
  }
  return AVERROR_EOF;
}

bool Decoder::startDecode() {
  if (!m_deviceSocket) {
    return false;
  }

  m_quit = false;
  // 启动解码线程
  start();

  return true;
}

void Decoder::stopDecode() {
  m_quit = true;
  if (m_frames) {
    m_frames->stop();
  }
  // 等待解码线程退出
  wait();
}

void Decoder::run() {
  struct ResourceGuard {
    unsigned char *decoderBuffer = nullptr;
    AVIOContext *avioCtx = nullptr;
    AVFormatContext *formatCtx = nullptr;
    AVCodecContext *codecCtx = nullptr;
    AVPacket *packet = nullptr;
    bool isFormatCtxOpen = false;
    bool isCodecCtxOpen = false;

    ~ResourceGuard() {
      if (isCodecCtxOpen && codecCtx) {
        avcodec_free_context(&codecCtx);
      }
      if (isFormatCtxOpen && formatCtx) {
        avformat_close_input(&formatCtx);
      }
      if (avioCtx) {
        av_freep(&avioCtx->buffer);
        avio_context_free(&avioCtx);
      }
      if (packet) {
        av_packet_free(&packet);
      }
      if (decoderBuffer) {
        av_free(decoderBuffer);
      }
    }
  } resources;

  // 申请解码缓冲区
  resources.decoderBuffer = (unsigned char *)av_malloc(BUFSIZE);
  if (!resources.decoderBuffer) {
    qCritical("Could not allocate decoder buffer");
    return;
  }

  // 初始化io上下文
  resources.avioCtx = avio_alloc_context(resources.decoderBuffer, BUFSIZE, 0,
                                         this, readPacket, NULL, NULL);
  if (!resources.avioCtx) {
    qCritical("Could not allocate avio context");
    av_free(resources.decoderBuffer);
    return;
  }

  // 初始化封装上下文
  resources.formatCtx = avformat_alloc_context();
  if (!resources.formatCtx) {
    qCritical("Could not allocate format context");
    return;
  }

  // 为封装上下文指定io上下文
  resources.formatCtx->pb = resources.avioCtx;
  // 打开封装上下文
  if (avformat_open_input(&resources.formatCtx, NULL, NULL, NULL) < 0) {
    qCritical("Could not open input");
    return;
  }
  resources.isFormatCtxOpen = true;

  // 初始化解码器
  const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
  if (!codec) {
    qCritical("Could not find H.264 codec");
    return;
  }

  // 初始化解码器上下文
  resources.codecCtx = avcodec_alloc_context3(codec);
  if (!resources.codecCtx) {
    qCritical("Could not allocate codec context");
    return;
  }

  // 打开解码器上下文
  if (avcodec_open2(resources.codecCtx, codec, NULL) < 0) {
    qCritical("Could not open H.264 codec");
    return;
  }
  resources.isCodecCtxOpen = true;

  // 解码数据包：保存解码前的一帧h264数据
  resources.packet = av_packet_alloc();
  if (!resources.packet) {
    qCritical("Could not allocate packet");
    return;
  }

  // 初始化解码数据包
  resources.packet->data = nullptr;
  resources.packet->size = 0;

  // 从封装上下文中读取一帧解码前的数据，保存到AVPacket中
  while (!m_quit && !av_read_frame(resources.formatCtx, resources.packet)) {
    // 获取AVFrame用来保存解码出来的yuv数据
    AVFrame *decodingFrame = m_frames->decodingFrame();
    // 解码
    int ret;
    // 解码h264
    if ((ret = avcodec_send_packet(resources.codecCtx, resources.packet)) < 0) {
      qCritical("Could not send video packet: %d", ret);
      return;
    }
    // 取出yuv
    if (decodingFrame) {
      ret = avcodec_receive_frame(resources.codecCtx, decodingFrame);
    }
    if (!ret) {
      // 成功解码出一帧
      pushFrame();
    } else if (ret != AVERROR(EAGAIN)) {
      qCritical("Could not receive video frame: %d", ret);
      av_packet_unref(resources.packet);
      return;
    }

    av_packet_unref(resources.packet);

    if (resources.avioCtx->eof_reached) {
      break;
    }
  }
  qDebug() << "End of frames";

  emit onDecodeStop();
}

void Decoder::pushFrame() {
  bool previousFrameConsumed = m_frames->offerDecodedFrames();
  if (!previousFrameConsumed) {
    return;
  }

  emit onNewFrame();
}