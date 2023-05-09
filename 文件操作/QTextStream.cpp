#include "QTextStream.h"

void newQTextStream()
{
	QFile file("example.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream in(&file); // 从文件读取文本
}

//QTextStream 也有许多写入操作，可以写入单个字符、字符串、数值等，例如：
void read()
{
	QChar c = in.read(); // 读取单个字符
	QString word = in.readWord(); // 读取一个单词
	QString line = in.readLine(); // 读取一行文本
}

//QTextStream 也有许多写入操作，可以写入单个字符、字符串、数值等
void write()
{
	out << "Hello, world!" << endl; // 写入字符串和换行符
	out << 42 << " is the answer." << endl; // 写入数值和字符串
}

//QTextStream 支持格式化输出，例如可以指定输出的精度、宽度、填充字符等
void cout()
{
	out.setFieldWidth(10);
	out.setPadChar('-');
	out.setRealNumberPrecision(4);
	out << 3.14159265358979323846 << endl; // 输出：--3.1416
}

//QTextStream 可以控制读写位置，可以获取当前读写位置、设置读写位置等
void ctrPos()
{
	int pos = in.pos(); // 获取当前读取位置
	in.seek(0); // 设置读取位置为文件开头
}

//QTextStream 可以设置字符编码
void setCode()
{
	in.setCodec("UTF-8"); // 设置输入流的字符编码为 UTF-8
	out.setCodec("GBK"); // 设置输出流的字符编码为 GBK
}
