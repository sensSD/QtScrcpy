#ifndef SERVERENUMS_H
#define SERVERENUMS_H

#include <QMetaType>

namespace ServerEnums {
enum SERVER_START_STEP {
  SSS_NULL,            // 默认
  SSS_PUSH,            // 开始推送
  SSS_ENABLE_REVERSE,  // 启动反向
  SSS_EXEC_SERVER,     // 启动服务
  SSS_RUNNING,         // 运行中
};
}

// 注册枚举
Q_DECLARE_METATYPE(ServerEnums::SERVER_START_STEP)

#endif  // SERVERENUMS_H
