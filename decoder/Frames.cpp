#include "Frames.h"

#include <qassert.h>

#include "ScopeGuard.h"


extern "C" {
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}

Frames::Frames(QObject* parent) : QObject{parent} {
}

bool Frames::init() {
  auto cleanup = sg::make_scope_guard([this]() {
    if (m_decodingFrame) {
      av_frame_free(&m_decodingFrame);
    }
    if (m_renderingframe) {
      av_frame_free(&m_renderingframe);
    }

    m_renderingFrameConsumed = true;
  });

  m_decodingFrame = av_frame_alloc();
  if (!m_decodingFrame) {
    return false;
  }

  m_renderingframe = av_frame_alloc();
  if (!m_renderingframe) {
    return false;
  }

  cleanup.dismiss();  // 资源分配成功，取消清理
  return true;
}

Frames::~Frames() {
  deInit();
}

void Frames::deInit() {
  if (m_decodingFrame) {
    av_frame_free(&m_decodingFrame);
    m_decodingFrame = nullptr;
  }
  if (m_renderingframe) {
    av_frame_free(&m_renderingframe);
    m_renderingframe = nullptr;
  }
}

void Frames::lock() {
  m_mutex.lock();
}

void Frames::unLock() {
  m_mutex.unlock();
}

AVFrame* Frames::decodingFrame() {
  return m_decodingFrame;
}

bool Frames::offerDecodedFrames() {
  m_mutex.lock();
  swap();
  bool previousFrameConsumed = m_renderingFrameConsumed;
  m_renderingFrameConsumed = false;
  m_mutex.unlock();

  return previousFrameConsumed;
}

const AVFrame* Frames::consumeRenderingFrame() {
  Q_ASSERT(!m_renderingFrameConsumed);
  m_renderingFrameConsumed = true;

  return m_renderingframe;
}

void Frames::swap() {
  AVFrame* temp = m_decodingFrame;
  m_decodingFrame = m_renderingframe;
  m_renderingframe = temp;
}

void Frames::stop() {
  // Implementation for stopping the frames
}
