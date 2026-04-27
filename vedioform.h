#ifndef VEDIOFORM_H
#define VEDIOFORM_H

#include <QWidget>

#include "Decoder.h"
#include "Frames.h"
#include "Server.h"
#include "InputConvertGame.h"

namespace Ui {
class vedioForm;
}

class vedioForm : public QWidget {
  Q_OBJECT

 public:
  explicit vedioForm(const QString& serial, QWidget* parent = nullptr);
  ~vedioForm();

 protected:
  virtual void mousePressEvent(QMouseEvent* event);
  virtual void mouseReleaseEvent(QMouseEvent* event);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void wheelEvent(QWheelEvent* event);
  virtual void keyPressEvent(QKeyEvent* event);
  virtual void keyReleaseEvent(QKeyEvent* event);

 private:
  void updateShowSize(const QSize& newSize);

 private:
  QString m_serial;
  Server m_server;
  Decoder m_decoder;
  Frames m_frames;
  Ui::vedioForm* ui;
  InputConvertGame m_inputConvert;
  QSize m_frameSize;
};

#endif  // VEDIOFORM_H
