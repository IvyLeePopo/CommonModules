#include "protocol_parse.h"
#include <QByteArray>
#include <QTextCodec>
#include <QDebug>

#include "protocol_cmd.h"

void ProtocolParse::ParseProtocolData(std::vector<std::string>& c0_queue, std::vector<unsigned char>& real_data)
{
	std::list<unsigned char> ref_datas;//收到一条数据
	while (!ref_datas.empty())
	{
		std::list<unsigned char>::iterator beg_it = std::find(ref_datas.begin(), ref_datas.end(), TWPD_STARTFLAG);
		// 找帧头
		if (beg_it != ref_datas.begin())
		{
			std::vector<unsigned char> tmp;
			tmp.insert(tmp.end(), ref_datas.begin(), beg_it);
			ref_datas.erase(ref_datas.begin(), beg_it);
			continue;
		}

		// 找帧尾
		std::list<unsigned char>::iterator end_it = std::find(beg_it, ref_datas.end(), TWPD_ENDFLAG);
		if (end_it == ref_datas.end())
		{
			break;
		}

		std::vector<unsigned char> one_data;
		std::advance(end_it, 1);
		// 找到一条完整的数据
		one_data.insert(one_data.end(), ref_datas.begin(), end_it);
		ref_datas.erase(ref_datas.begin(), end_it);

		// 进行转义，得到真实的数据real_data
		UnEscapeComProtocol(one_data, real_data);

		// 可以拿着真实的数据real_data，进行操作
	}
}

/* 计算数据长度的几种方法
1、	uint8 objLen1 = uint8(size() / 256);
	uint8 objLen2 = uint8(size() % 256);

2、	uint8 objLen1 = (objectData.size() >> 8) & 0xFF;
	uint8 objLen2 = objectData.size() & 0xFF;

3、	package.push_back(HIBYTE(len));
	package.push_back(LOBYTE(len));
*/

// 具体要看数据定义，只是将中件的数据组装并转义
bool ProtocolParse::packBytes(eSubCmdType_t subCmdType, const string& jsonData, u8* pcucDstData, u32& nDstLen)
{
	std::vector<unsigned char> package;
	unsigned short len = static_cast<unsigned short>(jsonData.size());
	package.push_back(HIBYTE(len));
	package.push_back(LOBYTE(len));
	package.push_back(TWPD_NUMBER);
	package.push_back(TWPD_NUMBER);
	package.push_back(0x58);
	package.push_back(subCmdType);

	std::vector<unsigned char> data;
	std::copy(jsonData.begin(), jsonData.end(), std::back_inserter(data));
	std::copy(data.begin(), data.end(), std::back_inserter(package));

	unsigned short crc_code = crc16(package.begin(), package.end());
	package.push_back(HIBYTE(crc_code));
	package.push_back(LOBYTE(crc_code));

	std::vector<unsigned char> escapePackage;
	EscapeComProtocol(package, escapePackage);

	for (int index = 0; index < nDstLen; index++)
	{
		pcucDstData[index] = escapePackage[index];
	}

	return true;
}


// CRC校验
bool ProtocolParse::crc16(const u8* byte, int len, u8* cHi, u8* cLo)
{
	u8	CRC16Hi = 0xFF;
	u8	CRC16Lo = 0xFF;
	u8	CH = 0xA0;
	u8	CL = 0x01;
	u8	SaveHi = 0;
	u8	SaveLo = 0;
	int     i;
	u8   Flag;

	for (i = 1; i < len; i++)
	{
		CRC16Lo = CRC16Lo ^ byte[i];
		for (Flag = 0; Flag < 8; Flag++)
		{
			SaveHi = CRC16Hi;
			SaveLo = CRC16Lo;
			CRC16Hi = CRC16Hi >> 1;
			CRC16Lo = CRC16Lo >> 1;
			if ((SaveHi & 1) == 1)
				CRC16Lo |= 0x80;
			if ((SaveLo & 1) == 1)
			{
				CRC16Hi = CRC16Hi ^ CH;
				CRC16Lo = CRC16Lo ^ CL;
			}
		}
	}
	*cHi = CRC16Hi;
	*cLo = CRC16Lo;

	return true;
}

