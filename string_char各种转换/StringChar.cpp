#include "StringChar.h"
#include <stdio.h>
#include <stdlib.h>

void string2Char()
{
	//string转char*
	string str = "hello";
	char* ch = str.c_str;

	//char*转string 
	//C
	char* ch = "hello";
	std::string str(ch);
	//C++
	char* ch = "hello";
	std::string str = ch;
}

void string2UnsignedChar()
{
	//string转unsigned char*
	string str = "hello";
	unsigned char *uc = (unsigned char *)str.c_str();//强转，可以参考下面安全转换

	//unsigned char*转string
	string str = std::string(char*)ch;
}

/**
	char* 转unsigned char* 的安全转换方法如下：
	1. 首先，我们需要定义一个指向 char 类型的指针和一个指向 unsigned char 类型的指针。
		char* charPtr;
		unsigned char* ucharPtr;
	2. 然后，我们需要将 char 类型的指针强制转换为 void* 类型的指针。
		void* voidPtr = static_cast<void*>(charPtr);
	3. 接下来，我们将 void* 类型的指针强制转换为 unsigned char* 类型的指针。
		ucharPtr = static_cast<unsigned char*>(voidPtr);
	4. 最后，我们可以使用 ucharPtr 指针访问 unsigned char 类型的数据，而不会出现任何问题。

	这种安全转换方法可以确保我们在访问 unsigned char 数据时不会出现任何问题，
	因为我们已经将 char* 指针转换为 unsigned char* 指针，而不是直接访问 char* 数据。
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
	//int转string
	int num = 123;
	std::string str = std::to_string(num);

	//double转stirng
	double d = 3.14159;
	std::string str = std::to_string(d);

/**********************************************************************************/

	//string转int
	char str[] = "123";
	int num = atoi(str);  
	printf("The integer is %d\n", num);

	//string转double
	char str[] = "3.14";
	double num = atof(str);
	printf("num = %lf\n", num);
}
