#pragma once

#include <QMutex>
#include <QPointer>
#include <QThread>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

class Frames;
class DeviceSocket;
class Decoder : public QThread {
  Q_OBJECT

 public:
  explicit Decoder(QThread* parent = nullptr);
  virtual ~Decoder();

 public:
  /**
   * @brief 初始化解码器
   *
   * @return true
   * @return false
   */
  static bool init();

  /**
   * @brief 清理解码器
   *
   */
  static void deInit();

  /**
   * @brief 保存解码帧
   *
   * @param frames
   */
  void setFrames(Frames* frames);

  /**
   * @brief 设置socket
   *
   * @param deviceSocket
   */
  void setDeviceSocket(DeviceSocket* deviceSocket);

  /**
   * @brief 接收数据
   *
   * @param buf
   * @param bufSize
   * @return qint32
   */
  qint32 recvData(quint8* buf, qint32 bufSize);

  /**
   * @brief 开始解码
   *
   * @return true
   * @return false
   */
  bool startDecode();

  /**
   * @brief 停止解码
   *
   */
  void stopDecode();

 signals:
  /**
   * @brief 新的帧到达
   *
   */
  void onNewFrame();

  /**
   * @brief 解码停止
   *
   */
  void onDecodeStop();

 protected:
  /**
   * @brief 运行解码线程
   *
   */
  void run();

  /**
   * @brief 推送解码帧
   *
   */
  void pushFrame();

 private:
  QPointer<DeviceSocket> m_deviceSocket;  // 接收h264数据
  bool m_quit = false;                    // 退出标记
  Frames* m_frames;                       // 解码出的帧
};
