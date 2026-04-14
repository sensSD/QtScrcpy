#include "Decoder.h"
#include "dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 设置环境变量
    qputenv("QSCRCPY_ADB_PATH", "..\\..\\..\\thrid_party\\adb\\win\\adb.exe");

    Decoder::init();
    
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    int ret = a.exec();

    Decoder::deInit();

    return ret;
}
