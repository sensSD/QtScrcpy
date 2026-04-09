#pragma once

#include <QObject>
#include <QPointer>
#include <QTcpSocket>
#include <QTcpServer>
#include <QSize>

#include "serverEnums.h"
#include "adbEnums.h"
#include "adbprocess.h"

using namespace ServerEnums;

class Server: public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);

    /**
     * 开始服务器
     * @brief startServer
     * @param serial
     * @param localPort
     * @param maxSize
     * @param bitRate
     * @return
     */
    bool startServer(const QString& serial, quint16 localPort, quint16 maxSize, quint32 bitRate);
    void stopServer();

signals:
    /**
     * 服务器启动结果
     * @brief serverStartResult
     * @param success
     */
    void serverStartResult(bool success);

    /**
     * @brief 连接结果
     * 
     * @param success 
     * @param deviceName 
     * @param size 
     */
    void connectToResult(bool success, QString deviceName, QSize size);

private slots:
    /**
     * 工作进程结果
     * @brief onWorkProcessResult
     * @param processResult
     */
    void onWorkProcessResult(AdbEnums::ADB_EXEC_RESULT processResult);
    
private:
    /**
     * 获取服务器路径
     * @brief getServerPath
     * @return
     */
    QString getServerPath();

    /**
     * 按步骤启动服务器
     * @brief startServerStep
     * @return
     */
    bool startServerStep();

    /**
     * 上传服务器
     * @brief pushServer
     * @return
     */
    bool pushServer();

    /**
     * 删除服务器
     * @brief removeServer
     * @return
     */
    bool removeServer();

    /**
     * 启用反向
     * @brief enableReverse
     * @return
     */
    bool enableReverse();

    /**
     * @brief 执行服务器
     * 
     * @return true 
     * @return false 
     */
    bool executeServer();

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool disableReverse();

    /**
     * @brief 
     * 
     * @param deviceName 
     * @param size 
     * @return true 
     * @return false 
     */
    bool readInfo(QString& deviceName, QSize& size);

private:
    QString m_serial;
    quint16 m_localPort;
    quint16 m_maxSize;
    quint32 m_bitRate;
    static QString m_serverPath;
    bool m_serverCopiedToDevice;
    bool m_enableReverse;

    SERVER_START_STEP m_serverStartStep = SSS_NULL;

    AdbProcess m_workProcess;
    AdbProcess m_serverProcess;

    QTcpServer m_serverSocket;
    QPointer<QTcpSocket> m_deviceSocket = nullptr;
};
