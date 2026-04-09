#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <qcontainerfwd.h>
#include <QString>
#include <QtCore>

#include "adbprocess.h"

QString AdbProcess::s_adbPath = "";

AdbProcess::AdbProcess(QObject *parent)
    : QProcess(parent)
{
    initSignals();

    getAdbPath();
}

void AdbProcess::push(const QString &serial, const QString &local, const QString &remote)
{
    QStringList args;
    args << "push"
         << local 
         << remote;

    execute(serial, args);
}

void AdbProcess::removePath(const QString &serial, const QString &remote)
{
    QStringList args;
    args << "shell" 
         << "rm" 
         << remote;

    execute(serial, args);
}

void AdbProcess::reverse(const QString &serial, const QString &deviceSocketName, quint16 localPort)
{
    QStringList args;
    args << "reverse" 
         << QString("localabstract:%1").arg(deviceSocketName) 
         << QString("tcp:%1").arg(localPort);

    execute(serial, args);
}

void AdbProcess::removeReverse(const QString &serial, const QString &deviceSocketName)
{
    QStringList args;
    args << "reverse" 
         << "--remove" 
         << QString("localabstract:%1").arg(deviceSocketName);

    execute(serial, args);
}

QStringList AdbProcess::getDevicesSerialFromStdOut()
{
    QStringList serials;
    QStringList devicesInfoList = m_standardOutput.split(QRegularExpression("\r\n|\n"), Qt::SkipEmptyParts);

    for(const auto& deviceInfo : devicesInfoList) {
        QStringList parts = deviceInfo.split("\t", Qt::SkipEmptyParts);
        if(2 == parts.size() && parts[1] == "device") {
            serials << parts[0];
        }
    }

    return serials;
}

QString AdbProcess::getDeviceIpFromStdOut()
{
    QRegularExpression ipRegex(R"(inet\s+(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}))");
    QRegularExpressionMatch match = ipRegex.match(m_standardOutput);
    
    if (match.hasMatch()) {
        return match.captured(1);
    }

    return QString();
}

QString AdbProcess::getStdOutput()
{
    return m_standardOutput;
}

QString AdbProcess::getStdError()
{
    return m_standardError;
}

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

            s_adbPath = dir.path() + "/third_party/adb/win/adb.exe";
        }
    }
    
    // qDebug() << s_adbPath;

    return s_adbPath;
}

void AdbProcess::execute(const QString &serial, const QStringList &args)
{
    QStringList adbArgs;
    if(!serial.isEmpty()) {
        adbArgs << "-s" << serial;
    }
    adbArgs << args;

    start(getAdbPath(), adbArgs);
}

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
        QByteArray error = readAllStandardError();
        m_standardError = QString::fromLocal8Bit(error).trimmed();
        qDebug() << QString::fromLocal8Bit(error);
    });

    connect(this, &QProcess::readyReadStandardOutput, this, [this](){
        QByteArray output = readAllStandardOutput();
        m_standardOutput = QString::fromLocal8Bit(output).trimmed();
        qDebug() << QString::fromLocal8Bit(output);
    });

    // 启动
    connect(this, &QProcess::started, this, [this](){
        emit adbProcessResult(AER_SUCCESS_START);
    });
}
