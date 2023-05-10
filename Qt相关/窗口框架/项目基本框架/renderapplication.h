/*
 * @Description: 项目应用最外层 1
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
#include <QPointer>
#include "application/applicationbase.h"
class QDialog;
class SdLoginManager;
class DataManager;
class SdLocalClient;
class VirDesignSubWidget;
class RenderMainWindowMgr;
class MatLibFilterManager;

class RenderApplication : public ApplicationBase
{
	Q_OBJECT
public:
	RenderApplication(int& argc, char ** argv);
	~RenderApplication();
	bool OnAppInit() override;
	void OnAppExit() override;
	// 渲染主窗口指针
	QWidget * designMainWidget() override;

protected:
	bool notify(QObject *obj, QEvent *event) override;

private Q_SLOTS:
	void onLocalClientConnected();
	void onLocalClientDisconnected();
	void onInitProgressSuccess();
	void onClearCacheSuccess(bool success);
	void onUploadModelSuccess();

private:
	void initInfoFromServer(const QJsonObject& json);
	void sendProgressVal(int clientId, int initProgress, int initState);
	QPointer<SdLoginManager>        m_loginManager;
	QPointer<DataManager>           m_dataManager;
	QPointer<SdLocalClient>         m_localClient;
	QPointer<RenderMainWindowMgr>   m_mainwindowMgr;
	QPointer<MatLibFilterManager>   m_libFilterMgr;
	int                             m_initProgress = 0;
};
