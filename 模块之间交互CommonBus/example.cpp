#include "example.h"
/**
	实例演示：包含其头文件，直接用
*/

Test::Test(QObject *parent)
{
	register2Bus(KEY_HENAN_HARDWARE_CTRL);// 注册key
}

Test::~Test()
{
	unRegister2Bus(KEY_HENAN_HARDWARE_CTRL);// 注销key
}

vodi Test::sendMsg()
{
	//send2Bus(目标的key, msg, msgLen, 自己的key);
	send2Bus(KEY_SELF_HELP_ENTRY_NODE_MANAGER, verData.data(), 1, KEY_HENAN_HARDWARE_CTRL);
}

int Test::recvFromBus(int destKey, char *msg, int len, int srcKey = 0)
{

}