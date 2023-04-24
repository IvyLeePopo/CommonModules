#include "StringChar.h"
#include <stdio.h>
#include <stdlib.h>

void string2Char()
{
	//stringתchar*
	string str = "hello";
	char* ch = str.c_str;

	//char*תstring 
	//C
	char* ch = "hello";
	std::string str(ch);
	//C++
	char* ch = "hello";
	std::string str = ch;
}

void string2UnsignedChar()
{
	//stringתunsigned char*
	string str = "hello";
	unsigned char *uc = (unsigned char *)str.c_str();//ǿת�����Բο����氲ȫת��

	//unsigned char*תstring
	string str = std::string(char*)ch;
}

/**
	char* תunsigned char* �İ�ȫת���������£�
	1. ���ȣ�������Ҫ����һ��ָ�� char ���͵�ָ���һ��ָ�� unsigned char ���͵�ָ�롣
		char* charPtr;
		unsigned char* ucharPtr;
	2. Ȼ��������Ҫ�� char ���͵�ָ��ǿ��ת��Ϊ void* ���͵�ָ�롣
		void* voidPtr = static_cast<void*>(charPtr);
	3. �����������ǽ� void* ���͵�ָ��ǿ��ת��Ϊ unsigned char* ���͵�ָ�롣
		ucharPtr = static_cast<unsigned char*>(voidPtr);
	4. ������ǿ���ʹ�� ucharPtr ָ����� unsigned char ���͵����ݣ�����������κ����⡣

	���ְ�ȫת����������ȷ�������ڷ��� unsigned char ����ʱ��������κ����⣬
	��Ϊ�����Ѿ��� char* ָ��ת��Ϊ unsigned char* ָ�룬������ֱ�ӷ��� char* ���ݡ�
*/
void char2unsignedChar()
{
	char* charPtr;
	unsigned char* ucharPtr;
	void* voidPtr = static_cast<void*>(charPtr);
	ucharPtr = static_cast<unsigned char*>(voidPtr);
}

void int2String()
{
	//intתstring
	int num = 123;
	std::string str = std::to_string(num);

	//doubleתstirng
	double d = 3.14159;
	std::string str = std::to_string(d);

/**********************************************************************************/

	//stringתint
	char str[] = "123";
	int num = atoi(str);  
	printf("The integer is %d\n", num);

	//stringתdouble
	char str[] = "3.14";
	double num = atof(str);
	printf("num = %lf\n", num);
}
