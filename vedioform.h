#ifndef VEDIOFORM_H
#define VEDIOFORM_H

#include <QWidget>

#include "Controller.h"
#include "Decoder.h"
#include "Frames.h"
#include "Server.h"

namespace Ui {
class vedioForm;
}

class vedioForm : public QWidget {
  Q_OBJECT

 public:
  explicit vedioForm(const QString& serial, QWidget* parent = nullptr);
  ~vedioForm();

 private:
  QString m_serial;
  Server m_server;
  Decoder m_decoder;
  Frames m_frames;
  Controller m_controller;
  Ui::vedioForm* ui;
};

#endif  // VEDIOFORM_H
