#include "qt_inisetting.h"

//��ע�������Ĭ���������ļ���·������������Ż���

IniSetting::init();         // ��ʼ�������ļ�
if (IniSetting::value("/AddressPort/LocalEth0Addr").toString() == "") {
	IniSetting::setValue("/AddressPort/LocalEth0Addr", "192.168.1.136");
}