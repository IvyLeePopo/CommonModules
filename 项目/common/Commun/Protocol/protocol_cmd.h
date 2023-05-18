#pragma once

#define MSG_HEAD_LEN    4    //֡ͷ���ֽڳ��ȣ�02 03 04 05��
#define MSG_LENGTH_LEN  4    //��ʾ֡���ȵ���ֵ���ֽڳ��ȣ�XX XX XX XX��unsigned int�ͣ�
#define MSG_TAIL_LEN    1    //֡β���ֽڳ��ȣ�03��
#define MSG_FRONT_LEN   MSG_HEAD_LEN + MSG_LENGTH_LEN    //��֡DATA��֮ǰ���ֽڳ���
#define MSG_MIN_LEN     MSG_HEAD_LEN + MSG_LENGTH_LEN + MSG_TAIL_LEN    //��֡��С�ֽڳ���

#define TCP_HEART_BEAT_INTERVAL 10 * 1000  //���������ͼ��
#define TCP_RECONNECT_INTERVAL 10 * 1000  //�����������

typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef unsigned long long      uint64;

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
	SUB_CMD_TYPE_OpenReader = 0x01,			//��
	SUB_CMD_TYPE_CloseReader = 0x02,		//�ر�
}eSubCmdType_t;