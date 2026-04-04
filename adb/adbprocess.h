#ifndef ADBPROCESS_H
#define ADBPROCESS_H

#include <QProcess>

class AdbProcess : public QProcess
{
    Q_OBJECT
public:
    enum ADB_EXEC_RESULT {
        AER_SUCCESS_START,          // 启动成功
        AER_ERROR_START,            // 启动失败
        AER_SUCCESS_EXEC,           // 执行成功
        AER_ERROR_EXEC,             // 执行失败
        AER_ERROR_MISSING_BINARY,   // 找不到文件
    };

    AdbProcess(QObject *parent = Q_NULLPTR);

    /** 封装adb
     * 执行adb命令
     * @brief execute
     * @param serial
     * @param args
     */
    void execute(const QString& serial, const QStringList& args);

    /**
     * 获取adb所在路径
     * @brief getAdbPath
     * @return
     */
    static QString getAdbPath();

signals:
    void adbProcessResult(ADB_EXEC_RESULT processResult);

private:

    /**
     * QProcess信号
     * @brief initSignals
     */
    void initSignals();

    static QString s_adbPath;
};

#endif // ADBPROCESS_H
