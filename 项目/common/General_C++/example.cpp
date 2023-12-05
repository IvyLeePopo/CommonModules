// 1、写日志
#include "AutoLogEx.h"
#include "Helper.h"
AutoLogEx::getInstance()->startLog(Helper->getModulePath() + "\\LogFileCamera\\", "CameraControlApp");


// 2、读取配置文件中的值
// 初始化日志
string strConfigFilePath = Helper->getModulePath() + "\\CameraConfig.ini";
// 从config.ini文件中读取[Section]节下的"Key"键对应的值
bool bSaveLog = (BOOL)::GetPrivateProfileIntA(_T("Log"), _T("bSaveLog"), 0, strConfigFilePath.c_str());
int nSaveDays = ::GetPrivateProfileIntA(_T("Log"), _T("nSaveDays"), 60, strConfigFilePath.c_str());

if (bSaveLog)
{
	printf("要保存配置文件！\n");
	AutoLogEx::getInstance()->startLog(Helper->getModulePath() + "\\LogFileCamera\\", "CameraControlApp");
}

printf("配置文件保留的天数为：%d！\n", nSaveDays);
if (nSaveDays > 0)
	AutoLogEx::getInstance()->deleteLogFile(Helper->getModulePath() + "\\LogFileCamera\\", nSaveDays);