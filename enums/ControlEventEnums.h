#ifndef ControlEventEnums_H
#define ControlEventEnums_H

#include <QMetaType>

namespace ControlEventEnums
{
    enum ControlEventType {
        CET_KEYCODE,    // 键盘事件
        CET_TEXT,       // 文本输入事件
        CET_MOUSE,      // 鼠标事件（包括点击和移动）
        CET_SCROLL,     // 滚轮事件
        CET_COMMAND,    // 文本数据事件
        CET_TOUCH,      // 多指触控事件
    };
}

// 注册枚举
Q_DECLARE_METATYPE(ControlEventEnums::ControlEventType)

#endif // ControlEventEnums_H
