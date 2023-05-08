#include "CommonPacketObj.h"

// ʵ����ʾ

//���յ����ݣ�ת��

vector<uint8> m_VerData;

// �������ݴ�����ת�壬��У�飩
void ReceivedMessage(QByteArray array)
{
	//�����ת�룬У��
	m_VerData.insert(m_VerData.end(), array.begin(), array.end());
	vector<vector<uint8>> completePackets = CommonPacketObj::UnPackBytes(m_VerData);      //�õ���ʵ����

	if (completePackets.size() < 1)
	{
		qDebug() << __LINE__ << __FUNCTION__ << "completePackets count < 1";
		return;
	}
}

// �������ݴ�����У�飬��ת�壩
void SendMessage(uint8 cmd1, uint8 cmd2, QByteArray bytes)
{
	vector<uint8> vec_data = CommonPacketObj::PackBytes(cmd1, cmd2, bytes.data(), bytes.length());
	QByteArray array(reinterpret_cast<const char*>(vec_data.data()), static_cast<int>(vec_data.size()));

	m_WebSocket->sendBinaryMessage(array);
}