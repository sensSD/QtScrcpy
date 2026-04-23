#pragma once

#include "InputConvertBase.h"

class InputConvertNormal : public InputConvertBase {
 public:
  explicit InputConvertNormal();
  ~InputConvertNormal() = default;

 public:
  void mouseEvent(const QMouseEvent* from, const QSize& frameSize,
                  const QSize& widgetSize) override;
  void wheelEvent(const QWheelEvent* from, const QSize& frameSize,
                  const QSize& widgetSize) override;
  void keyEvent(const QKeyEvent* from, const QSize& frameSize, const QSize& widgetSize) override;

 private:
  /**
   * @brief 鼠标按键转换
   *
   * @param buttons
   * @return AndroidMotioneventButtons
   */
  AndroidMotioneventButtons convertMouseButtons(Qt::MouseButtons buttons);

  /**
   * @brief 键盘按键转换
   *
   * @param key
   * @param modifiers
   * @return AndroidKeycode
   */
  AndroidKeycode convertKeyCode(int key, Qt::KeyboardModifiers modifiers);

  /**
   * @brief 键盘修饰符转换
   *
   * @param modifiers
   * @return AndroidMetastate
   */
  AndroidMetastate convertMetastate(Qt::KeyboardModifiers modifiers);
};
