#pragma once

#include <QMutex>
#include <QTcpSocket>
#include <QWaitCondition>

class DeviceSocket : public QTcpSocket {
  Q_OBJECT

public:
  explicit DeviceSocket(QObject *parent = nullptr);
  ~DeviceSocket();

  /**
   * @brief 生产者线程调用，接收数据
   * 
   * @param buf 
   * @param bufSize 
   * @return qint32 
   */
  qint32 subThreadRecvData(quint8 *buf, qint32 bufSize);

protected:
  /**
   * @brief 发送事件与主线程通信
   * 
   * @param event 
   * @return true 
   * @return false 
   */
  bool event(QEvent *event) override;

protected slots:
  /**
   * @brief 消费者线程调用，接收数据事件的槽函数
   * 
   */
  void onReadyRead();

  /**
   * @brief 退出DeviceSocket
   * 
   */
  void quitNotify();

private:
  // 锁
  QMutex m_mutex;
  QWaitCondition m_recvDataCond;

  // 标志
  bool m_recvData = false;
  bool m_quit = false;

  // 数据缓存
  quint8* m_buffer;
  qint32 m_bufferSize = 0;
  qint32 m_dataSize = 0;
};
