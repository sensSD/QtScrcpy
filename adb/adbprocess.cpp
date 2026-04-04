#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>

#include "adbprocess.h"

QString AdbProcess::s_adbPath = "";

AdbProcess::AdbProcess(QObject *parent)
    : QProcess(parent)
{
    initSignals();

    getAdbPath();
}

/**
 * 获取adb路径
 * @brief AdbProcess::getAdbPath
 * @return
 */
QString AdbProcess::getAdbPath()
{
    if(s_adbPath.isEmpty()) {
        s_adbPath = QCoreApplication::applicationDirPath();
        QFileInfo fileInfo(s_adbPath);
        
        if(s_adbPath.isEmpty() || !fileInfo.isFile()) {
            QDir dir = fileInfo.dir();
            while(dir.dirName() != "QtScrcpy" && !dir.isRoot()) {
                dir.cdUp();
            }

            s_adbPath = dir.path() + "/thrid_party/adb/win/adb.exe";
        }
    }
    
    // qDebug() << s_adbPath;

    return s_adbPath;
}

/**
 * 执行adb
 * @brief AdbProcess::execute
 * @param serial
 * @param args
 */
void AdbProcess::execute(const QString &serial, const QStringList &args)
{
    QStringList adbArgs;
    if(!serial.isEmpty()) {
        adbArgs << "-s" << serial;
    }
    adbArgs << args;

    start(getAdbPath(), adbArgs);
}

/**
 * QProcess信号
 * @brief AdbProcess::initSignals
 */
void AdbProcess::initSignals()
{
    // 发生错误
    connect(this, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error){
        if(QProcess::FailedToStart == error) {
            emit adbProcessResult(AER_ERROR_MISSING_BINARY);
        } else {
            emit adbProcessResult(AER_ERROR_START);
        }
        qDebug() << error;
    });

    // 退出状态
    connect(this, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    [=](int exitCode, QProcess::ExitStatus exitStatus){
        if(QProcess::NormalExit == exitStatus && 0 == exitCode) {
            emit adbProcessResult(AER_SUCCESS_EXEC);
        } else {
            emit adbProcessResult(AER_ERROR_EXEC);
        }
        qDebug() << exitCode << exitStatus;
    });

    // 标准输出
    connect(this, &QProcess::readyReadStandardError, this, [this](){
        qDebug() << readAllStandardError();
    });

    connect(this, &QProcess::readyReadStandardOutput, this, [this](){
        qDebug() << readAllStandardOutput();
    });

    // 启动
    connect(this, &QProcess::started, this, [this](){
        emit adbProcessResult(AER_SUCCESS_START);
    });
}
