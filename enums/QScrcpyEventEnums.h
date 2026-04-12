#ifndef QSCRCPYEVENTENUMS_H
#define QSCRCPYEVENTENUMS_H

#include <QEvent>
#include <QMetaType>


namespace QScrcpyEventEnums {
    enum Type {
    DeviceSocket = QEvent::User + 1, // 设备套接字事件
    Control,
    };
}

// 注册枚举
Q_DECLARE_METATYPE(QScrcpyEventEnums::Type)

#endif // QSCRCPYEVENTENUMS_H
