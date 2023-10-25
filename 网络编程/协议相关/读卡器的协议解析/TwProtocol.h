using namespace std;


//����֡�������
typedef enum eProtocolConst
{
	PROTOCOL_HEADER = 0xDF,					//Э����ʼ��
	PROTOCOL_END = 0xFD,					//Э����ֹ��

	PROTOCOL_NUMBER = 0x00,			//Э��֡���

	PROTOCOL_CONVERT_CHAR = 0x00,			//Э��ת�빫���ַ�

	PROTOCOL_HEAND_CONVERT_CHAR1 = 0xDE,	//Э����ʼ��ת���ַ�1
	PROTOCOL_HEAND_CONVERT_CHAR2 = 0x01,	//Э����ʼ��ת���ַ�2

	PROTOCOL_END_CONVERT_CHAR1 = 0xFC,		//Э����ֹ��ת���ַ�1
	PROTOCOL_END_CONVERT_CHAR2 = 0x01		//Э����ֹ��ת���ַ�2
}eProtocolType_t;

//����������
typedef enum eTopCmdType
{
	TOP_CMD_TYPE = 0x58       //���������ͣ�֡��ǣ���ֵ:58(H)
}eTopCmdType_t;

//����������
typedef enum eSubCmdType
{
	//TOPCMD:58(H)
	SUB_CMD_TYPE_OpenReader = 0x01,			//�� IC ����д��
	SUB_CMD_TYPE_CloseReader = 0x02,		//�ر� IC ����д��

}eSubCmdType_t;


namespace TW_Protocol
{
	//��������
	bool PackBytes(eSubCmdType_t subCmdType, const string& jsonData, std::vector<unsigned char>& escapePackage);
	bool UnPackBytes(const u8 * pcucSrcData, int nSrcLen, eSubCmdType_t correctSubCmd, string& jsonData, int& index);

	// ת��
	bool EscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst);
	bool UnEscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst);

	// CRCУ��
	unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first, const std::vector<unsigned char>::const_iterator& it_second);

	// �����յİ�
	bool CheckPackage(const std::vector<unsigned char>& data);
};