/*
 * @Description: websocket服务器
 */
#ifndef SYNCABNOTESERVER_H
#define SYNCABNOTESERVER_H
#include<QObject>

class QWebSocketServer;
class QWebSocket;
class QString;
class QObject;

class WebSocketServer : public QObject
{
    Q_OBJECT

public:
    WebSocketServer(QString serverName,  quint16 port);
    virtual ~WebSocketServer();

public:
    //检测是否存在客户端
    bool hadClients();

    //获取本机IP
    QString getLocalHostIP();

signals:
    //智能节点A回馈数据的信号
    void sigProcessServerMessage(const QString & strMessage);

public slots:   
    //发送文本给所有客户端
    void slotSendToAllClients(const QString &strMessage);

    //启动websocket服务器
    void slotStartServer();

private slots:
    //处理新接入的连接
    void slotNewConnection();

    //接收数据，并转发给其他所有客户端节点
    void slotProcessMessage(const QString &strMessage);

    //处理链接断开的事件
    void slotSocketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;	//服务器
    QList<QWebSocket *> m_clients;			//连接服务器的客户端列表

    unsigned short m_nPort;
    QString m_strServerName;
};

#endif // SYNCABNOTESERVER_H
