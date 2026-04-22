#pragma once

#include <QEvent>

#include "QScrcpyEventEnums.h"


using namespace QScrcpyEventEnums;

class QScrcpyEvent : public QEvent {
 public:
  QScrcpyEvent(QScrcpyEventEnums::Type type) : QEvent(static_cast<QEvent::Type>(type)) {
  }
};

class DeviceSocketEvent : public QScrcpyEvent {
 public:
  DeviceSocketEvent() : QScrcpyEvent(QScrcpyEventEnums::DeviceSocket) {
  }
};
