#pragma once

#include <QObject>
#include <QPointer>

class DeviceSocket;
class ControlEvent;
class Controller : public QObject {
  Q_OBJECT

 public:
  explicit Controller(QObject* parent = nullptr);

  /**
   * @brief 设置设备套接字
   *
   * @param deviceSocket
   */
  void setDeviceSocket(DeviceSocket* deviceSocket);

  /**
   * @brief 发布控制事件
   *
   * @param controlEvent
   */
  void postControlEvent(ControlEvent* controlEvent);

  /**
   * @brief
   *
   * @param rc
   */
  void test(QRect rc);

 protected:
  bool event(QEvent* event) override;

 private:
  /**
   * @brief 发送控制事件数据到设备
   *
   * @param buffer
   * @return true
   * @return false
   */
  bool sendControl(const QByteArray& buffer);

 private:
  QPointer<DeviceSocket> m_deviceSocket;
};