unsigned short ProtocolParse::crc16(const std::vector<u8>::const_iterator & it_first, const std::vector<u8>::const_iterator & it_second)
{
	if (it_first == it_second)
	{
		return 0;
	}
	u8    CRC16Hi = 0xFF;
	u8    CRC16Lo = 0xFF;
	u8    CH = 0xA0;
	u8    CL = 0x01;
	u8    SaveHi = 0;
	u8    SaveLo = 0;

	for (std::vector<u8>::const_iterator it = it_first; it != it_second; ++it)
	{
		CRC16Lo = CRC16Lo ^ (*it);

		for (int Flag = 0; Flag < 8; Flag++)
		{
			SaveHi = CRC16Hi;
			SaveLo = CRC16Lo;
			CRC16Hi = CRC16Hi / 2; //高位右移一位
			CRC16Lo = CRC16Lo / 2; //低位右移一位
			if ((SaveHi & 1) == 1)
				CRC16Lo |= 0x80;
			if ((SaveLo & 1) == 1)
			{
				CRC16Hi = CRC16Hi ^ CH;
				CRC16Lo = CRC16Lo ^ CL;
			}
		}//for(int Flag = 0;Flag < 8;Flag++)
	}

	return (unsigned short)(CRC16Hi * 0x100 + CRC16Lo);
}

// 转义
bool ProtocolParse::EscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst)
{
	dst.clear();
	dst.push_back(TWPD_STARTFLAG);

	for (size_t i = 0; i < src.size(); ++i)
	{
		if (src[i] == TWPD_STARTFLAG)
		{
			dst.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
			dst.push_back(TWPD_STARTFLAG_ESCAPE_SECOND);
		}
		else if (src[i] == TWPD_STARTFLAG_ESCAPE_FIRST)
		{
			dst.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
			dst.push_back(0x00);
		}
		else if (src[i] == TWPD_ENDFLAG)
		{
			dst.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
			dst.push_back(TWPD_ENDFLAG_ESCAPE_SECOND);
		}
		else if (src[i] == TWPD_ENDFLAG_ESCAPE_FIRST)
		{

			dst.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
			dst.push_back(0x00);
		}
		else
		{
			dst.push_back(src[i]);
		}
	}

	dst.push_back(TWPD_ENDFLAG);

	return true;
}

bool ProtocolParse::UnEscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst)
{
	dst.clear();

	if (src.empty())
	{
		return true;
	}
	size_t i = 0;
	while (i < (src.size() - 1)) /**/
	{
		if (src[i] == TWPD_STARTFLAG_ESCAPE_FIRST)
		{
			if (src[i + 1] == TWPD_STARTFLAG_ESCAPE_SECOND)
			{
				dst.push_back(TWPD_STARTFLAG);
			}
			else if (src[i + 1] == 0x00)
			{
				dst.push_back(TWPD_STARTFLAG_ESCAPE_FIRST);
			}
			else
			{
				return false;
			}
			i += 2;
		}
		else if (src[i] == TWPD_ENDFLAG_ESCAPE_FIRST)
		{
			if (src[i + 1] == TWPD_ENDFLAG_ESCAPE_SECOND)
			{
				dst.push_back(TWPD_ENDFLAG);
			}
			else if (src[i + 1] == 0x00)
			{
				dst.push_back(TWPD_ENDFLAG_ESCAPE_FIRST);
			}
			else
			{
				return false;
			}
			i += 2;
		}
		else
		{
			dst.push_back(src[i]);
			i++;
		}
	}
	/*last char unescape*/
	if (src.back() == TWPD_STARTFLAG_ESCAPE_FIRST
		|| src.back() == TWPD_ENDFLAG_ESCAPE_FIRST)
	{
		return false;
	}
	else
	{
		dst.push_back(src.back());
	}


	return true;
}



ProtocolParse::ProtocolParse()
{
}

