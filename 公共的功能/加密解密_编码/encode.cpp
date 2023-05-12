#include"encode.h"

namespace base64
{
#define AVal(x) Base64ValTab[x]
#define BVal(x) Base64IdxTab[x]
	const BYTE Base64ValTab[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	const BYTE Base64IdxTab[128] =
	{
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255,
		255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
		255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255
	};

	int encodeBase64(unsigned char * pszInput, int nLength, char * pszOutput)
	{
		if (nullptr == pszInput || nullptr == pszOutput)
		{
			return 0;
		}

		int nIndex = 0;
		int nLoop = 0;
		int nRemain = 0;
		int nDstLen = 0;
		int nSrcLen = nLength;

		nLoop = nSrcLen / 3;
		nRemain = nSrcLen % 3;

		// also can encode native char one by one as decode method
		// but because all of char in native string is to be encoded so encode 3-chars one time is easier.

		for (nIndex = 0; nIndex < nLoop; nIndex++)
		{
			BYTE a1 = (pszInput[nIndex * 3] >> 2);
			BYTE a2 = (((pszInput[nIndex * 3] & 0x03) << 4) | (pszInput[nIndex * 3 + 1] >> 4));
			BYTE a3 = (((pszInput[nIndex * 3 + 1] & 0x0F) << 2) | ((pszInput[nIndex * 3 + 2] & 0xC0) >> 6));
			BYTE a4 = (pszInput[nIndex * 3 + 2] & 0x3F);

			pszOutput[nIndex * 4] = AVal(a1);
			pszOutput[nIndex * 4 + 1] = AVal(a2);
			pszOutput[nIndex * 4 + 2] = AVal(a3);
			pszOutput[nIndex * 4 + 3] = AVal(a4);
		}

		nDstLen = nIndex * 4;
		if (nRemain == 1)
		{
			// should pad two equal sign
			nIndex = nSrcLen - 1;
			BYTE a1 = (pszInput[nIndex] >> 2);
			BYTE a2 = ((pszInput[nIndex] & 0x03) << 4);

			pszOutput[nDstLen++] = AVal(a1);
			pszOutput[nDstLen++] = AVal(a2);
			pszOutput[nDstLen++] = '=';
			pszOutput[nDstLen++] = '=';
			pszOutput[nDstLen] = 0x00;
		}
		else if (nRemain == 2)
		{
			// should pad one equal sign
			nIndex = nSrcLen - 2;
			BYTE a1 = (pszInput[nIndex] >> 2);
			BYTE a2 = (((pszInput[nIndex] & 0x03) << 4) | (pszInput[nIndex + 1] >> 4));
			BYTE a3 = ((pszInput[nIndex + 1] & 0x0F) << 2);

			pszOutput[nDstLen++] = AVal(a1);
			pszOutput[nDstLen++] = AVal(a2);
			pszOutput[nDstLen++] = AVal(a3);
			pszOutput[nDstLen++] = '=';
			pszOutput[nDstLen] = 0x00;
		}
		else
		{
			// just division by 3
			pszOutput[nDstLen] = 0x00;
		}

		return nDstLen;
	}

	int decodeBase64(unsigned char * pszInput, int nLength, char * pszOutput)
	{
		if (nullptr == pszInput || nullptr == pszOutput)
		{
			return 0;
		}

		int nCnt = 0;
		int nSrcLen = nLength;

		char * pChar = pszOutput;
		for (int nIndex = 0; nIndex < nSrcLen; nIndex++)
		{
			if (pszInput[nIndex] > 127)
				continue;

			if (pszInput[nIndex] == '=')
				return (int)(pChar - pszOutput + 1);

			BYTE byteValue = BVal(pszInput[nIndex]);
			if (byteValue == 255) continue;

			switch (nCnt)
			{
			case 0:
			{
				*pChar = byteValue << 2;
				nCnt++;
			}
			break;
			case 1:
			{
				*pChar++ |= byteValue >> 4;
				*pChar = byteValue << 4;
				nCnt++;
			}
			break;
			case 2:
			{
				*pChar++ |= byteValue >> 2;
				*pChar = byteValue << 6;
				nCnt++;
			}
			break;
			case 3:
			{
				*pChar++ |= byteValue;
				nCnt = 0;
			}
			break;
			default:
				break;
			}
		}

		*pChar = 0x00;
		return (int)(pChar - pszOutput);
	}

	string string2Base64_qt(const char * pszData, unsigned int nLen)
	{
		if (nullptr == pszData)
			return "";

		QByteArray qByteArray = pszData;
		qByteArray = qByteArray.toBase64();
		QString qStr = qByteArray.data();
		return qStr.toStdString();
	}

	string base64ToString_qt(string strData)
	{
		QByteArray qByteArray = QByteArray(strData.c_str());
		QString strResult = QString::fromUtf8(qByteArray.fromBase64(qByteArray));
		return strResult.toStdString();
	}
}


namespace sha_md5_crc16
{
	string encryptSha256(const char * pszData, int nSize)
	{
		if (NULL == pszData)
			return "";

		//uint8_t szResult[64] = {0};
		//sha256(pszData, nSize, szResult);

		string strResult;// = format("") szResult;
		return strResult;
	}


