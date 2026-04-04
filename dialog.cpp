#include <QDebug>
#include <QDir>

#include "dialog.h"
#include "ui_dialog.h"
#include "adbprocess.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}


Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_TestButton_clicked()
{
    // 获取当前工作路径
    // qDebug()<<"current applicationDirPath: "<< QCoreApplication::applicationDirPath();
    // qDebug()<<"current currentPath: "<<QDir::currentPath();

    QStringList arguments;
    arguments << "devices";

    AdbProcess *myProcess = new AdbProcess(this);

    connect(myProcess, &AdbProcess::adbProcessResult, this, [this](AdbProcess::ADB_EXEC_RESULT processResult){
        qDebug() << ">>>>>>>>" << processResult;
    });

    /**
    command：启动的程序   arguments：外部程序的参数   mode：进程的模式
    void start(const QString &program, const QStringList &arguments, OpenMode mode = ReadWrite);
    */
    myProcess->execute("", arguments);
}