//UTF8转GBK(打包用)
vector<uint8> ProtocolParse::vectorUTF8ToGBK(char *msg, int len)
{
	vector<uint8> verData;
	QByteArray inStr(msg, len);

	QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
	QTextCodec *gbk = QTextCodec::codecForName("gbk");

	QString str = utf8->toUnicode(inStr.data());
	QByteArray gbk_bytes = gbk->fromUnicode(str);

	verData.insert(verData.end(), gbk_bytes.begin(), gbk_bytes.end());
	return verData;
}

//UTF8转GBK(打包用)
QByteArray ProtocolParse::bytesUTF8ToGBK(char *msg, int len)
{
	vector<uint8> verData;
	QByteArray inStr(msg, len);

	QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
	QTextCodec *gbk = QTextCodec::codecForName("gbk");

	QString str = utf8->toUnicode(inStr.data());
	QByteArray gbk_bytes = gbk->fromUnicode(str);

	return gbk_bytes;
}

//GBK转UTF8(解包用)
vector<uint8> ProtocolParse::vectorGBKToUTF8(char *msg, int len)
{
	vector<uint8> verData;
	QByteArray inStr(msg, len);

	QTextCodec *gbk = QTextCodec::codecForName("gbk");
	QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

	QString str = gbk->toUnicode(inStr.data());
	QByteArray utf8_bytes = utf8->fromUnicode(str);

	verData.insert(verData.end(), utf8_bytes.begin(), utf8_bytes.end());
	return verData;
}

//GBK转UTF8(解包用)
QByteArray ProtocolParse::bytesGBKToUTF8(char *msg, int len)
{
	vector<uint8> verData;
	QByteArray inStr(msg, len);

	QTextCodec *gbk = QTextCodec::codecForName("gbk");
	QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

	QString str = gbk->toUnicode(inStr.data());
	QByteArray utf8_bytes = utf8->fromUnicode(str);

	return utf8_bytes;
}

//CRC生成
vector<uint8> ProtocolParse::crc16(vector<uint8> &byte, uint32 len)
{
	uint32 i;
	uint8 Flag;
	vector<uint8> array;
	uint8 CRC16Hi = 0xFF;
	uint8 CRC16Lo = 0xFF;
	uint8 CH = 0xA0;
	uint8 CL = 0x01;
	uint8 SaveHi = 0;
	uint8 SaveLo = 0;

	for (i = 1; i < len; i++)
	{
		CRC16Lo = CRC16Lo ^ byte[i];
		for (Flag = 0; Flag < 8; Flag++)
		{
			SaveHi = CRC16Hi;
			SaveLo = CRC16Lo;
			CRC16Hi = CRC16Hi >> 1;
			CRC16Lo = CRC16Lo >> 1;
			if ((SaveHi & 1) == 1)
				CRC16Lo |= 0x80;
			if ((SaveLo & 1) == 1)
			{
				CRC16Hi = CRC16Hi ^ CH;
				CRC16Lo = CRC16Lo ^ CL;
			}
		}
	}
	array.push_back(CRC16Hi);
	array.push_back(CRC16Lo);
	return array;
}

//CRC校验判断
bool ProtocolParse::checkPackage(const vector<uint8>& vecData)
{
	uint64 package_len = vecData.size();
	vector<uint8> vecData_t(vecData);

	if (package_len < MSG_MIN_LEN)
	{
		qDebug() << __LINE__ << __FUNCTION__ << "协议校验失败:长度不合法";
		return false;
	}

	if (vecData[0] != PROT_HEADER || vecData[package_len - 1] != PROT_END)
	{
		qDebug() << __LINE__ << __FUNCTION__ << "协议校验失败:协议头尾错误";
		return false;
	}

	int len_t = (vecData[1] << 8) + vecData[2];
	uint64 package_len_t = package_len - MSG_MIN_LEN;
	if (static_cast<uint64>(len_t) != package_len_t)
	{
		qDebug() << __LINE__ << __FUNCTION__ << "协议校验失败:数据域缺失" << len_t << package_len_t;
		return false;
	}

	vecData_t.erase(vecData_t.end() - 3, vecData_t.end());
	vector<uint8> package_crc = crc16(vecData_t, static_cast<uint32>(vecData_t.size()));
	vector<uint8> calc_crc;
	calc_crc.push_back(vecData.at(vecData.size() - 3));
	calc_crc.push_back(vecData.at(vecData.size() - 2));

	if (package_crc != calc_crc)
	{
		qDebug() << __LINE__ << __FUNCTION__ << "协议校验失败:CRC错误";
		return false;
	}

	return true;
}

