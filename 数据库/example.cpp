#include "setting.h"
/**
	实例演示：
*/

//读取注册表中的数据
QString getQtWebkitDebuggerHost()
{
	SDSetting setting;
	QString webHost = setting.value("webkitDebugSwitchHost").toString();
	return webHost;
}