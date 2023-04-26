#include "TcpClient.h"

#include <QTimer>

#define MSG_HEAD_LEN    4    //帧头的字节长度（02 03 04 05）
#define MSG_LENGTH_LEN  4    //表示帧长度的数值的字节长度（XX XX XX XX，unsigned int型）
#define MSG_TAIL_LEN    1    //帧尾的字节长度（03）
#define MSG_FRONT_LEN   MSG_HEAD_LEN + MSG_LENGTH_LEN    //单帧DATA域之前的字节长度
#define MSG_MIN_LEN     MSG_HEAD_LEN + MSG_LENGTH_LEN + MSG_TAIL_LEN    //单帧最小字节长度

#define TCP_HEART_BEAT_INTERVAL 10 * 1000  //心跳包发送间隔
#define TCP_RECONNECT_INTERVAL 10 * 1000  //发起重连间隔

#define MSG_TYPE_HEARTBEAT  "Heartbeat" //心跳 Heartbeat
#define EVENT_HeartbeatRsp  "HeartbeatRsp" //心跳回复

#define DELETEBUFF(_object) \
    if(NULL != _object) \
    {\
        delete [] _object; \
        _object = NULL; \
    }

enum EnumResult
{
	EnumResult_OK = 0,                      // 执行成功
	EnumResult_ConnectFail = 400,           // 连接服务失败
	EnumResult_TimeOut = 401,               // 超时错误
	EnumResult_DataNone = 402,              // 数据为空
	EnumResult_DataErr = 403,               // 数据错误
	EnumResult_FrameIncomplete = 404,       // 数据有分包
};

// pParent 业务基础管理类
// 每个基础管理类，都new *TcpClient，收到信息，统一由此发出去
TcpClient::TcpClient(void* pParent, int nDeviceType)
    : m_nDeviceType(nDeviceType)
{
    m_pParent = pParent;
    m_pTimerHeartBeat = new QTimer();
    m_pTimerHeartBeat->setSingleShot(false);
    connect(m_pTimerHeartBeat, &QTimer::timeout, this, &TcpClient::slotHearBeat);

    m_pTimerReconnect = new QTimer();
    m_pTimerReconnect->setSingleShot(false);
    connect(m_pTimerReconnect, &QTimer::timeout, this, &TcpClient::slotReConnect);
}

TcpClient::~TcpClient()
{

}

// 具体见协议，协议规定的数据结构tagTcpDataParams
int TcpClient::send(tagTcpDataParams& param, bool bLog)
{
    // 套上json外壳
    string strData = TcpClientParse::getInstance()->generateBaseData(param);

    if (bLog) {
        WriteLog(EnumLogType_Info, Helper->format("发送网络数据(%s): 设备类型:%s 设备ID:%s 数据:%s",
            getParentType().c_str(),
            param.strDeviceType.c_str(),
            param.strDeviceId.c_str(),
            strData.c_str()),
            __LINE__, __FUNCTION__);
    }
    // 套上消息格式外壳
    /* 帧头    帧长度  数据域  帧尾 */
    /* 4位      4位  json数据  1位 */
    /*02030405                 03 */
    
    int nSendDataLen = 9 + strData.size() + 1;        // 多加1位，在拷贝发送的时候再减去就行
    char* pszSendData = new char[nSendDataLen];
    memset(pszSendData, 0, nSendDataLen);
    pszSendData[0] = 0x02;
    pszSendData[1] = 0x03;
    pszSendData[2] = 0x04;
    pszSendData[3] = 0x05;

    int nDataLen = strData.size() + 1;                // 协议的长度竟然还包括了帧尾，不讲武德
	pszSendData[4] = 0xFF & (nDataLen >> 24);
	pszSendData[5] = 0xFF & (nDataLen >> 16);
	pszSendData[6] = 0xFF & (nDataLen >> 8);
	pszSendData[7] = 0xFF & nDataLen;
    memcpy(pszSendData + 8 , strData.c_str(), strData.size());            // 拷贝数据
    pszSendData[nSendDataLen-2] = 0x03;

    int nRet = TcpClientBase::send(pszSendData, nSendDataLen-1);
    DELETEBUFF(pszSendData);
    return nRet;
}

