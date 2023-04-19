/*
 * @Description: websocket客户端，用于与HardwareControl_App通信
 */
#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>

class QTimer;
class QWebSocket;
class WebSocketClient : public QObject
{
    Q_OBJECT
public:
    WebSocketClient(QObject *parent = nullptr);
    ~WebSocketClient();

    void setWebSocketUrl(QString strUrl="");
    void setWebSocketType(int iWsType = 0);

    bool m_bConnectStatus;         /*-<websocket连接状态，连接成功：true；断开：false */
    QTimer *m_timer;            /*-<周期重连Timer */
signals:
    void sigClientTextMessageReceived(const QString &message); //借用websocket的信号函数

public slots:
    void slotCreateDataRecWS();//创建websocket连接
    void slotSendTextMessage(const QString &message);
    void slotReconnect();           /*-<周期重连函数 */
    void slotActiveReconnect();

private slots:
    void slotOnConnected();                 /*-<socket建立成功后，触发该函数 */
    void slotOnTextReceived(const QString& msg);   /*-<收到Sev端的数据时，触发该函数 */
    void slotOnDisConnected();              /*-<socket连接断开后，触发该函数 */

private:
    QWebSocket *m_pDataRecvWS;     /*-<websocket类 */

    QString m_strURL;              /*连接URL*/
    int m_nWsType;                 //区分多个webSocket客户端
};

#endif // WEBSOCKETCLIENT_H
