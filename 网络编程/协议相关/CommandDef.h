#pragma once

//����֡�������
typedef enum ePROT_CONST
{
	PROT_HEADER = 0xDF,						//Э����ʼ��
	PROT_END = 0xFD,						//Э�������
	PROT_NUMBER = 0x00,						//Э��֡���
	PROT_CONVERT_CHAR = 0x00,				//Э��ת�빫���ַ�
	PROT_HEAND_CONVERT_CHAR1 = 0xDE,		//Э����ʼ��ת���ַ�1
	PROT_HEAND_CONVERT_CHAR2 = 0x01,		//Э����ʼ��ת���ַ�2
	PROT_END_CONVERT_CHAR1 = 0xFC,			//Э����ֹ��ת���ַ�1
	PROT_END_CONVERT_CHAR2 = 0x01			//Э����ֹ��ת���ַ�2
}ePROT_CONST_t;

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