int TcpClient::recv(const char *pszData, int nLen)
{
    if (nullptr == pszData || 0 == nLen)
    {
        return EnumResult_DataNone;
    }

    m_strUncompleteDatas.append(pszData, static_cast<size_t>(nLen));

    //如果下面的处理放在独立的program线程中处理，则需要对 m_strUncompleteDatas 进行加锁
    bool bIsSuccess = true;
    while (bIsSuccess) 
	{
       bIsSuccess =  splitFrame(m_strUncompleteDatas);//解包
    }

    if (m_queueFrameList.empty())
    {
        WriteLogWarning("warning : 有TCP分包.");
        return EnumResult_FrameIncomplete; //得不到完整的一帧数据，直接返回
    }

	//可能一次性发了几个包
    while (m_queueFrameList.size() > 0) {
        //到此为止，得到了一个完整的数据包；
        m_strEntireData = m_queueFrameList.front();
        m_queueFrameList.pop();

        ManagerBase* pManagerBase = nullptr;
        if(nullptr != m_pParent)
        {
            pManagerBase = reinterpret_cast<ManagerBase*>(m_pParent);//业务管理基类（管理多个管理类），主要用于信息的分发
        }

        pszData = m_strEntireData.c_str();

        /* 解析data */
        if (0x02 != pszData[0] ||
            0x03 != pszData[1] ||
            0x04 != pszData[2] ||
            0x05 != pszData[3])
        {
            return EnumResult_DataErr;
        }

        unsigned int nDstLen =
                (0xFF000000 & static_cast<unsigned int>(pszData[4] << 24)) +
                (0xFF0000 & (pszData[5] << 16)) +
                (0xFF00 & (pszData[6] << 8)) +
                (0xFF & pszData[7]);

        char* pszDst = new char[nDstLen];
        memset(pszDst, 0, nDstLen);
        memcpy(pszDst, pszData + 8, nDstLen - 1);       //注意算清数据长度   // -1,是因为车道服务把最后的帧尾也算到长度里边去了
        string strData = pszDst;
        DELETEBUFF(pszDst);

        tagTcpDataParams param;
        if(!TcpClientParse::getInstance()->parseBaseData(strData, param))
        {
            return EnumResult_DataErr;
        }

        if(nullptr != pManagerBase)
        {
            if(MSG_TYPE_HEARTBEAT == param.strMsgType && EVENT_HeartbeatRsp == param.strDataIdentifier)
            {
                // 心跳处理，暂时丢弃
                return EnumResult_OK;
            }

            if (MSG_TYPE_CMD == param.strMsgType) {

                if (EVENT_SendGlobalInfo == param.strDataIdentifier) {
                    // 设备车道全局信息，直接调用接口其他接口，直接处理即可
                    tagReqGlobalLaneInfo laneInfo;
                    if(TcpClientParse::getInstance()->parseReqLaneSignalData(param.strData, laneInfo))
                    {
                        WriteLogInfo(getParentType() + "已设置全局信息并回应");
                        return EnumResult_OK;
                    }
                }
            }

            if (param.strData.find("DataFlow") != string::npos && EVENT_CmdReply == param.strDataIdentifier) 
			{
                return EnumResult_OK;
            }

            //再分发给相应的业务类
            pManagerBase->cycle(reinterpret_cast<void*>(&param));
        }
    }

    return EnumResult_OK;
}

bool TcpClient::splitFrame(string &strUncomplete)
{
    size_t nFirstFrameOffset = 0;

    if (strUncomplete.length() < MSG_MIN_LEN)
    {
        return false;
    }

    //需要把‘0x02 0x03 0x04 0x05’之前的垃圾数据去掉，此处是为了找到‘0x02’，截取时需要 nFirstFrameOffset
    for (size_t i = 0; i < strUncomplete.length() - (MSG_HEAD_LEN - 1); i++)
    {
        if (   (strUncomplete[i]   == 0x02) &&
               (strUncomplete[i+1] == 0x03) &&
               (strUncomplete[i+2] == 0x04) &&
               (strUncomplete[i+3] == 0x05))
        {
            nFirstFrameOffset = i;
            break;
        }
    }

    //判断长度是否与数据包的长度匹配
    unsigned int nMsgLength = 0;
    if (strUncomplete.length() >= nFirstFrameOffset + MSG_MIN_LEN)
    {
        nMsgLength =
                (0xFF000000 & static_cast<unsigned int>(strUncomplete.at(nFirstFrameOffset + 4) << 24)) +
                (0xFF0000 & (strUncomplete.at(nFirstFrameOffset + 5) << 16)) +
                (0xFF00 & (strUncomplete.at(nFirstFrameOffset + 6) << 8)) +
                (0xFF & strUncomplete.at(nFirstFrameOffset + 7));
    }
    else {
        return false;
    }

    string oneFrame = strUncomplete.substr(nFirstFrameOffset, static_cast<size_t>(nMsgLength) + MSG_FRONT_LEN);
    if (static_cast<size_t>(nMsgLength) + MSG_FRONT_LEN == oneFrame.length())
    {
        m_queueFrameList.push(oneFrame);
		strUncomplete.erase(0, nFirstFrameOffset + (static_cast<size_t>(nMsgLength) + MSG_FRONT_LEN));//把已经读出来的字节删掉
        return true;
    }

    return false;
}

void TcpClient::slotHearBeat()
{
	// 心跳自动发送信号
	tagTcpDataParams param;//见协议的具体数据格式
	param.strMsgId = Helper->getMsgId();
	param.strMsgType = MSG_TYPE_HEARTBEAT;
	param.strData = TcpClientParse::getInstance()->generateHeartBeatData();
	send(param, false);
}

void TcpClient::slotConnected()
{
	WriteLogInfo(getParentType() + "的tcp连接已建立");

	m_bIsConnected = true;

	//启动心跳计时器
	m_pTimerHeartBeat->start(TCP_HEART_BEAT_INTERVAL);

	//停止重连定时器
	m_pTimerReconnect->stop();
}

void TcpClient::slotDisConnected()
{
	WriteLogWarning(getParentType() + "的tcp连接已断开，稍后将自动重连");

	m_bIsConnected = false;

	//断线后清空缓存，否则后续数据无法正确解析
	m_strUncompleteDatas.clear();

	//停止心跳计时器
	m_pTimerHeartBeat->stop();

	//启动重连定时器
	m_pTimerReconnect->start(TCP_RECONNECT_INTERVAL);
}

void TcpClient::slotReConnect()
{
	WriteLogInfo("向服务端发起重连,m_strIp=" + m_strIp + ", m_nPort=" + to_string(m_nPort));
	connectSvr(m_strIp.c_str(), m_nPort);//虚函数
	if (isConnected()) {
		WriteLogInfo(getParentType() + "tcp连接成功 tcp connected");
		login();
	}
	else {
		WriteLogErr(getParentType() + "tcp连接失败 tcp connect failed");
	}
}
