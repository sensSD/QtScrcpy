#include <QDebug>
#include <QDir>

#include "dialog.h"
#include "ui_dialog.h"

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
    arguments << "shell" << "ip" << "-f" << "inet" << "addr" << "show" << "wlan0";

    myProcess = new AdbProcess(this);

    connect(myProcess, &AdbProcess::adbProcessResult, this, [this](AdbProcess::ADB_EXEC_RESULT processResult){
        qDebug() << ">>>>>>>>" << processResult;

        if(AdbProcess::AER_SUCCESS_EXEC == processResult) { 
            // qDebug() << myProcess->getDevicesSerialFromStdOut().join("*");
            qDebug() << myProcess->getDeviceIpFromStdOut();
        }
    });

    /**
    command：启动的程序   arguments：外部程序的参数   mode：进程的模式
    void start(const QString &program, const QStringList &arguments, OpenMode mode = ReadWrite);
    */

    // myProcess->push("", "E:\\@Code\\Qt\\Test\\test\\openMe.txt", "/sdcard/openMe.txt");
    // myProcess->removePath("", "/sdcard/openMe.txt");
    // myProcess->reverse("", "scrcpy", 8080);
    // myProcess->removeReverse("", "scrcpy");
    myProcess->execute("", arguments);
}

