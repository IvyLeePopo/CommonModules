#pragma once
/**
	实例演示：包含其头文件，直接用
*/

#include "ConfigFile.h"

// 定义get\set模块
#define GETSET_FUNC(Type, key, Name) \
    private: \
        Type m_##key##Name; \
    public: \
        void set##Name(Type Name) \
        { \
            m_##key##Name = Name; \
        } \
        Type get##Name() \
        { \
            return m_##key##Name; \
        }

class GlobalConf : public Singleton<GlobalConf>
{
	friend class Singleton<GlobalConf>;
public:
	GlobalConf();
	virtual ~GlobalConf();

	GETSET_FUNC(string, str, NodeIp)        // 节点IP
	GETSET_FUNC(int, n, NodePort)           // 节点端口

public:
	// 初始化
	void init();

	// 读取文件信息
	void loadConf();

private:
	/**
	format函数是一个可变参数函数，第一个参数pFmt是一个格式化字符串，后面的参数可以是任意数量的参数，
	用于填充格式化字符串中的占位符。该函数将格式化字符串和参数组合成一个新的字符串并返回。

	例如，如果格式化字符串为"Hello, %s! Today is %d/%d/%d."，后面的三个参数分别为"World"、"10"和"2021"，
	那么该函数返回的字符串为"Hello, World! Today is 10/2021."。

	在使用该函数时，需要注意保证格式化字符串和参数的类型和数量匹配，否则可能会导致程序崩溃或输出错误的结果
	*/
	string format(const char *pFmt, ...);
	bool write2File(string strFilePath, string& strContent);
}

#define GLOBALDATA GlobalConf::getInstance()