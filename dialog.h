#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPointer>

#include "adbprocess.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

class vedioForm;
class Dialog : public QDialog {
  Q_OBJECT

 public:
  Dialog(QWidget* parent = nullptr);
  ~Dialog();

 public:
  /**
   * @brief 输出日志
   * 
   * @param log 
   * @param newLine 
   */
  void outLog(const QString& log, bool newLine = true);


 private slots:
  void on_startServerBtn_clicked();
  void on_stopServerBtn_clicked();
  void on_updateDevicesBtn_clicked();
  void on_startAdbdBtn_clicked();
  void on_getIpBtn_clicked();
  void on_wirelessConnectBtn_clicked();

 private:
  QPointer<vedioForm> m_vedioForm;
  AdbProcess m_adb;

  Ui::Dialog* ui;
};
#endif  // DIALOG_H