/************************************************************************
* 函数功能: 转译数据
* lpData: 要转译的数据
* len: 数据长度
* bSendState: 1发送，0接收
*************************************************************************/
vector<uint8> ProtocolParse::escapeComProtocol(vector<uint8> &lpData, uint32 len, uint32 bSendState)
{
	QByteArray t_lpData;
	vector<uint8> verData;
	lpData.resize(len + 1);

	t_lpData[0] = PROT_HEADER; //设置帧头
	uint32 iIndex = 1;  //处理数据帧其它区域

	for (quint32 i = 1; i < (len - 1); i++)
	{
		if (bSendState == 0)//接收数据
		{

			if (PROT_HEAND_CONVERT_CHAR1 == lpData[i] && PROT_HEAND_CONVERT_CHAR2 == lpData[i + 1])
			{
				t_lpData[iIndex++] = PROT_HEADER;
				i++;
			}
			else if (PROT_HEAND_CONVERT_CHAR1 == lpData[i] && PROT_CONVERT_CHAR == lpData[i + 1])
			{
				t_lpData[iIndex++] = PROT_HEAND_CONVERT_CHAR1;
				i++;
			}
			else if (PROT_END_CONVERT_CHAR1 == lpData[i] && PROT_END_CONVERT_CHAR2 == lpData[i + 1])
			{
				t_lpData[iIndex++] = PROT_END;
				i++;
			}
			else if (PROT_END_CONVERT_CHAR1 == lpData[i] && PROT_CONVERT_CHAR == lpData[i + 1])
			{
				t_lpData[iIndex++] = PROT_END_CONVERT_CHAR1;
				i++;
			}
			else
			{
				t_lpData[iIndex++] = lpData[i];
			}
		}
		else		//发送数据
		{
			if (PROT_HEADER == lpData[i])
			{
				t_lpData[iIndex++] = PROT_HEAND_CONVERT_CHAR1;
				t_lpData[iIndex++] = PROT_HEAND_CONVERT_CHAR2;
			}
			else if (PROT_HEAND_CONVERT_CHAR1 == lpData[i])
			{
				t_lpData[iIndex++] = PROT_HEAND_CONVERT_CHAR1;
				t_lpData[iIndex++] = PROT_CONVERT_CHAR;
			}
			else if (PROT_END == lpData[i])
			{
				t_lpData[iIndex++] = PROT_END_CONVERT_CHAR1;
				t_lpData[iIndex++] = PROT_END_CONVERT_CHAR2;
			}
			else if (PROT_END_CONVERT_CHAR1 == lpData[i])
			{
				t_lpData[iIndex++] = PROT_END_CONVERT_CHAR1;
				t_lpData[iIndex++] = PROT_CONVERT_CHAR;
			}
			else
			{
				t_lpData[iIndex++] = lpData[i];
			}
		}
	}
	//设置帧尾
	t_lpData[iIndex++] = PROT_END;

	verData.insert(verData.end(), t_lpData.begin(), t_lpData.end());
	return verData;
}

