#pragma once

#include "CommandDef.h"
#include <string>
#include <vector>

typedef unsigned char u8;
typedef   signed char s8;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned int u32; //note: KEIL C51: sizeof(int) = 2
typedef   signed int s32;
typedef unsigned long long u64;
typedef   signed long long s64;

using namespace std;

namespace TW_Protocol
{
	// ��֡��--->�����м��--->������
	bool ConstructFrame(eSubCmdType_t subCmdType, const string& jsonData, u8* pcucDstData, u32& nDstLen);
	bool ConstructFrame_Websocket(eSubCmdType_t subCmdType, const string& jsonData, u8* pcucDstData, u32& nDstLen);

	// �������յ�������
	//bool ParseProtocolData(const u8* pcucDstData, int nDstLen, string& jsonData);
	bool ParseProtocolData(const u8 * pcucSrcData, int nSrcLen, eSubCmdType_t correctSubCmd, string& jsonData);

	// CRCУ��
	bool CRC16(const u8* byte, int len, u8* cHi, u8* cLo);
	unsigned short crc16(const std::vector<unsigned char>::const_iterator& it_first, const std::vector<unsigned char>::const_iterator& it_second);
	unsigned short crc16_Websocket(const u8* byte, int len);

	// �����յİ�
	bool CheckPackage(const std::vector<unsigned char>& data);

	// ת��
	bool EscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst);
	bool UnEscapeComProtocol(const std::vector<unsigned char>& src, std::vector<unsigned char>&dst);

	// �����������(�ʺϴ󲿷���ͨ��Э��)
	bool generateGeneralJson(string& strJson, const int nHandle);
	bool parseGeneralRespJsonData(const string& strJson, string& strReturnCode);

	// 1���� IC ����д��
	bool generateOpenReaderJson(string& strJson, const int nMode, const char* sParas);
	bool parseOpenReaderRespJson(const string& strJson, string& nHandleCode);

	// 2���ر� IC ����д��
	bool generateCloseReaderJson(string& strJson, const int nHandle);
	bool parseCloseReaderRespJson(const string& strJson, string& ReturnCode);

	// 3������ IC ����д��ͨѶ���ʣ���ѡ��
	bool generateSetInitPPSJson(string& strJson, const int nHandle, const char* sPPS);
	bool parseSetInitPPSRespJson(const string& strJson, string& ReturnCode);

	// 4������ IC ����д����ʱʱ�䣨��ѡ��
	bool generateSetInitTimeOutJson(string& strJson, const int nHandle, const char* sTimeOut);
	bool parseSetInitTimeOutRespJson(const string& strJson, string& ReturnCode);

	// 5�����ö�д����������ܣ���ѡ��
	bool generateLEDDisplayJson(string& strJson, const int nHandle, const int nRed, const int nGreen, const int nBlue);
	bool parseLEDDisplayRespJson(const string& strJson, string& ReturnCode);

	// 6�����ö�д��������
	bool generateAudioControlJson(string& strJson, const int nHandle, const int nTimes, const int nVoice);
	bool parseAudioControlRespJson(const string& strJson, string& ReturnCode);

	// 7����ȡ�豸״̬
	bool generateGetStatusJson(string& strJson, const int nHandle);
	bool parseGetStatusRespJson(const string& strJson, string& StatusCode);

	//8�� ��ȡ�豸�汾��Ϣ
	bool generateReaderVersionJson(string& strJson, const int nHandle);
	bool parseReaderVersionRespJson(const string& strJson, string& sRDVersion, string& sAPIVersion);

	//9�� ��ȡ�豸��̬��汾
	bool generateGetVersionJson(string& strJson, const int nHandle);
	bool parseGetVersionRespJson(const string& strJson, string& sVersion);

	//10����ȡ��������ϸ����
	bool generateGetStatusMsgJson(string& strJson, const int StatusCode);
	bool parseGetStatusMsgRespJson(const string& strJson, string& StatusMsg);

	//11���򿪿�Ƭ
	bool generateOpenCardJson(string& strJson, const int nHandle);
	bool parseOpenCardRespJson(const string& strJson, string& RetrunCode, string& pCardPlace, string& sPhysicsCardno);

	//12���رտ�Ƭ
	bool generateCloseCardJson(string& strJson, const int nHandle);
	bool parseCloseCardRespJson(const string& strJson, string& ReturnCode);

	//13��CPU ��ͨ��ָ���
	bool generateCPUCommandJson(string& strJson, const int nHandle, const char* sCommand);
	bool parseCPUCommandRespJson(const string& strJson, string& ReturnCode, string& sReply);

	//14��CPU ��ͨ��ָ���
	bool generateSamResetJson(string& strJson, const int nHandle, const int nSockID, const int nProtocolType);
	bool parseSamResetRespJson(const string& strJson, string& ReturnCode);

	//15��CPU ��ͨ��ָ���
	bool generateSamCommandJson(string& strJson, const int nHandle, const int nSockID, const char* sCommand);
	bool parseSamCommandRespJson(const string& strJson, string& ReturnCode, string& sReply);

	//16��M1��������Կ
	bool generateLoadKeyJson(string& strJson, const int nHandle, const int nKeyType, const int nSec, const char* sKeyValue);
	bool parseLoadKeyRespJson(const string& strJson, string& ReturnCode);

	//17��M1�����麯��
	bool generateReadBlockJson(string& strJson, const int nHandle, const int nKeyType, const int nBlock);
	bool parseReadBlockRespJson(const string& strJson, string& ReturnCode, string& sReply);

	//18��M1��д�麯��
	bool generateWriteBlockJson(string& strJson, const int nHandle, const int nKeyType, const int nBlock, const char* sData);
	bool parseWriteBlockRespJson(const string& strJson, string& ReturnCode);
};
