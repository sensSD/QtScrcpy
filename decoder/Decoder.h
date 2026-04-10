#pragma once

#include <QThread>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

class Decoder: public QThread
{
    Q_OBJECT

public:
    static bool init();

    explicit Decoder(QThread *parent = nullptr);

signals:
};