/************************************************************************
* 函数功能: 打包数据
* actionType: 一级指令
* objectData: 要打包的数据
*************************************************************************/
vector<uint8> ProtocolParse::packBytes(DevFunctionType1 actionType, uint8 objectType, vector<uint8>& objectData)
{
	vector<uint8> object;
	//Data[0]:包头
	object.push_back(PROT_HEADER);
	//Data[1～2]:数据长度
	uint8 objLen1 = (objectData.size() >> 8) & 0xFF;
	uint8 objLen2 = objectData.size() & 0xFF;
	object.push_back(objLen1);
	object.push_back(objLen2);
	//Data[5～6]:本帧编号
	object.push_back(0x00);
	object.push_back(0x00);
	//Data[5～6]:一级&二级指令
	object.push_back(static_cast<uint8>(actionType));
	object.push_back(static_cast<uint8>(objectType));
	//Data[7～len]:数据
	if (objectData.size() != 0)
	{
		object.insert(object.end(), objectData.begin(), objectData.end());
	}
	//校验码
	vector<uint8> crc = crc16(object, static_cast<uint32>(object.size()));
	object.push_back(crc[0]);
	object.push_back(crc[1]);
	//包尾
	object.push_back(PROT_END);

	vector<uint8> ObjectBytes = escapeComProtocol(object, static_cast<uint32>(object.size()), 1);
	return ObjectBytes;
}

/************************************************************************
* 函数功能: 打包数据
* actionType: 一级指令
* msg: 要打包的数据
* len: 数据长度
*************************************************************************/
vector<uint8> ProtocolParse::packBytes(uint8 actionType, uint8 objectType, char *msg, int len)
{
	vector<uint8> bytes;
	bytes.insert(bytes.end(), msg, msg + len);

	vector<uint8> object;
	//Data[0]:包头
	object.push_back(PROT_HEADER);
	//Data[1～2]:数据长度
	uint8 objLen1 = uint8(bytes.size() / 256);
	uint8 objLen2 = uint8(bytes.size() % 256);
	object.push_back(objLen1);
	object.push_back(objLen2);
	//Data[5～6]:本帧编号
	object.push_back(0x00);
	object.push_back(0x00);
	//Data[5～6]:一级&二级指令
	object.push_back(actionType);
	object.push_back(objectType);
	//Data[7～len]:数据
	if (bytes.size() != 0)
	{
		object.insert(object.end(), bytes.begin(), bytes.end());
	}

	//校验码
	vector<uint8> crc = crc16(object, static_cast<uint32>(object.size()));
	object.push_back(crc[0]);
	object.push_back(crc[1]);

	//包尾
	object.push_back(PROT_END);
	vector<uint8> ObjectBytes = escapeComProtocol(object, static_cast<uint32>(object.size()), 1);
	return ObjectBytes;
}

/*****************************************************************
 函数名称：unPackBytes
 函数描述：解包
 输入参数：vecSource 接收到的包数据
 输出参数：vector<vector<uint8>>
 返回说明：返回完整且正确的数据
 其它说明：可能会得到多条数据，都存放在这个容器里，一条一条进行解析
 *****************************************************************/
vector<vector<uint8>> ProtocolParse::unPackBytes(vector<uint8>& vecSource)
{
	vector<vector<uint8>> completePackets;

	while (!vecSource.empty())
	{
		// 找到帧头
		vector<uint8>::iterator beg_it = std::find(vecSource.begin(), vecSource.end(), PROT_HEADER);

		// 丢弃不完整的数据
		if (beg_it != vecSource.begin())
		{
			vector<uint8> tmp;
			tmp.insert(tmp.end(), vecSource.begin(), beg_it);
			vecSource.erase(vecSource.begin(), beg_it);
			continue;
		}

		// 找到帧尾
		vector<uint8>::iterator end_it = std::find(beg_it, vecSource.end(), PROT_END);
		if (end_it == vecSource.end())
			break;

		//2.跳过不完整的数据（4字节为包头+指令+检验码+包尾）
		if (vecSource.size() < MSG_MIN_LEN)
			break;

		vector<uint8> one_data;
		std::advance(end_it, 1);//迭代器向前或向后移动指定数量的元素
		one_data.insert(one_data.end(), vecSource.begin(), end_it);
		vecSource.erase(vecSource.begin(), end_it);

		//转译
		vector<uint8> vecRealData = escapeComProtocol(one_data, static_cast<uint32>(one_data.size()), 0);

		//校验正确性
		bool flag = checkPackage(vecRealData);
		if (flag)
			completePackets.push_back(vecRealData);
		else
			qDebug() << __LINE__ << __FUNCTION__ << "CRC校验错误";

		break;
	}

	return completePackets;
}
