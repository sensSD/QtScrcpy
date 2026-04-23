#include "vedioForm.h"

#include <QTimer>

#include "ui_vedioForm.h"

#define VIDEO_FROM_WIDTH 420
#define VIDEO_FROM_HEIGHT 850

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
              setWindowTitle(deviceName);
              updateShowSize(size);

              m_decoder.setDeviceSocket(m_server.getDeviceSocket());
              m_decoder.startDecode();

              m_inputConvert.setDeviceSocket(m_server.getDeviceSocket());
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
    // qDebug() << "onNewFrame";
    m_frames.lock();
    const AVFrame* frame = m_frames.consumeRenderingFrame();

    updateShowSize(QSize(frame->width, frame->height));

    // 渲染视频帧
    ui->vedioWidget->setFrameSize(QSize(frame->width, frame->height));
    ui->vedioWidget->updateTextures(frame->data[0], frame->data[1], frame->data[2],
                                    frame->linesize[0], frame->linesize[1], frame->linesize[2]);
    m_frames.unLock();
  });
  updateShowSize(size());

  QTimer::singleShot(0, this, [this]() { m_server.startServer(m_serial, 27183, 720, 8000000); });
}

vedioForm::~vedioForm() {
  m_server.stopServer();
  m_decoder.stopDecode();
  m_frames.deInit();

  delete ui;
}

void vedioForm::mousePressEvent(QMouseEvent* event) {
  m_inputConvert.mouseEvent(event, ui->vedioWidget->frameSize(), ui->vedioWidget->size());
}

void vedioForm::mouseReleaseEvent(QMouseEvent* event) {
  m_inputConvert.mouseEvent(event, ui->vedioWidget->frameSize(), ui->vedioWidget->size());
}

void vedioForm::mouseMoveEvent(QMouseEvent* event) {
  m_inputConvert.mouseEvent(event, ui->vedioWidget->frameSize(), ui->vedioWidget->size());
}

void vedioForm::wheelEvent(QWheelEvent* event) {
  m_inputConvert.wheelEvent(event, ui->vedioWidget->frameSize(), ui->vedioWidget->size());
}

void vedioForm::keyPressEvent(QKeyEvent* event) {
  m_inputConvert.keyEvent(event, ui->vedioWidget->frameSize(), ui->vedioWidget->size());
}

void vedioForm::keyReleaseEvent(QKeyEvent* event) {
  m_inputConvert.keyEvent(event, ui->vedioWidget->frameSize(), ui->vedioWidget->size());
}

void vedioForm::updateShowSize(const QSize& newSize) {
  if (m_frameSize != newSize) {
    m_frameSize = newSize;

    bool vertical = newSize.height() > newSize.width();
    if (vertical) {
      resize(VIDEO_FROM_WIDTH, VIDEO_FROM_HEIGHT);
    } else {
      resize(VIDEO_FROM_HEIGHT, VIDEO_FROM_WIDTH);
    }
  }
}