// 1��д��־
#include "AutoLogEx.h"
#include "Helper.h"
AutoLogEx::getInstance()->startLog(Helper->getModulePath() + "\\LogFileCamera\\", "CameraControlApp");


// 2����ȡ�����ļ��е�ֵ
// ��ʼ����־
string strConfigFilePath = Helper->getModulePath() + "\\CameraConfig.ini";
// ��config.ini�ļ��ж�ȡ[Section]���µ�"Key"����Ӧ��ֵ
bool bSaveLog = (BOOL)::GetPrivateProfileIntA(_T("Log"), _T("bSaveLog"), 0, strConfigFilePath.c_str());
int nSaveDays = ::GetPrivateProfileIntA(_T("Log"), _T("nSaveDays"), 60, strConfigFilePath.c_str());

if (bSaveLog)
{
	printf("Ҫ���������ļ���\n");
	AutoLogEx::getInstance()->startLog(Helper->getModulePath() + "\\LogFileCamera\\", "CameraControlApp");
}

printf("�����ļ�����������Ϊ��%d��\n", nSaveDays);
if (nSaveDays > 0)
	AutoLogEx::getInstance()->deleteLogFile(Helper->getModulePath() + "\\LogFileCamera\\", nSaveDays);