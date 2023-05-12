//#include "global.h"
#pragma once
#include <QDir>
#include <qglobal.h>

namespace global
{
	QString appUniqueGUID();//���appΨһ��GUID

	//�ֱ���
	qreal dpiScaled(qreal value);//dpi����
	qreal dpiDeScaled(qreal value);//dpi��С����
	qreal dpiValue();

	//�ֱ�����ص�·������Ҫ��������ʽ��ȡ��ͬ·����ͼƬ�����ֻ��б׶ˣ�ֻ�ܷ�̫���ٱ����������svg��ʽ��ͼ��
	QString getResDPIRelatePath();
	QString getResDPIFullPath(const QString &subPath);
	QPixmap getResImgDPIFullPath(const QString &imgName, const QString &ext = ".png");

	QString getHostModulePath();
	QString getThisModulePath();
	QString getAppRoamingPath();
	QString getApplicationDir();
	int getChannelID();

	//����1366��ʾ�������С
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
	QString getPkgTempDir(); //>��ȡ���氲װ��·�� deprecated
	QString getCachePkgPath(); //>��ȡ���氲װ���ļ� deprecated
	QString getFunctionZipPkgPath(); //> ��ȡ���ܰ�װ��·��

	//>lazy auto update
	void setLazyAutoUpdateFlag(const QString& params);
	void removeLayAutoUpdateFlags();
	QString getLazyAutoUpdate();
	bool checkExistLazyUpdateFlags();
	int getLazyUpdateDeltaTime();
};
