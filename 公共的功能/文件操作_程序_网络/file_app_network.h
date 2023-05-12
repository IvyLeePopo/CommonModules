/*
 * @Description: 文件操作_程序_网络
 * @Author: Ivy
 * @Date: 2022-05-12 09:34:43
 * @LastEditTime: 2022-05-12 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

namespace app_network
{
	// 1、获取进程运行的路径，调用这后面自己加"\"
	string getProcessPath();
	// 获取app运行路径--qt
	string getAppDirPath();

	// 2、判断软件是否重复启动
	bool IsRepeatStart(const QRect& rect);

	// 3、从例“192.168.0.11,8000,admin,password”分离出来各个数据
	bool splitIpAndPort(const char* pszData, string& strIp, int& nPort, string& strUser, string& strPasswd);

	// 4、获取dll、exe的版本、公司等信息
	bool getDllExeVersion(const string& strName, string& strCompany, string& strVersion, string& strDescrip);

	// 5、 判断ip是否合法
	bool isVaildIp(const char* pszIp);

	// 6、将ip替换成域名
	void replaceIp2Domain(string& strUrl, string strDomain, bool bIsReplaceHttp2Https = false);

	// 7、创建随机32位UID
	string generate32UID();
}

namespace linux
{
	// 1、执行shell命令
	bool excuteCmd(string strCmd);

	// 2、启动一个进程（linux）
	void runProcess(const char* pszName, const char* pszCmd, bool bKill = true);

	// 3、断一个进程是否存在（linux）
	bool isExistProcess(const char *pszProcessName);
}
