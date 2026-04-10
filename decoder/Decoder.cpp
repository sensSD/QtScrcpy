#include "Decoder.h"
Decoder::Decoder(QThread *parent) : QThread{parent} {}

bool Decoder::init() { 
    if(avformat_network_init()) {
        return false;
    }
    return true; 
}