#pragma once

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "Controller.h"
#include "ControlEvent.h"


class ControlEvent;
class InputConvertBase {
 public:
  explicit InputConvertBase();
  virtual ~InputConvertBase() = default;

 public:
  virtual void mouseEvent(const QMouseEvent* from, const QSize& frameSize,
                          const QSize& widgetSize) = 0;
  virtual void wheelEvent(const QWheelEvent* from, const QSize& frameSize,
                          const QSize& widgetSize) = 0;
  virtual void keyEvent(const QKeyEvent* from, const QSize& frameSize, const QSize& widgetSize) = 0;

  void setDeviceSocket(::DeviceSocket* deviceSocket);

  /**
   * @brief 发送控制事件
   *
   * @param event
   */
  void sendControlEvent(ControlEvent* event);

 private:
  Controller m_controller;
};
