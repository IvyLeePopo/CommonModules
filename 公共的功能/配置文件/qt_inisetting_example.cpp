#include "qt_inisetting.h"

//备注：这个类默认了配置文件的路径，这个可以优化下

IniSetting::init();         // 初始化配置文件
if (IniSetting::value("/AddressPort/LocalEth0Addr").toString() == "") {
	IniSetting::setValue("/AddressPort/LocalEth0Addr", "192.168.1.136");
}