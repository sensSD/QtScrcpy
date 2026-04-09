#include <QDebug>
#include <QDir>

#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    connect(&m_server, &Server::serverStartResult, this, [this](bool success){
        qDebug() << "serverStartResult" << success;
    });

    connect(&m_server, &Server::connectToResult, this, [this](bool success, QString deviceName, QSize size) {
        qDebug() << "connectToResult" << success;
        if(success) {
            qDebug() << "deviceName:" << deviceName << "size:" << size;
        }
    });
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_stopServerBtn_clicked() {
    m_server.stopServer();
}

void Dialog::on_startServerBtn_clicked()
{
    m_server.startServer("", 5678, 720, 8000000);
}

