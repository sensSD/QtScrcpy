#include "dialog.h"

#include <QDebug>
#include <QDir>

#include "ui_dialog.h"
#include "vedioForm.h"

Dialog::Dialog(QWidget* parent) : QDialog(parent), ui(new Ui::Dialog) {
  ui->setupUi(this);

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

void Dialog::on_mouseEventBtn_clicked() {
}
