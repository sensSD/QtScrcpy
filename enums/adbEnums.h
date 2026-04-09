#ifndef ADBENUMS_H
#define ADBENUMS_H

#include <QMetaType>

namespace AdbEnums
{
    enum ADB_EXEC_RESULT {
        AER_SUCCESS_START,          // 启动成功
        AER_ERROR_START,            // 启动失败
        AER_SUCCESS_EXEC,           // 执行成功
        AER_ERROR_EXEC,             // 执行失败
        AER_ERROR_MISSING_BINARY,   // 找不到文件
    };
}

// 注册枚举
Q_DECLARE_METATYPE(AdbEnums::ADB_EXEC_RESULT)

#endif // ADBENUMS_H
