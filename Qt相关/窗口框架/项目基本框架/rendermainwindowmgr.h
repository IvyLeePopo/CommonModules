/*
 * @Description: 窗口框架的管理类
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
#include <QPointer>
#include <QObject>

class RenderToolWidgetMgr;
class RenderLibraryWidgetMgr;
class RenderModelWidgetMgr;
class RenderMainWindow;
class RenderProjectWidgetMgr;
class SdWebViewContainerDialg;
class SdWebViewClient;

class RenderMainWindowMgr : public QObject
{
	Q_OBJECT
public:
	explicit RenderMainWindowMgr(QObject* parent = nullptr);

	QPointer<RenderMainWindow>  createMainWindowWidget(QWidget* parent = nullptr);
	QPointer<RenderMainWindow>  getMainWindowWidget();
	QPointer<QWidget>           getMainWindowWidgetAbstract();
	void                        showMainWindow();
	void                        clearCacheSuccessCallback(bool success);
	void                        uploadModelSuccessCallback();
	void                        sendToExitProcess();
Q_SIGNALS:
	void sigApplyMaterial(const QString &materialId);
	void sigUpdateCompanyId(const QString &companyId);
private slots:
	void onModelSwitch(const QString& workspaceId);
	void onShowLoading(const QString &loadingText);
	void onHideLoading(bool fillLastProgress = true);
	void onOperatingInstructions();
private:
	void initSignalConnection();
	void initToolWidgetMgrSC();
	void initModelWidgetMgrSC();
	void initLibraryWidgetMgrSC();
	void initProjectWidgetMgrSC();
private:
	QPointer<RenderMainWindow>          m_mainWindowWidget;
	QPointer<RenderToolWidgetMgr>       m_toolWidgetMgr;
	QPointer<RenderModelWidgetMgr>      m_renderModelWidgetMgr;
	QPointer<RenderLibraryWidgetMgr>    m_libraryWidgetMgr;
	QPointer<RenderProjectWidgetMgr>    m_projectWidgetMgr;
	// 操作帮助页面
	QPointer<SdWebViewContainerDialg>   m_helpWebViewContainerDialg;
	QPointer<SdWebViewClient>           m_helpWwebViewClient;
};
