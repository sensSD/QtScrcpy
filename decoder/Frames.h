#pragma once

#include <QMutex>
#include <QObject>
#include <QWaitCondition>

typedef struct AVFrame AVFrame;

class Frames : public QObject {
  Q_OBJECT

 public:
  explicit Frames(QObject* parent = nullptr);
  virtual ~Frames();

 public:
  /**
   * @brief 初始化Frames
   *
   * @return true
   * @return false
   */
  bool init();

  /**
   * @brief 清除Frames
   *
   */
  void deInit();

  /**
   * @brief 锁定Frames
   *
   */
  void lock();

  /**
   * @brief 解锁Frames
   *
   */
  void unLock();

  /**
   * @brief 获取解码帧
   *
   * @return AVFrame*
   */
  AVFrame* decodingFrame();

  /**
   * @brief 提供已解码的帧
   *
   * @return true
   * @return false
   */
  bool offerDecodedFrames();

  /**
   * @brief 消费渲染帧
   *
   * @return const AVFrame*
   */
  const AVFrame* consumeRenderingFrame();

  /**
   * @brief 交换解码帧和渲染帧
   *
   */
  void swap();

  void stop();

 private:
  // 解码出来的一帧数据（yuv）
  // 保存正在解码的yuv
  AVFrame* m_decodingFrame = nullptr;
  // 保存正在渲染的yuv
  AVFrame* m_renderingframe = nullptr;
  // 保证AVFrame的多线程安全
  QMutex m_mutex;
  bool m_renderingFrameConsumed = true;
};
