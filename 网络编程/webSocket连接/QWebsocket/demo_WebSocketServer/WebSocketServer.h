#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>

class WebSocketServer : public QObject
{
    Q_OBJECT
public:
    static WebSocketServer* getInstance();
    static WebSocketServer* m_pInstance;
    virtual ~WebSocketServer();
    bool StartServer();
    bool StopServer();
    bool SetPort(int nPort);


protected:
    WebSocketServer();
    QWebSocket* m_pSocket = nullptr;
    QWebSocketServer* m_pServer;
    quint16 m_nPort = 8899;
    bool m_IsConnect = false;

public slots:
    void OnNewConnection();
    void MsgReceived(QString strMsg);
    void Disconnected();
    void SlotSendMsg(QString strData);
};

#endif // WEBSOCKETSERVER_H
