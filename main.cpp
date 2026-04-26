#include <QApplication>
#include <QDir>
#include <QFile>

#include "Decoder.h"
#include "dialog.h"

int main(int argc, char* argv[]) {
  // 设置环境变量
  qputenv("QSCRCPY_ADB_PATH", "..\\..\\..\\thrid_party\\adb\\win\\adb.exe");

  QApplication a(argc, argv);

  QDir::addSearchPath("icon", ":/theme/active");
  QDir::addSearchPath("icon", ":/theme/disabled");
  QDir::addSearchPath("icon", ":/theme/primary");

  QFile styleFile(":/theme/dark_teal.qss");
  if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    a.setStyleSheet(styleFile.readAll());
    styleFile.close();
  } else {
    qWarning() << "无法加载主题文件: " << styleFile.fileName();
  }

  Decoder::init();

  Dialog w;
  w.show();

  int ret = a.exec();

  Decoder::deInit();

  return ret;
}
