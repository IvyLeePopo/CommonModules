#ifndef WEBSOCKETSERVERBASE_H
#define WEBSOCKETSERVERBASE_H

#include <QObject>
#include <QtWebSockets>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)

class WebsocketServerBase : public QObject
{
    Q_OBJECT
public:
    explicit WebsocketServerBase(QString serverName,  quint16 port, QObject *parent = 0);
    virtual ~WebsocketServerBase();

signals:
    //读卡器发来的数据
    void sigProcessServerMessage(const QByteArray &data);

public slots:
    //发送数据给读卡器
    void slotSendToAllClients(const QByteArray &data);

    //启动websocket服务器
    void slotStartServer();

private slots:
    //处理新接入的连接
    void slotNewConnection();

    //处理链接断开的事件
    void slotSocketDisconnected();

    //接收数据，并转发给串口库
    void slotProcessBinaryMessage(const QByteArray &message);

public:
    //检测是否存在客户端
    bool hadClients();

private:
    QWebSocketServer *m_pWebSocketServer = nullptr;
    QList<QWebSocket *> m_clients;

    unsigned short m_nPort = 9000;
    QString m_strServerName = "server";
};

#endif // WEBSOCKETSERVERBASE_H
