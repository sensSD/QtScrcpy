#include "dialog.h"

#include <QDebug>
#include <QDir>
#include <QTimer>

#include "ui_dialog.h"
#include "vedioForm.h"

Dialog::Dialog(QWidget* parent) : QDialog(parent), ui(new Ui::Dialog) {
  ui->setupUi(this);

  connect(&m_adb, &AdbProcess::adbProcessResult, this,
          [this](AdbEnums::ADB_EXEC_RESULT prcessResult) {
            QString log = "";
            bool newLine = true;
            switch (prcessResult) {
              case AdbEnums::ADB_EXEC_RESULT::AER_SUCCESS_START:
                log = "adb run";
                newLine = false;
                break;
              case AdbEnums::ADB_EXEC_RESULT::AER_ERROR_EXEC:
                log = m_adb.getStdError();
                break;
              case AdbEnums::ADB_EXEC_RESULT::AER_ERROR_MISSING_BINARY:
                log = "adb not find";
                break;
              case AdbEnums::ADB_EXEC_RESULT::AER_SUCCESS_EXEC:
                QStringList args = m_adb.arguments();
                if (args.contains("devices")) {
                  QStringList devices = m_adb.getDevicesSerialFromStdOut();
                  if (!devices.isEmpty()) {
                    ui->serialEdt->setText(devices.at(0));
                  }
                }
                if (args.contains("show") && args.contains("wlan0")) {
                  QString ip = m_adb.getDeviceIpFromStdOut();
                  if (!ip.isEmpty()) {
                    ui->ipAddrEdt->setText(ip);
                  }
                }
                break;
              defalt:
                break;
            }

            if (!log.isEmpty()) {
              outLog(log, newLine);
            }
          });
}

Dialog::~Dialog() {
  delete ui;
}

void Dialog::on_stopServerBtn_clicked() {
  if (m_vedioForm) {
    m_vedioForm->close();
  }
}

void Dialog::on_startServerBtn_clicked() {
  if (!m_vedioForm) {
    m_vedioForm = new vedioForm("");
  }

  m_vedioForm->show();
}

void Dialog::outLog(const QString& log, bool newLine) {
  QString tmpLog = log;
  bool tmpNewLine = newLine;
  QTimer::singleShot(0, this, [this, tmpLog, tmpNewLine]() {
    ui->outTxtEdt->append(tmpLog);
    if (tmpNewLine) {
      ui->outTxtEdt->append("<br/>");
    }
  });
}

void Dialog::on_updateDevicesBtn_clicked() {
  outLog("update devices...", false);
  m_adb.execute("", QStringList() << "devices");
}

void Dialog::on_startAdbdBtn_clicked() {
  outLog("start devices adbd...", false);
  // adb tcpip 5555
  QStringList adbArgs;
  adbArgs << "tcpip";
  adbArgs << "5555";
  m_adb.execute(ui->serialEdt->text().trimmed(), adbArgs);
}

void Dialog::on_getIpBtn_clicked() {
  outLog("get ip...", false);

  // adb shell ip -f inet addr show wlan0
  QStringList adbArgs;
  adbArgs << "shell"
          << "ip"
          << "-f"
          << "inet"
          << "addr"
          << "show"
          << "wlan0";
  m_adb.execute(ui->serialEdt->text().trimmed(), adbArgs);
}

void Dialog::on_wirelessConnectBtn_clicked() {
  outLog("wireless connect...", false);
  QString addr = ui->ipAddrEdt->text().trimmed();
  if (!ui->portEdt->text().isEmpty()) {
    addr += ":";
    addr += ui->portEdt->text().trimmed();
  }
  // connect ip:port
  QStringList adbArgs;
  adbArgs << "connect";
  adbArgs << addr;
  m_adb.execute(ui->serialEdt->text().trimmed(), adbArgs);
}