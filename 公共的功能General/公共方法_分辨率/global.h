//#include "global.h"
#pragma once
#include <QDir>
#include <qglobal.h>

namespace global
{
	QString appUniqueGUID();//获得app唯一的GUID

	//分辨率
	qreal dpiScaled(qreal value);//dpi缩放
	qreal dpiDeScaled(qreal value);//dpi缩小比例
	qreal dpiValue();

	//分辨率相关的路径：主要是设置样式，取不同路径的图片（这种会有弊端，只能放太多少倍，最好是用svg格式切图）
	QString getResDPIRelatePath();
	QString getResDPIFullPath(const QString &subPath);
	QPixmap getResImgDPIFullPath(const QString &imgName, const QString &ext = ".png");

	QString getHostModulePath();
	QString getThisModulePath();
	QString getAppRoamingPath();
	QString getApplicationDir();
	int getChannelID();

	//适配1366显示器窗体大小
	QSize getWindowSizeFromScreen();
	QString userAgent();
	QString getWindowTitle();

	//version
	QString getVersion();
	QString getFileVersion(const std::wstring& filepath);
	int compareVersion(const QString& src, const QString& dest);

	//>update
	QString getMainUpdateUrl();
	void startNewProcess(const QString& exePath, const QString& command);
	bool deleteDir(const QString& path);

	//>check AI install,get AI path
	bool isInstallIllustrator(QString& illustratorPath);
	void setForgroundWindow(QWidget *widget, bool isForce = true);
	QString makeGmtNow();
	bool checkShowRedPoint();
	void updateRemindPoint();

	//diff package
	QString getPkgTempDir(); //>获取缓存安装包路径 deprecated
	QString getCachePkgPath(); //>获取缓存安装包文件 deprecated
	QString getFunctionZipPkgPath(); //> 获取功能安装包路径

	//>lazy auto update
	void setLazyAutoUpdateFlag(const QString& params);
	void removeLayAutoUpdateFlags();
	QString getLazyAutoUpdate();
	bool checkExistLazyUpdateFlags();
	int getLazyUpdateDeltaTime();
};
