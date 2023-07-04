#pragma once

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
	SUB_CMD_TYPE_SetInitPPS = 0x03,			//���� IC ����д��ͨѶ���ʣ���ѡ��
	SUB_CMD_TYPE_SetInitTimeOut = 0x04,     //���� IC ����д����ʱʱ�䣨��ѡ��
	SUB_CMD_TYPE_LEDDisplay = 0x05,         //���ö�д�����������
	SUB_CMD_TYPE_AudioControl = 0x06,       //���ö�д��������
	SUB_CMD_TYPE_GetStatus = 0x07,          //��ȡ�豸״̬
	SUB_CMD_TYPE_ReaderVersion = 0x08,      //��ȡ�豸�汾��Ϣ
	SUB_CMD_TYPE_GetVersion = 0x09,         //��ȡ�豸��̬��汾
	SUB_CMD_TYPE_GetStatusMsg = 0x10,       //��ȡ��������ϸ����
	SUB_CMD_TYPE_OpenCard = 0x11,           //�򿪿�Ƭ
	SUB_CMD_TYPE_CloseCard = 0x12,          //�رտ�Ƭ
	SUB_CMD_TYPE_CPUCommand = 0x13,         //CPU ��ͨ��ָ���
	SUB_CMD_TYPE_SamReset = 0x14,           //SAM ����λ
	SUB_CMD_TYPE_SamCommand = 0x15,         //SAM ��ͨ��ָ���
	SUB_CMD_TYPE_LoadKey = 0x16,            //M1 ��������Կ����ѡ��
	SUB_CMD_TYPE_ReadBlock = 0x17,          //M1 �����麯������ѡ��
	SUB_CMD_TYPE_WriteBlock = 0x18,         //M1 ��д�麯������ѡ��
}eSubCmdType_t;