#include "stringApi.h"

/**
* @str������Ҫ���ҵ��ַ�����
* @c������Ҫ���ҵ��ַ���
* @����ֵ��
	����ҵ��˸��ַ����򷵻ظ��ַ����ַ����е�ָ��λ�ã�
	���û���ҵ����򷵻�NULL��
*/
char *strchr(const char *str, int c)
{
	char str[] = "Hello, world!";
	char *ptr = strchr(str, 'o');
	//�ҵ��ַ���'o'��λ���ǣ�4
}

/**
	str��ָ��Ҫ�洢�������Ļ�������ָ�룻
	size�ǻ������Ĵ�С��
	format��һ����ʽ���ַ���������ָ������ĸ�ʽ��
	����ʡ�Ժű�ʾ�ɱ����������ָ����������ݡ�
*/
int snprintf(char * str, size_t size, const char * format, ...)
{
	char m_cDevArgs[512 + 64] = { 0 };
	char cStrIp[] = "192.168.1.250";
	char cPort[] = "21003";
	snprintf(m_cDevArgs, sizeof(m_cDevArgs), "ServerIP = %s, Port = %s, TimeoutMs = 1000", cStrIp, cPort);
	//���m_cDevArgs��ServerIP = 192.168.1.250, Port = 21003, TimeoutMs = 1000
}

