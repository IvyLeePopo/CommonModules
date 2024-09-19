#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>
#include <QTimer>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include <QPair>
#include <QThread>
#include <QTextCodec>
#include "DevMananger.h"

class WebSocketServer : public QObject
{
    Q_OBJECT
public:
    static WebSocketServer* getInstance();
    static WebSocketServer* m_pInstance;
    virtual ~WebSocketServer();
    QDateTime *pCurrentTime;
    quint16 m_nPort = 8899;
    bool m_IsConnect = false;

public:
    bool StartServer();
    bool StopServer();
    bool SetPort(int nPort);
    void SendMsg(QString strData);
    bool parseHeartBeatMsg(QByteArray szData);

public slots:
    void OnNewConnection();
    void MsgReceived(QString strMsg);
    void Disconnected();
    void SlotSendLogToWebSocket(int cmd, int executeCode, QString resultMsg);
    void slotSendMsg(int ret);

signals:
    void SignalKillApp();

private:
    WebSocketServer();
    QWebSocket* m_pSocket = nullptr;
    QWebSocketServer* m_pServer;
    QTimer *m_timer;
};

#endif // WEBSOCKETSERVER_H
