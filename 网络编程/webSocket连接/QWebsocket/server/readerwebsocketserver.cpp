#include "readerwebsocketserver.h"

#include <QThread>
#include <cstring>
#include "websocketserverbase.h"

ReaderWebsocketServer::ReaderWebsocketServer(QObject *parent)
    : QObject(parent)
{
    m_thdWsServer = new QThread();
    m_pWsServer = new WebsocketServerBase("reader", 9000);
    m_pWsServer->moveToThread(m_thdWsServer);

    connect(m_pWsServer, &WebsocketServerBase::sigProcessServerMessage, this, &ReaderWebsocketServer::slotProcessServerMessage);
    connect(this, &ReaderWebsocketServer::sigSendToAllClients, m_pWsServer, &WebsocketServerBase::slotSendToAllClients);

    connect(m_thdWsServer, &QThread::started, m_pWsServer, &WebsocketServerBase::slotStartServer);
    connect(m_thdWsServer, &QThread::finished, m_pWsServer, &WebsocketServerBase::deleteLater);
    connect(m_thdWsServer, &QThread::finished, m_thdWsServer, &WebsocketServerBase::deleteLater);
    m_thdWsServer->start();
}

ReaderWebsocketServer::~ReaderWebsocketServer()
{
    if(m_thdWsServer)
    {
        m_thdWsServer->quit();
        m_thdWsServer->wait();
    }

    if(m_pWsServer)
    {
        m_pWsServer->deleteLater();
    }
}

void ReaderWebsocketServer::setCallBack(recvMsgToSerial pFunc, void *pUser)
{
    if (nullptr != pFunc)
        m_pRecvMsgToSerial = pFunc;

    if (nullptr != pUser)
        m_pUser = pUser;
}

//接收串口发来数据，转发给读卡器客户端
bool ReaderWebsocketServer::sendData(const QByteArray &byteArray)
{
    bool hadBnode = m_pWsServer->hadClients();
    if (hadBnode)
    {
        emit sigSendToAllClients(byteArray);
    }
}

//处理读卡器发来的数据，转发给串口
void ReaderWebsocketServer::slotProcessServerMessage(const QByteArray &byteArray)
{
    qDebug() << __FUNCTION__ << "处理读卡器发来的数据，转发给串口";
    m_pRecvMsgToSerial(byteArray);
}
