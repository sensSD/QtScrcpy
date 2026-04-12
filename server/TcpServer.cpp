#include "TcpServer.h"
#include "DeviceSocket.h"
TcpServer::TcpServer(QObject *parent)
    : QTcpServer{parent}
{
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    DeviceSocket* socket = new DeviceSocket();
    socket->setSocketDescriptor(socketDescriptor);
    addPendingConnection(socket);
}
