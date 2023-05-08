#include "CommonPacketObj.h"

// 实例演示

//接收到数据：转译

vector<uint8> m_VerData;

// 接收数据处理（先转义，再校验）
void ReceivedMessage(QByteArray array)
{
	//解包：转译，校验
	m_VerData.insert(m_VerData.end(), array.begin(), array.end());
	vector<vector<uint8>> completePackets = CommonPacketObj::UnPackBytes(m_VerData);      //得到真实数据

	if (completePackets.size() < 1)
	{
		qDebug() << __LINE__ << __FUNCTION__ << "completePackets count < 1";
		return;
	}
}

// 发送数据处理（先校验，再转义）
void SendMessage(uint8 cmd1, uint8 cmd2, QByteArray bytes)
{
	vector<uint8> vec_data = CommonPacketObj::PackBytes(cmd1, cmd2, bytes.data(), bytes.length());
	QByteArray array(reinterpret_cast<const char*>(vec_data.data()), static_cast<int>(vec_data.size()));

	m_WebSocket->sendBinaryMessage(array);
}