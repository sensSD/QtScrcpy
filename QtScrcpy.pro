QMAKE_PROJECT_DEPTH = 0

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    dialog.cpp

HEADERS += \
    dialog.h

FORMS += \
    dialog.ui

# 子工程
include($$PWD/adb/adb.pri)
include($$PWD/server/server.pri)
include($$PWD/decoder/decoder.pri)

# 包含目录
INCLUDEPATH += \
    $$PWD/adb \
    $$PWD/server \
    $$PWD/enums \
    $$PWD/decoder \
    $$PWD/third_party/ffmpeg/include \

# 依赖模块
LIBS += \
    -L$$PWD/third_party/ffmpeg/lib -lavformat \
    -L$$PWD/third_party/ffmpeg/lib -lavcodec \
    -L$$PWD/third_party/ffmpeg/lib -lavutil \
    -L$$PWD/third_party/ffmpeg/lib -lswscale \
    -L$$PWD/third_party/ffmpeg/lib -libswresample \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
