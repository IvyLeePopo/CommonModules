#include "LinuxCmd.h"
/**
	实例演示：
*/


//要是当前没有root权限时候，可以用这
/***
 * echo 88 > /sys/class/gpio/export
 * echo out > /sys/class/gpio/gpio88/direction
 * echo 0 > /sys/class/gpio/gpio88/value
 * echo 1 > /sys/class/gpio/gpio88/value
 */
void test()
{
	QString cmd = QString("echo %1 > /sys/class/gpio/export").arg(strGpio);
	exeCmd(cmd);

	cmd = QString("echo out > /sys/class/gpio/gpio%1/direction").arg(strGpio);
	exeCmd(cmd);

	cmd = QString("echo %1 > /sys/class/gpio/gpio%2/value").arg(operation).arg(strGpio);
	exeCmd(cmd);
}

//通过读写文件的形式，获取某个gpio口的值
void test1()
{
	//给gpio95赋值1
	setGpioCtrl("95", 1);

	//获取gpio95的值
	int status = getGpioCtrl("95");
}