/*
 * @Description: websocket客户端，用于与中间件通信
 */

#pragma once
#include <QObject>
#include <QByteArray>

class QTimer;
class QWebSocket;
class WebSocketBase : public QObject
{
    Q_OBJECT
public:
    WebSocketBase(QObject *parent = nullptr);
    ~WebSocketBase();

    void setWebSocketUrl(QString strUrl="");
    bool getConnectStatus();
    int RecvFrom(QByteArray& byteArray);

signals:
    void sigClientBinaryMessageReceived(const QByteArray &byteArray); //借用websocket的信号函数

public slots:
    void slotCreateDataRecWS();//创建websocket连接
    void slotSendBinaryMessageMessage(const QByteArray &byteArray);
    void slotReconnect();           /*-<周期重连函数 */
    void slotActiveReconnect();

private slots:
    void slotOnConnected();                 /*-<socket建立成功后，触发该函数 */
    void slotOnBinaryMessageReceived(const QByteArray &byteArray);   /*-<收到Sev端的数据时，触发该函数 */
    void slotOnDisConnected();              /*-<socket连接断开后，触发该函数 */

private:
    QWebSocket *m_pDataRecvWS;     /*-<websocket类 */
    QTimer *m_pTimer;            /*-<周期重连Timer */
    QString m_strURL;              /*连接URL*/
    bool m_bConnectStatus;         /*-<websocket连接状态，连接成功：true；断开：false */
    QByteArray m_byteArray;
};
