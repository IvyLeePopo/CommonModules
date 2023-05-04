/*
 * @Description: ģ���key���ȵ�һЩ�궨��
 * @Author: Ivy
 * @Date: 2022-05-04 09:34:43
 * @LastEditTime: 2022-05-04 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

enum TW_PROT_CONST
{
	TW_PROT_HEADER = 0xDF,	            //Э����ʼ��
	TW_PROT_END = 0xFD,                 //Э�������
	TW_PROT_CONVERT_CHAR = 0x00,        //Э��ת�빫���ַ�
	TW_PROT_HEAND_CONVERT_CHAR1 = 0xDE,	//Э����ʼ��ת���ַ�1
	TW_PROT_HEAND_CONVERT_CHAR2 = 0x01,	//Э����ʼ��ת���ַ�2
	TW_PROT_END_CONVERT_CHAR1 = 0xFC,	//Э����ֹ��ת���ַ�1
	TW_PROT_END_CONVERT_CHAR2 = 0x01	//Э����ֹ��ת���ַ�2
};

 //ע����KEY
enum ClassMapKeyValue
{
	KEY_HENAN_WEB_SOCKET_SERVER = 404,      //�м��ͨ�ŷ����
	KEY_HENAN_HARDWARE_CTRL = 405,          //������Ӳ��������
	KEY_SELF_HELP_ENTRY_NODE_MANAGER = 406, //����������
	KEY_SELF_HELP_EXIT_NODE_MANAGER = 407,  //�����ɷ���
	KEY_READ_CARD_NODE_MANAGER = 408,       //��������
};