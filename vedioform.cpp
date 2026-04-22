#include "vedioForm.h"

#include <qnamespace.h>

#include "ui_vedioForm.h"

vedioForm::vedioForm(const QString& serial, QWidget* parent)
    : QWidget(parent), ui(new Ui::vedioForm), m_serial(serial) {
  ui->setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);

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

  connect(&m_server, &Server::onServerStopped, this, [this] {
    close();
    qDebug() << "Server stopped";
  });

  connect(&m_decoder, &Decoder::onDecodeStop, this, [this]() {
    close();
    qDebug() << "Decode stopped";
  });

  connect(&m_decoder, &Decoder::onNewFrame, this, [this] {
    qDebug() << "onNewFrame";
    m_frames.lock();
    const AVFrame* frame = m_frames.consumeRenderingFrame();
    // 渲染视频帧
    ui->vedioWidget->setFrameSize(QSize(frame->width, frame->height));
    ui->vedioWidget->updateTextures(frame->data[0], frame->data[1], frame->data[2],
                                    frame->linesize[0], frame->linesize[1], frame->linesize[2]);
    m_frames.unLock();
  });

  m_server.startServer(m_serial, 5678, 720, 8000000);
}

vedioForm::~vedioForm() {
  m_server.stopServer();
  m_decoder.stopDecode();
  m_frames.deInit();
  delete ui;
}
