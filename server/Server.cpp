#include "Server.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QtCore>

#include "DeviceSocket.h"
#include "adbEnums.h"
#include "adbprocess.h"
#include "serverEnums.h"

#define DEVICE_SERVER_PATH "/data/local/tmp/scrcpy-server.jar"
#define SOCKET_NAME "scrcpy"
#define DEVICE_NAME_FIELD_LENGTH 64
#define MAX_CONNECTIONS 1

#define TEST

QString Server::m_serverPath = "";

Server::Server(QObject* parent) : QObject{parent} {
  connect(&m_workProcess, &AdbProcess::adbProcessResult, this, &Server::onWorkProcessResult);
  connect(&m_serverProcess, &AdbProcess::adbProcessResult, this, &Server::onWorkProcessResult);
  connect(&m_serverSocket, &QTcpServer::newConnection, this, [this] {
    m_deviceSocket = dynamic_cast<DeviceSocket*>(m_serverSocket.nextPendingConnection());

    // 连接成功时，scrcpy-server会返回devices name，size
    QString deviceName;
    QSize size;
    if (m_deviceSocket && m_deviceSocket->isValid() && readInfo(deviceName, size)) {
      qDebug("Device connected");
      // 连接到设备时，反向代理就可以关闭了
      disableReverse();
      // 此时删除服务会在安卓端留下标记，当进程执行结束时，会自动删除
      removeServer();
      emit connectToResult(true, deviceName, size);
    } else {
      stopServer();
      emit connectToResult(false, deviceName, size);
    }
  });
}

void Server::onWorkProcessResult(AdbEnums::ADB_EXEC_RESULT processResult) {
  if (sender() == &m_workProcess) {
    switch (m_serverStartStep) {
      case SSS_NULL:
        break;
      case SSS_PUSH:
        if (AdbEnums::AER_SUCCESS_EXEC == processResult) {
          m_serverStartStep = SSS_ENABLE_REVERSE;
          m_serverCopiedToDevice = true;
          startServerStep();
        } else if (AdbEnums::AER_SUCCESS_START != processResult) {
          qCritical("adb push failed");
          m_serverStartStep = SSS_NULL;
          emit serverStartResult(false);
        }
        break;
      case SSS_ENABLE_REVERSE:
        if (AdbEnums::AER_SUCCESS_EXEC == processResult) {
          m_enableReverse = true;
          m_serverStartStep = SSS_EXEC_SERVER;
          startServerStep();
        } else if (AdbEnums::AER_SUCCESS_START != processResult) {
          qCritical("adb reverse failed");
          m_serverStartStep = SSS_NULL;
          removeServer();
          emit serverStartResult(false);
        }
        break;

      default:
        break;
    }
  }

  if (sender() == &m_serverProcess) {
    if (SSS_EXEC_SERVER == m_serverStartStep) {
      // 启动服务为阻塞命令，只会抛出start信号
      if (AER_SUCCESS_START == processResult) {
        m_serverStartStep = SSS_RUNNING;
        emit serverStartResult(true);
      } else if (AER_ERROR_START == processResult) {
        m_serverStartStep = SSS_NULL;
        disableReverse();
        qCritical("adb start-server failed");
        removeServer();
        emit serverStartResult(false);
      }
    } else if (SSS_RUNNING == m_serverStartStep) {
      m_serverStartStep = SSS_NULL;
      emit onServerStopped();
    }
  }
}

bool Server::startServer(const QString& serial, quint16 localPort, quint16 maxSize,
                         quint32 bitRate) {
  m_localPort = localPort;
  m_maxSize = maxSize;
  m_bitRate = bitRate;
  m_serial = serial;

  m_serverStartStep = SSS_PUSH;
  return startServerStep();
}

void Server::stopServer() {
  // 保存当前设备套接字，然后清空成员变量
  DeviceSocket* tempSocket = m_deviceSocket;
  m_deviceSocket = nullptr;

  // 关闭设备连接
  if (tempSocket) {
    tempSocket->close();
    tempSocket->deleteLater();
  }

  // 停止服务器进程
  if (m_serverProcess.state() == QProcess::Running) {
    m_serverProcess.terminate();
    if (!m_serverProcess.waitForFinished(2000)) {
      m_serverProcess.kill();
      m_serverProcess.waitForFinished(1000);
    }
  }

  // 关闭服务器监听
  m_serverSocket.close();

  // 同步清理
  disableReverse();
  removeServer();

  // 重置状态
  m_enableReverse = false;
  m_serverCopiedToDevice = false;
  m_serverStartStep = SSS_NULL;
}

QString Server::getServerPath() {
  if (m_serverPath.isEmpty()) {
    m_serverPath = QCoreApplication::applicationDirPath();
    QFileInfo fileInfo(m_serverPath);

    if (m_serverPath.isEmpty() || !fileInfo.isFile()) {
      QDir dir = fileInfo.dir();
      while (dir.dirName() != "QtScrcpy" && !dir.isRoot()) {
        dir.cdUp();
      }

      m_serverPath = dir.path() + "/third_party/scrcpy-server.jar";
    }
  }

  return m_serverPath;
}

