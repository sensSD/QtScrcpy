#include "ControlEvent.h"

#include <QDebug>

#define TEXT_MAX_CHARACTER_LENGTH 300

ControlEvent::ControlEvent(ControlEventType type) : QScrcpyEvent(Control) {
  m_data.type = type;
}

void ControlEvent::setKeycodeEventData(AndroidKeyeventAction action, AndroidKeycode keycode,
                                       AndroidMetastate metastate) {
  m_data.keyEvent.action = action;
  m_data.keyEvent.keycode = keycode;
  m_data.keyEvent.metastate = metastate;
}

void ControlEvent::setTextEventData(const QString& text) {
  m_data.textEvent.text = text;
}

void ControlEvent::setCommandEventData(const quint32& command) {
  m_data.commandEvent.action = command;
}

void ControlEvent::setTouchEventData(const quint32& id, AndroidMotioneventAction action,
                                     QRect position) {
  m_data.touchEvent.id = id;
  m_data.touchEvent.action = action;
  m_data.touchEvent.position = position;
}

void ControlEvent::setScrollEventData(const QRect& position, qint32 hScroll, qint32 vScroll) {
  m_data.scrollEvent.position = position;
  m_data.scrollEvent.hScroll = hScroll;
  m_data.scrollEvent.vScroll = vScroll;
}

void ControlEvent::setMouseEventData(AndroidMotioneventAction action,
                                     AndroidMotioneventButtons buttons, QRect position) {
  m_data.mouseEvent.action = action;
  m_data.mouseEvent.buttons = buttons;
  m_data.mouseEvent.position = position;
}

QByteArray ControlEvent::serializeData() {
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  buffer.open(QBuffer::WriteOnly);
  buffer.putChar(m_data.type);

  switch (m_data.type) {
    case CET_KEYCODE:
      buffer.putChar(m_data.keyEvent.action);
      writeByByte(buffer, m_data.keyEvent.keycode);
      writeByByte(buffer, m_data.keyEvent.metastate);
      break;
    case CET_TEXT: {
      if (TEXT_MAX_CHARACTER_LENGTH < m_data.textEvent.text.length()) {
        m_data.textEvent.text = m_data.textEvent.text.left(TEXT_MAX_CHARACTER_LENGTH);
      }

      QByteArray textBytes = m_data.textEvent.text.toUtf8();
      writeByByte(buffer, textBytes.length(), 2);
      buffer.write(textBytes.data(), textBytes.length());
      break;
    }
    case CET_MOUSE:
      buffer.putChar(m_data.mouseEvent.action);
      writeByByte(buffer, m_data.mouseEvent.buttons);
      writePosition(buffer, m_data.mouseEvent.position);
      break;
    case CET_SCROLL:
      writePosition(buffer, m_data.scrollEvent.position);
      writeByByte(buffer, m_data.scrollEvent.hScroll);
      writeByByte(buffer, m_data.scrollEvent.vScroll);
      break;
    case CET_TOUCH:
      buffer.putChar(m_data.touchEvent.id);
      buffer.putChar(m_data.touchEvent.action);
      writePosition(buffer, m_data.touchEvent.position);
      break;
    case CET_COMMAND:
      buffer.putChar(m_data.commandEvent.action);
      break;
    default:
      qDebug() << "Unknown event type:" << m_data.type;
      break;
  }

  buffer.close();
  return byteArray;
}

void ControlEvent::writeByByte(QBuffer& buffer, quint32 value, int byteCount) {
  for (int i = byteCount - 1; i >= 0; i--) {
    buffer.putChar((value >> (i * 8)) & 0xFF);
  }
}

void ControlEvent::writePosition(QBuffer& buffer, const QRect& value) {
  writeByByte(buffer, value.left(), 2);
  writeByByte(buffer, value.top(), 2);
  writeByByte(buffer, value.width(), 2);
  writeByByte(buffer, value.height(), 2);
}
