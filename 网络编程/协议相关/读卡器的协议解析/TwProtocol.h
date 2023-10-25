using namespace std;


//数据帧定义解析
typedef enum eProtocolConst
{
	PROTOCOL_HEADER = 0xDF,					//协议起始符
	PROTOCOL_END = 0xFD,					//协议休止符

	PROTOCOL_NUMBER = 0x00,			//协议帧编号

	PROTOCOL_CONVERT_CHAR = 0x00,			//协议转译公共字符

	PROTOCOL_HEAND_CONVERT_CHAR1 = 0xDE,	//协议起始符转译字符1
	PROTOCOL_HEAND_CONVERT_CHAR2 = 0x01,	//协议起始符转译字符2

	PROTOCOL_END_CONVERT_CHAR1 = 0xFC,		//协议休止符转译字符1
	PROTOCOL_END_CONVERT_CHAR2 = 0x01		//协议休止符转译字符2
}eProtocolType_t;

//主命令类型
typedef enum eTopCmdType
{
	TOP_CMD_TYPE = 0x58       //主命令类型，帧标记，其值:58(H)
}eTopCmdType_t;

//子命令类型
typedef enum eSubCmdType
{
	//TOPCMD:58(H)
	SUB_CMD_TYPE_OpenReader = 0x01,			//打开 IC 卡读写器
	SUB_CMD_TYPE_CloseReader = 0x02,		//关闭 IC 卡读写器

}eSubCmdType_t;


namespace TW_Protocol
{
	//组包、解包
	bool PackBytes(eSubCmdType_t subCmdType, const string& jsonData, std::vector<unsigned char>& escapePackage);
	bool UnPackBytes(const u8 * pcucSrcData, int nSrcLen, eSubCmdType_t correctSubCmd, string& jsonData, int& index);

	// 转义
	bool EscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst);
	bool UnEscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst);

	// CRC校验
	unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first, const std::vector<unsigned char>::const_iterator& it_second);

	// 检查接收的包
	bool CheckPackage(const std::vector<unsigned char>& data);
};