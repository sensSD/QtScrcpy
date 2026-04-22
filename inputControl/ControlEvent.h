#pragma once

#include <QBuffer>
#include <QRect>

#include "ControlEventEnums.h"
#include "QScrcpyEvent.h"
#include "input.h"
#include "keycodes.h"

using namespace ControlEventEnums;

class ControlEvent : public QScrcpyEvent {
 public:
  explicit ControlEvent(ControlEventType type);
  ~ControlEvent() = default;

 public:
  /**
   * @brief 序列化事件数据为字节数组，方便通过网络发送
   *
   * @return QByteArray
   */
  QByteArray serializeData();

  /**
   * @brief 设置鼠标事件数据
   *
   * @param action
   * @param buttons
   * @param position
   */
  void setMouseEventData(AndroidMotioneventAction action, AndroidMotioneventButtons buttons,
                         QRect position);

  /**
   * @brief 设置键盘事件数据
   *
   * @param action
   * @param keycode
   * @param metastate
   */
  void setKeycodeEventData(AndroidKeyeventAction action, AndroidKeycode keycode,
                           AndroidMetastate metastate);

  /**
   * @brief 设置文本输入事件数据
   *
   * @param text
   */
  void setTextEventData(const QString& text);

  /**
   * @brief 设置命令事件数据
   *
   * @param command
   */
  void setCommandEventData(const quint32& command);

  /**
   * @brief 设置多指触控事件数据
   *
   * @param id
   * @param action
   * @param position
   */
  void setTouchEventData(const quint32& id, AndroidMotioneventAction action, QRect position);

  /**
   * @brief 设置滚动事件数据
   *
   * @param position
   * @param hScroll
   * @param vScroll
   */
  void setScrollEventData(const QRect& position, qint32 hScroll, qint32 vScroll);

 protected:
  /**
   * @brief 将16进制数按字节写入缓冲区，默认写4字节（32位）
   *
   * @param buffer
   * @param value
   * @param byteCount
   */
  void writeByByte(QBuffer& buffer, quint32 value, int byteCount = 4);
  void writePosition(QBuffer& buffer, const QRect& value);

 private:
  struct ControlEventData {
    ControlEventType type;
    union {
      struct {
        AndroidKeyeventAction action;
        AndroidKeycode keycode;
        AndroidMetastate metastate;
      } keyEvent;
      struct {
        QString text;
      } textEvent;
      struct {
        AndroidMotioneventAction action;
        AndroidMotioneventButtons buttons;
        QRect position;
      } mouseEvent;
      struct {
        QRect position;
        qint32 hScroll;
        qint32 vScroll;
      } scrollEvent;
      struct {
        quint32 action;
      } commandEvent;
      struct {
        quint32 id;
        AndroidMotioneventAction action;
        QRect position;
      } touchEvent;
    };

    ControlEventData() {
    }
    ~ControlEventData() {
    }
  };

  // 事件数据
  ControlEventData m_data;
};
