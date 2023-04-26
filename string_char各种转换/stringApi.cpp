#include "stringApi.h"

/**
* @str参数是要查找的字符串，
* @c参数是要查找的字符。
* @返回值：
	如果找到了该字符，则返回该字符在字符串中的指针位置；
	如果没有找到，则返回NULL。
*/
char *strchr(const char *str, int c)
{
	char str[] = "Hello, world!";
	char *ptr = strchr(str, 'o');
	//找到字符：'o'，位置是：4
}

/**
	str是指向要存储输出结果的缓冲区的指针；
	size是缓冲区的大小；
	format是一个格式化字符串，用于指定输出的格式；
	最后的省略号表示可变参数，用于指定输出的数据。
*/
int snprintf(char * str, size_t size, const char * format, ...)
{
	char m_cDevArgs[512 + 64] = { 0 };
	char cStrIp[] = "192.168.1.250";
	char cPort[] = "21003";
	snprintf(m_cDevArgs, sizeof(m_cDevArgs), "ServerIP = %s, Port = %s, TimeoutMs = 1000", cStrIp, cPort);
	//输出m_cDevArgs：ServerIP = 192.168.1.250, Port = 21003, TimeoutMs = 1000
}

