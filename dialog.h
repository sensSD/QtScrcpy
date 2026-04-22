#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPointer>

#include "Controller.h"
#include "Decoder.h"
#include "Frames.h"
#include "QYUVOpenGLWidget.h"
#include "server.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

class Dialog : public QDialog {
  Q_OBJECT

 public:
  Dialog(QWidget* parent = nullptr);

  ~Dialog();

 private slots:
  void on_startServerBtn_clicked();
  void on_stopServerBtn_clicked();
  void on_mouseEventBtn_clicked();

 private:
  Server m_server;
  Decoder m_decoder;
  Frames m_frames;
  Controller m_controller;
  QPointer<QYUVOpenGLWidget> m_videoWidget;

  Ui::Dialog* ui;
};
#endif  // DIALOG_H
