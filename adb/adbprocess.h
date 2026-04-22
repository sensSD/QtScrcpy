#ifndef ADBPROCESS_H
#define ADBPROCESS_H

#include <QProcess>

#include "adbEnums.h"

using namespace AdbEnums;

class AdbProcess : public QProcess {
  Q_OBJECT
public:
  AdbProcess(QObject *parent = Q_NULLPTR);
  ~AdbProcess();

public:
  /** 封装adb
   * 执行adb命令
   * @brief execute
   * @param serial
   * @param args
   */
  void execute(const QString &serial, const QStringList &args);

  /**
   * 上传文件
   * @brief push
   * @param serial
   * @param local
   * @param remote
   */
  void push(const QString &serial, const QString &local, const QString &remote);

  /**
   * 删除文件
   * @brief removePath
   * @param serial
   * @param remote
   */
  void removePath(const QString &serial, const QString &remote);

  /**
   * 端口映射
   * @brief reverse
   * @param serial
   * @param deviceSocketName
   * @param localPort
   */
  void reverse(const QString &serial, const QString &deviceSocketName, quint16 localPort);

  /**
   * 删除端口映射
   * @brief removeReverse
   * @param serial
   * @param deviceSocketName
   */
  void removeReverse(const QString &serial, const QString &deviceSocketName);

  /**
   * 获取设备列表
   * @brief getDevicesSerialFromStdOut
   * @return QStringList
   */
  QStringList getDevicesSerialFromStdOut();

  /**
   * 获取设备IP
   * @brief getDeviceIpFromStdOut
   * @return QString
   */
  QString getDeviceIpFromStdOut();

  /**
   * 获取标准输出
   * @brief getStandardOutput
   * @return QString
   */
  QString getStdOutput();

  /**
   * 获取标准错误
   * @brief getStandardError
   * @return QString
   */
  QString getStdError();

  /**
   * 获取adb所在路径
   * @brief getAdbPath
   * @return QString
   */
  static QString getAdbPath();

signals:
  /**
   * adb执行结果
   * @brief adbProcessResult
   * @param processResult
   */
  void adbProcessResult(ADB_EXEC_RESULT processResult);

private:
  /**
   * 信号处理
   * @brief initSignals
   */
  void initSignals();

  static QString s_adbPath;
  QString m_standardOutput;
  QString m_standardError;
};

#endif // ADBPROCESS_H
