/*
	������Ҫ�Ƿ�װ��websocket�Ļ��࣬����libwebsockets��Դ�� 
*/
#pragma once
#include <list>
#include <map>
#include "ThreadBase.h"
#include "libwebsockets.h"

#define LOGTYPE "_log_type:"

// �ص���������websocket�Ľ���׸��ϲ�
typedef void(*recvWebSocketMsg)(void* pUser, const char* pszData, int nSize);

// ÿһ��websocket�������̻߳��࣬��Ҫ���ڹ����Լ�����Ϣ
class WebSocketBase : public SingleTon<WebSocketBase>, public CThreadBase
{
	friend class SingleTon<WebSocketBase>;

public:
	WebSocketBase(void);
	virtual ~WebSocketBase(void);
	virtual int circle();

public:
	// ��ʼ��
	void init();

	// ����ʼ��
	void unInit();

	// �Ƿ��Ѿ�����
	bool isConnected() { return m_bIsConnected; }
	bool isErrorDisonnect() { return m_bErrorDisconnect; }

	// �Ƿ������ݳɹ�
	int isSended() { return m_nSendResult; }

	// �������ӱ�־
	void resetConnected(bool bConnected) { m_bIsConnected = bConnected; }
	void reseErrortDisconnect(bool bDisconnect) { m_bErrorDisconnect = bDisconnect; }

	// ���ӷ�����
	bool connect(const char* pszUrl);
	bool connect(const char* pszIp, int nPort, string& strErr);

	// �Ͽ�����
	bool disConnect();

	// �Ͽ�����
	bool autoConnect();

	// ������Ϣ
	bool send(const char* pszJson, unsigned int  nSize);

	// ��ȡ���lws��websocketbase
	static void* getLwsToWs(lws* pLws);


public:
	// ���ûص�����
	void setCallBack(recvWebSocketMsg pFunc, void* pUser);

	// �������˷�����
	void connectResult(bool bConnect);
	void getConnectStatus(int nConncet);		//-1-δ���ӣ�0-�ͻ��˶Ͽ���1-����˶Ͽ�

	// ���ѷ�����Ϣ����
	void consumeSendMsg();

	// ��־���ͣ�����recvWebSocketMsg�ص�
	void writeLog(const char* pszData);

	// ������Ϣ
	bool recvMsgByCallBack(const char* pszData, int nSize);

private:
	int m_nPort;												// websocket �˿�
	string m_strIp;												// websocket ip
	char m_szHost[256];											// ���������
	int m_nSendResult;											// �Ƿ������ݳɹ�
	bool m_bIsConnected;										// �Ƿ��Ѿ��������˷�����
	bool m_bErrorDisconnect;									// �Ƿ��쳣�Ͽ�����
	list<pair<int, char*>> m_listMsg;							// �ȴ����͵���Ϣ����
	int m_nReconnectTimes;										// �����Ĵ���
	string m_strLog;											// ˽�����͵���־

	struct lws* m_pWsi;											// websocket���Ӿ��
	struct lws_context* m_pContext;								// websocket������
	struct lws_context_creation_info* m_pLwsCtxInfo;			// context����
	struct lws_protocols m_pLwsProtocols[2];					// Э��㴫����
	
	void* m_pUser;												// ���ػص��Ķ���
	struct lws_client_connect_info m_tagConnInfo;				// ���Ӳ���
	recvWebSocketMsg m_pRecvWebSocketFunc;						// �ص�

	static map<lws*, void*> m_mapLwsToWs;						// ���lws��websocketbase
};

#define WebSocketBaseClient WebSocketBase::getInstance()