bool Server::startServerStep() {
  bool stepSeccess = false;

  switch (m_serverStartStep) {
    case SSS_NULL:
      break;
    case SSS_PUSH:
      stepSeccess = pushServer();
      break;
    case SSS_ENABLE_REVERSE:
      stepSeccess = enableReverse();
      break;
    case SSS_EXEC_SERVER:
      m_serverSocket.setMaxPendingConnections(MAX_CONNECTIONS);
      if (!m_serverSocket.listen(QHostAddress::LocalHost, m_localPort)) {
        qCritical(QString("Could not listen on port %1").arg(m_localPort).toStdString().c_str());
        m_serverStartStep = SSS_NULL;
        disableReverse();
        removeServer();
        emit serverStartResult(false);
        return false;
      }
      stepSeccess = executeServer();
      break;
    case SSS_RUNNING:
      break;
  }

  if (!stepSeccess) {
    emit serverStartResult(false);
  }

  return stepSeccess;
}

bool Server::pushServer() {
  m_workProcess.push(m_serial, getServerPath(), DEVICE_SERVER_PATH);
  return true;
}

bool Server::removeServer() {
  if (!m_serverCopiedToDevice || m_serial.isEmpty()) {
    return true;
  }
  m_serverCopiedToDevice = false;

  // 创建临时进程对象
  AdbProcess* adb = new AdbProcess(this);

  // 执行删除命令
  adb->removePath(m_serial, DEVICE_SERVER_PATH);

  // 同步等待adb命令完成，而不是异步回调
  bool finished = adb->waitForFinished(5000);  // 等待5秒
  if (!finished) {
    qWarning("Remove server timeout, killing adb process");
    adb->kill();
    adb->waitForFinished(1000);
  }

  adb->deleteLater();
  return true;
}

bool Server::enableReverse() {
  m_workProcess.reverse(m_serial, SOCKET_NAME, m_localPort);
  return true;
}

bool Server::executeServer() {
  // adb shell CLASSPATH=/data/local/tmp/scrcpy-server.jar app_process /
  // com.genymobile.scrcpy.Server maxsize bitrate false
  QStringList args;
  args << "shell" << QString("CLASSPATH=%1").arg(DEVICE_SERVER_PATH) << "app_process"
       << "/"
       << "com.genymobile.scrcpy.Server" << QString::number(m_maxSize) << QString::number(m_bitRate)
       << "false"
       << "";

  m_serverProcess.execute(m_serial, args);

  return true;
}

bool Server::disableReverse() {
  if (!m_enableReverse || m_serial.isEmpty()) {
    return true;
  }

  // 创建临时进程对象
  AdbProcess* adb = new AdbProcess(this);

  // 执行删除端口映射命令
  adb->removeReverse(m_serial, SOCKET_NAME);

  // 同步等待完成
  bool finished = adb->waitForFinished(3000);
  if (!finished) {
    qWarning("Disable reverse timeout, killing adb process");
    adb->kill();
    adb->waitForFinished(1000);
  }

  m_enableReverse = false;
  adb->deleteLater();
  return true;
}

bool Server::readInfo(QString& deviceName, QSize& size) {
  if (!m_deviceSocket) {
    qWarning("Device socket is null when reading info");
    return false;
  }

  // abk001-----------------------0x0438 0x02d0
  //               64b            2b w   2b h
  unsigned char buffer[DEVICE_NAME_FIELD_LENGTH + 4];
  if (m_deviceSocket->bytesAvailable() <= DEVICE_NAME_FIELD_LENGTH + 4) {
    if (!m_deviceSocket->waitForReadyRead(300)) {
      qWarning("Timeout waiting for device info");
      return false;
    }
  }

  qint64 len = m_deviceSocket->read((char*)buffer, sizeof(buffer));
  if (len < DEVICE_NAME_FIELD_LENGTH + 4) {
    qInfo("Could not read device info");
    return false;
  }

  buffer[DEVICE_NAME_FIELD_LENGTH - 1] = '\0';
  deviceName = QString::fromUtf8((char*)buffer);
  size.setWidth((buffer[DEVICE_NAME_FIELD_LENGTH] << 8 | buffer[DEVICE_NAME_FIELD_LENGTH + 1]));
  size.setHeight(
      (buffer[DEVICE_NAME_FIELD_LENGTH + 2] << 8 | buffer[DEVICE_NAME_FIELD_LENGTH + 3]));

  return true;
}

DeviceSocket* Server::getDeviceSocket() const {
  return m_deviceSocket;
}

Server::~Server() {
  stopServer();

  // 如果设置了清理标志，则尝试删除服务器
  if (m_serverCopiedToDevice) {
    removeServer();
  }
}