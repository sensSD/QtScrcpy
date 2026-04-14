#include <QDebug>
#include <QDir>

#include "Decoder.h"
#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog) {
  ui->setupUi(this);

  connect(&m_server, &Server::serverStartResult, this,
          [this](bool success) { qDebug() << "serverStartResult" << success; });

  connect(&m_server, &Server::connectToResult, this,
          [this](bool success, QString deviceName, QSize size) {
            qDebug() << "connectToResult" << success;
            if (success) {
              m_decoder.setDeviceSocket(m_server.getDeviceSocket());
              m_decoder.startDecode();
            }
          });

  m_frames.init();
  m_decoder.setFrames(&m_frames);

  connect(&m_decoder, &Decoder::onNewFrame, this,
          [this] { qDebug() << "onNewFrame"; });
}

Dialog::~Dialog() {
  m_frames.deInit();
  delete ui;
}

void Dialog::on_stopServerBtn_clicked() {
  m_server.stopServer();
}

void Dialog::on_startServerBtn_clicked() {
  m_server.startServer("", 5678, 720, 8000000);
}
