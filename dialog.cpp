#include "dialog.h"

#include <QDebug>
#include <QDir>
#include <memory>

#include "ControlEvent.h"
#include "Decoder.h"
#include "ui_dialog.h"


Dialog::Dialog(QWidget* parent) : QDialog(parent), ui(new Ui::Dialog) {
  ui->setupUi(this);

  connect(&m_server, &Server::serverStartResult, this,
          [this](bool success) { qDebug() << "serverStartResult" << success; });

  connect(&m_server, &Server::connectToResult, this,
          [this](bool success, QString deviceName, QSize size) {
            qDebug() << "connectToResult" << success;
            if (success) {
              m_decoder.setDeviceSocket(m_server.getDeviceSocket());
              m_decoder.startDecode();

              m_controller.setDeviceSocket(m_server.getDeviceSocket());
            }
          });

  m_frames.init();
  m_decoder.setFrames(&m_frames);

  connect(&m_decoder, &Decoder::onNewFrame, this, [this] {
    qDebug() << "onNewFrame";
    m_frames.lock();
    const AVFrame* frame = m_frames.consumeRenderingFrame();
    // 渲染视频帧
    m_videoWidget->setFrameSize(QSize(frame->width, frame->height));
    m_videoWidget->updateTextures(frame->data[0], frame->data[1], frame->data[2],
                                  frame->linesize[0], frame->linesize[1], frame->linesize[2]);
    m_frames.unLock();
  });
  m_videoWidget = new QYUVOpenGLWidget(nullptr);
  m_videoWidget->resize(QSize(420, 850));
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
  m_videoWidget->show();
}

void Dialog::on_mouseEventBtn_clicked() {
  if (!m_server.getDeviceSocket()) {
    qDebug() << "Device socket is not connected.";
    return;
  }
}
