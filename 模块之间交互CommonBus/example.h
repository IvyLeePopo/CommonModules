#include "Jpeg2Bmp.h"
/**
	实例演示：包含其头文件，直接用
*/

#include "CommonBus.h"

class Test : public CommonBus
{
	Q_OBJECT
public:
	Test(QObject *parent = nullptr);
	~Test();
	void sendMsg();
	int recvFromBus(int destKey, char *msg, int len, int srcKey = 0);// 重载这个函数
};
}