	unsigned short crc16(const char* pszData, int nSize)
	{
		if (nullptr == pszData || 0 == nSize)
			return 0;

		BYTE crc16Hi = 0xFF;
		BYTE crc16Lo = 0xFF;
		BYTE cH = 0xA0;
		BYTE cL = 0x01;
		BYTE saveHi = 0;
		BYTE saveLo = 0;

		for (int nIndex = 0; nIndex < nSize; nIndex++)
		{
			crc16Lo = crc16Lo ^ pszData[nIndex];
			for (int nFlag = 0; nFlag < 8; nFlag++)
			{
				saveHi = crc16Hi;
				saveLo = crc16Lo;
				crc16Hi = crc16Hi / 2; //高位右移一位
				crc16Lo = crc16Lo / 2; //低位右移一位
				if ((saveHi & 1) == 1)
					crc16Lo |= 0x80;

				if ((saveLo & 1) == 1)
				{
					crc16Hi = crc16Hi ^ cH;
					crc16Lo = crc16Lo ^ cL;
				}
			}
		}

		return (WORD)(crc16Hi * 0x100 + crc16Lo);
	}

	unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first,
		const std::vector<unsigned char>::const_iterator& it_second)
	{
		BYTE crc16Hi = 0xFF;
		BYTE crc16Lo = 0xFF;
		BYTE cH = 0xA0;
		BYTE cL = 0x01;
		BYTE saveHi = 0;
		BYTE saveLo = 0;

		for (std::vector<unsigned char>::const_iterator it = it_first; it != it_second; ++it)
		{
			crc16Lo = crc16Lo ^ (*it);
			for (int nFlag = 0; nFlag < 8; nFlag++)
			{
				saveHi = crc16Hi;
				saveLo = crc16Lo;
				crc16Hi = crc16Hi / 2; //高位右移一位
				crc16Lo = crc16Lo / 2; //低位右移一位
				if ((saveHi & 1) == 1)
					crc16Lo |= 0x80;

				if ((saveLo & 1) == 1)
				{
					crc16Hi = crc16Hi ^ cH;
					crc16Lo = crc16Lo ^ cL;
				}
			}
		}

		return (WORD)(crc16Hi * 0x100 + crc16Lo);
	}

	uint16_t crc16_2(uint8_t * puchMsg, uint32_t usDataLen)
	{
		short crctab16[] =
		{
			0, 4489, 8978, 12955, 17956, 22445, 25910, 29887,
			-29624, -25151, -20646, -16685, -13716, -9243, -5762, -1801, 4225,
			 264, 13203, 8730, 22181, 18220, 30135, 25662, -25399, -29376,
			 -16421, -20910, -9491, -13468, -1537, -6026, 8450, 12427,
			 528, 5017, 26406, 30383, 17460, 21949, -21174, -17213, -29096,
			 -24623, -5266, -1305, -14212, -9739, 12675, 8202, 4753, 792,
			 30631, 26158, 21685, 17724, -16949, -21438, -24871, -28848,
			 -1041, -5530, -9987, -13964, 16900, 21389, 24854, 28831, 1056,
			 5545, 10034, 14011, -12724, -8251, -4770, -809, -30616, -26143,
			 -21638, -17677, 21125, 17164, 29079, 24606, 5281, 1320, 14259,
			 9786, -8499, -12476, -545, -5034, -26391, -30368, -17413,
			 -21902, 25350, 29327, 16404, 20893, 9506, 13483, 1584, 6073,
			 -4274, -313, -13220, -8747, -22166, -18205, -30088, -25615, 29575,
			 25102, 20629, 16668, 13731, 9258, 5809, 1848, -49, -4538, -8995,
			 -12972, -17941, -22430, -25863, -29840, -31736, -27263, -22758,
			 -18797, -15828, -11355, -7874, -3913, 2112, 6601, 11090, 15067,
			 20068, 24557, 28022, 31999, -27511, -31488, -18533, -23022, -11603,
			 -15580, -3649, -8138, 6337, 2376, 15315, 10842, 24293, 20332, 32247,
			 27774, -23286, -19325, -31208, -26735, -7378, -3417, -16324, -11851,
			 10562, 14539, 2640, 7129, 28518, 32495, 19572, 24061, -19061, -23550,
			 -26983, -30960, -3153, -7642, -12099, -16076, 14787, 10314, 6865, 2904,
			 32743, 28270, 23797, 19836, -14836, -10363, -6882, -2921, -32728, -28255,
			 -23750, -19789, 19012, 23501, 26966, 30943, 3168, 7657, 12146, 16123,
			 -10611, -14588, -2657, -7146, -28503, -32480, -19525, -24014, 23237,
			 19276, 31191, 26718, 7393, 3432, 16371, 11898, -6386, -2425, -15332,
			 -10859, -24278, -20317, -32200, -27727, 27462, 31439, 18516, 23005,
			 11618, 15595, 3696, 8185, -2161, -6650, -11107, -15084, -20053, -24542,
			 -27975, -31952, 31687, 27214, 22741, 18780, 15843, 11370, 7921, 3960
		};

		int16_t fcs = -1;
		int i;
		uint16_t res;

		for (i = 0; i < usDataLen; i++)
		{
			fcs = (short)(((fcs & 0xFFFF) >> 8) ^ crctab16[(fcs ^ puchMsg[i]) & 0xFF]);
		}

		fcs = (int16_t)(~fcs);

		res = fcs;
		return res;
	}

#include <openssl/md5.h>
	string md5_OpenSSL(const string & input)
	{
		unsigned char digest[MD5_DIGEST_LENGTH];
		MD5((unsigned char*)input.c_str(), input.length(), digest);
		stringstream ss;
		for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
			ss << hex << setw(2) << setfill('0') << (int)digest[i];
		}
		return ss.str();
	}
}


