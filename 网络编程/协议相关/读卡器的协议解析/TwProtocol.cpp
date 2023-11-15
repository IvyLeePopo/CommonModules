#include "TwProtocol.h"

#define TWPD_STARTFLAG  0xDF    //帧开始取值
#define TWPD_ENDFLAG    0xFD    //帧结束取值
#define TWPD_STARTFLAG_ESCAPE_FIRST 0xDE
#define TWPD_STARTFLAG_ESCAPE_SECOND 0x01
#define TWPD_ENDFLAG_ESCAPE_FIRST 0xFC
#define TWPD_ENDFLAG_ESCAPE_SECOND 0x01
#define TWPD_COM1 0x01
#define TWPD_COM2 0x02


typedef unsigned char		u8;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;



bool TW_Protocol::PackBytes(eSubCmdType_t subCmdType, const string & jsonData, std::vector<unsigned char>& escapePackage)
{
	std::vector<unsigned char> package;
	unsigned short len = static_cast<unsigned short>(jsonData.size());
	unsigned char chLen1 = 0xFF & (len >> 8);
	unsigned char chLen2 = 0xFF & len;
	package.push_back(chLen1);
	package.push_back(chLen2);

	package.push_back(eProtocolConst::PROTOCOL_NUMBER);
	package.push_back(eProtocolConst::PROTOCOL_NUMBER);
	package.push_back(eTopCmdType::TOP_CMD_TYPE);
	package.push_back(subCmdType);

	std::vector<unsigned char> data;
	std::copy(jsonData.begin(), jsonData.end(), std::back_inserter(data));
	std::copy(data.begin(), data.end(), std::back_inserter(package));

	unsigned short crc_code = crc16(package.begin(), package.end());
	unsigned char crc_code1 = 0xFF & (crc_code >> 8);
	unsigned char crc_code2 = 0xFF & crc_code;
	package.push_back(crc_code1);
	package.push_back(crc_code2);

	//组包完，转义，后面再发送出去
	EscapeComProtocol(package, escapePackage);

	return true;
}

bool TW_Protocol::UnPackBytes(const u8 * pcucSrcData, int nSrcLen, eSubCmdType_t correctSubCmd, string & jsonData)
{
	if (NULL == pcucSrcData || 0 == nSrcLen)
	{
		return false;
	}

	if (nSrcLen <= 10)
	{
		return false;
	}

	std::vector<unsigned char> src, dst;
	for (int index = 0; index < nSrcLen; index++)
	{
		src.push_back(pcucSrcData[index]);
	}

	//拿到数据先转义，再解包
	UnEscapeComProtocol(src, dst);

	//转义就检查这个包是否合理
	if(CheckPackage(dst))
		return false;

	//转义成功
	u8* pszDst = new u8[nDstLen];
	memset(pszDst, 0, nDstLen);
	for (int index = 0; index < nDstLen; index++)
	{
		pszDst[index] = dst.at(index + DATA_OFFSET);//只需要真正的数据，不要帧头、长度、编号、效验码、帧尾
	}

	jsonData = (char *)pszDst;

	return true;
}

bool TW_Protocol::EscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst)
{
	dst.clear();
	dst.push_back(eProtocolConst::PROTOCOL_HEADER);

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

	dst.push_back(eProtocolConst::PROTOCOL_END);

	return true;
}

bool TW_Protocol::UnEscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst)
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

unsigned short TW_Protocol::crc16(const std::vector<u8>::const_iterator & it_first, const std::vector<u8>::const_iterator & it_second)
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
		}
	}

	return (unsigned short)(CRC16Hi * 0x100 + CRC16Lo);
}


bool TW_Protocol::CheckPackage(const std::vector<u8>& data)
{
	size_t package_len = data.size();
	if (package_len < 10)//协议校验失败:长度不合法
	{
		return false;
	}

	if (data[0] != 0xDF || data[package_len - 1] != 0xFD) // 议校验失败:协议头尾错误
	{
		return false;
	}

	size_t data_len = MAKEWORDBIG(data[1], data[2]);
	if (data_len != (package_len - 10))//协议校验失败:数据域缺失
	{
		return false;
	}
	
	unsigned short calc_crc = crc16(data.begin() + 1, data.end() - 3);
	unsigned char crc_code1 = 0xFF & (calc_crc >> 8);
	unsigned char crc_code2 = 0xFF & calc_crc;

	std::vector<u8> tmp;
	tmp.insert(tmp.begin(), data.begin() + 1, data.end() - 2);

	if (!(data[package_len - 3] == crc_code1 && data[package_len - 2] == crc_code2))//协议校验失败:CRC错误
	{
		return false;
	}

	return true;
}
