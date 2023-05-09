#include "example.h"

GlobalConf::GlobalConf()
{
	//共同IP和端口
	string strIp = "10.243.10.87";
	int nPort = 6004;

	m_strNodeIp = strIp;
	m_nNodePort = nPort;

	init();
}

GlobalConf::~GlobalConf()
{
}

void GlobalConf::init()
{
	loadConf();
}

void GlobalConf::loadConf()
{
	//获取配置文件的路径
	string strCfgFilePath = Helper->getAppDirPath() + "/HunanServiceConfig.conf";
	Config cfgFile;
	if (!cfgFile.FileExist(strCfgFilePath))//如果不存在，就写，存在就直接读
	{
		string strContent = format("\
###岛尾智能节点IP\nNodeIp=%s\n\n\
###岛尾智能节点端口\nNodePort=%d\n\n\
",
										m_strNodeIp.c_str(),
										m_nNodePort,
										);
		write2File(strCfgFilePath, strContent);
		return;
	}

	cfgFile.ReadFile(strCfgFilePath.c_str());//读配置文件

	m_strENodeIp = cfgFile.Read("NodeIp", m_strEndSmartNodeIp);
	m_nNodePort = cfgFile.Read("NodePort", m_nEndSmartNodePort);
}


/**
vsnprintf：
	vsnprintf 是一个函数，用于将格式化的字符串输出到指定的缓冲区中，其参数列表中包含一个可变参数列表。
	与 printf 类似，vsnprintf 可以根据格式化字符串中的占位符，将后续参数转换成对应的字符串并输出到缓冲区中，
	但是与 printf 不同的是，vsnprintf 可以指定输出的最大长度，以避免缓冲区溢出的问题。
*/

/**
va_start是一个宏定义，用于在使用可变参数函数时获取可变参数的起始地址。
	它的作用是初始化一个va_list类型的变量，使其指向可变参数列表的第一个参数。
	va_start宏需要两个参数，分别是一个va_list类型的变量和可变参数的最后一个固定参数的地址。
*/
string GlobalConf::format(const char * pFmt, ...)
{
#define FORMAT_MSG_BUFFER_SIZE (2048000)
	char szBuffer[FORMAT_MSG_BUFFER_SIZE + 1] = { 0 };
	va_list args;
	va_start(args, pFmt);
	vsnprintf(szBuffer, FORMAT_MSG_BUFFER_SIZE, pFmt, args);
	va_end(args);

	std::string strRet = szBuffer;
	return strRet;
}

bool GlobalConf::write2File(string strFilePath, string & strContent)
{
	ofstream out(strFilePath.c_str());
	out << strContent.c_str() << endl;
	out.close();
	return true;
}