namespace codeTrans
{
#include <windows.h>

	string utf8ToGbk(const char *pszSrc)
	{
		if (nullptr == pszSrc)
			return "";

		//Windows API 函数，用于将多字节字符集（如 ASCII）转换为宽字符集（如 Unicode）
		int nLen = MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, NULL, 0);
		wchar_t* pwszGBK = new wchar_t[nLen + 1];
		memset(pwszGBK, 0, nLen * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, pwszGBK, nLen);
		nLen = WideCharToMultiByte(CP_ACP, 0, pwszGBK, -1, NULL, 0, NULL, NULL);

		char* pszGBK = new char[nLen + 1];
		memset(pszGBK, 0, nLen + 1);
		WideCharToMultiByte(CP_ACP, 0, pwszGBK, -1, pszGBK, nLen, NULL, NULL);
		string strTemp(pszGBK);

		delete[] pwszGBK;
		pwszGBK = nullptr;

		delete[] pszGBK;
		pszGBK = nullptr;
		return strTemp;
	}

	std::string gbk2Utf8(string& strData)
	{
		int nLen = MultiByteToWideChar(CP_ACP, 0, strData.c_str(), -1, NULL, 0);

		WCHAR *pWStr1 = new WCHAR[nLen];
		MultiByteToWideChar(CP_ACP, 0, strData.c_str(), -1, pWStr1, nLen);
		nLen = WideCharToMultiByte(CP_UTF8, 0, pWStr1, -1, NULL, 0, NULL, NULL);


		char *pStr2 = new char[nLen];
		WideCharToMultiByte(CP_UTF8, 0, pWStr1, -1, pStr2, nLen, NULL, NULL);

		string strOutUtf8 = pStr2;

		delete[] pWStr1;
		pWStr1 = NULL;

		delete[] pStr2;
		pStr2 = NULL;

		return strOutUtf8;
	}

	string G2U_qt(string strGBK)
	{
		QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
		QTextCodec *gbk = QTextCodec::codecForName("GBK");
		//QTextCodec::setCodecForLocale(utf8);

		QByteArray szGBK = QByteArray::fromStdString(strGBK);
		QString strUnicode = gbk->toUnicode(szGBK.data());
		QByteArray utf8data = utf8->fromUnicode(strUnicode);
		return utf8data.toStdString();
	}

	QString GBK_2_UTF8_qt(const char * p)
	{
		QTextCodec *gbk = QTextCodec::codecForName("GBK");
		QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

		QString strUnicode = gbk->toUnicode(p);
		QByteArray utf8_bytes = utf8->fromUnicode(strUnicode);

		QString result = QString::fromLocal8Bit(utf8_bytes);

		return result;
	}

	QString UTF8_2_GBK_qt(const char * p)
	{
		QTextCodec *gbk = QTextCodec::codecForName("GBK");
		QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");

		QString strUnicode = utf8->toUnicode(p);
		QByteArray gbk_bytes = gbk->fromUnicode(strUnicode);

		QString result = QString::fromLocal8Bit(gbk_bytes);

		return result;
	}
}