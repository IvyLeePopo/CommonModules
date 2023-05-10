
#include "pch.h"
#include "rendermainwindowmgr.h"
#include "api/cadviewcontext.h"
#include "application/pubglobalinstances.h"
#include "common/dpiscaled.h"
#include "common/defs.h"
#include "localsocket/sdlocalclient.h"
#include "mainwindowtoolpanel/rendertoolwidgetmgr.h"
#include "rendermainwindow.h"
#include "renderpanel/rendermodelwidgetmgr.h"
#include "librarypanel/librarywidgetmgr.h"
#include "projectpanel/projectwidgetmgr.h"
#include "urlmanager/urldef.h"
#include "core/document.h"
#include "core/documentcenter.h"
#include "web/sdwebviewclient.h"
//TODO newpc
#include <QJsonObject>
#include "datatransfermgr/downloadmgr/downloadmanager.h"
#include "general/modeldatadef.h"
#include "general/pathutils.h"
RenderMainWindowMgr::RenderMainWindowMgr(QObject* parent)
	: QObject(parent)
{

}

QPointer<RenderMainWindow> RenderMainWindowMgr::createMainWindowWidget(QWidget* parent)
{
	QSize screenSize = QGuiApplication::primaryScreen()->size();
	m_mainWindowWidget = new RenderMainWindow(parent, Qt::Window | Qt::FramelessWindowHint);
	m_mainWindowWidget->setMinimumSize(_SS(1366, 768));
	m_mainWindowWidget->resize(_SS(1366, 768));
	m_mainWindowWidget->move((screenSize.width() - m_mainWindowWidget->width()) / 2, (screenSize.height() - m_mainWindowWidget->height()) / 2);
	m_toolWidgetMgr = new RenderToolWidgetMgr(this);
	QWidget* mainWindowMenuBar = m_toolWidgetMgr->createToolMenuBarWidget(m_mainWindowWidget);
	m_renderModelWidgetMgr = new RenderModelWidgetMgr(this);
	QWidget* centralWidget = m_renderModelWidgetMgr->getRenderModelWidget(m_mainWindowWidget);
	m_libraryWidgetMgr = new RenderLibraryWidgetMgr(this);
	QWidget* libraryWidget = m_libraryWidgetMgr->createLibraryWidget(m_mainWindowWidget);
	m_projectWidgetMgr = new RenderProjectWidgetMgr(m_mainWindowWidget, this);
	m_mainWindowWidget->initUi(mainWindowMenuBar, libraryWidget, nullptr, centralWidget);
	initSignalConnection();
	return m_mainWindowWidget;
}

QPointer<RenderMainWindow> RenderMainWindowMgr::getMainWindowWidget()
{
	return m_mainWindowWidget;
}

QPointer<QWidget> RenderMainWindowMgr::getMainWindowWidgetAbstract()
{
	return qobject_cast<QWidget*>(m_mainWindowWidget);
}

void RenderMainWindowMgr::showMainWindow()
{
	m_mainWindowWidget->show();
	m_mainWindowWidget->activateWindow();
}

void RenderMainWindowMgr::clearCacheSuccessCallback(bool success)
{
	if (m_toolWidgetMgr)
	{
		m_toolWidgetMgr->clearCacheSuccess(success);
	}
}

void RenderMainWindowMgr::uploadModelSuccessCallback()
{
	if (m_libraryWidgetMgr)
	{
		m_libraryWidgetMgr->refreshModelListCur();
	}
}
void RenderMainWindowMgr::sendToExitProcess()
{
	CheckResult(LocalClientInstance);
	QJsonObject jsonToServer;
	jsonToServer.insert("state", RIS_Exit);
	LocalClientInstance->syncCall(LSMT_RequestLogout, jsonToServer);
}
void RenderMainWindowMgr::initSignalConnection()
{
	initToolWidgetMgrSC();
	initModelWidgetMgrSC();
	initLibraryWidgetMgrSC();
	initProjectWidgetMgrSC();
	if (m_mainWindowWidget)
	{
		connect(m_mainWindowWidget, &RenderMainWindow::sigFoldSwitch, this, [=](FoldSwitchSource src, bool isFold) {
			if (src == FSS_Left && m_libraryWidgetMgr)
			{
				if (isFold)
					m_libraryWidgetMgr->shrinkWidget();
			}
			else if (src == FSS_Right && m_projectWidgetMgr)
			{
				if (isFold)
					m_projectWidgetMgr->shrinkWidget();
			}
		});
		if (m_libraryWidgetMgr)
		{
			connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigExpand, this, [=]() {
				m_mainWindowWidget->foldSwitch(FSS_Left, false);
			});
			connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigBtnClicked, this, [=](const QString &text) {
				m_mainWindowWidget->sigDockTitleChange(text, FSS_Left);
			});
		}
		if (m_projectWidgetMgr)
		{
			connect(m_projectWidgetMgr, &RenderProjectWidgetMgr::sigExpand, this, [=]() {
				m_mainWindowWidget->foldSwitch(FSS_Right, false);
			});
			connect(m_projectWidgetMgr, &RenderProjectWidgetMgr::sigBtnClicked, this, [=](const QString &text) {
				m_mainWindowWidget->sigDockTitleChange(text, FSS_Right);
			});
		}
		if (m_toolWidgetMgr)
		{
			connect(m_mainWindowWidget, &RenderMainWindow::sigChangeHeight, this, [=](int height) {
				m_toolWidgetMgr->updateMainWindowHeight(height);
			});
		}
	}
}
void RenderMainWindowMgr::initToolWidgetMgrSC()
{
	if (m_toolWidgetMgr)
	{
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigOpen, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->openModel();
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigSave, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->save();
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigSaveAs, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->saveAs();
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigExport, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->exportModel();
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigHistoryVersion, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->showHistoryVersion();
		});
		// 编辑
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigUndo, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->undo();
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigRedo, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->redo();
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigPreferenceSetting, this, [=]() {
			//int a = 0;
		});
		// 相机
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigSwitchPerspective, this, [=](int type) {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->switchDisplayOrthoMode((RenderOrthoPersType)type);
			if (m_projectWidgetMgr)
				m_projectWidgetMgr->switchDisplayOrthoMode((RenderOrthoPersType)type);
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigSwitchViewType, this, [=](int type) {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->switchRenderView((RenderViewType)type);
		});
		// 配色
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigGenerateColorway, this, [=]() {
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigShowColorwayList, this, [=]() {
		});
		// 渲染
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigRenderMode, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigCloudRenderSetting, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigCloudRenderProgress, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigDesignAlbum, this, [=]() {});
		// 工具
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigExplodedView, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->explodedView();
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigCostEstimate, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigModelQuality, this, [=](int type) {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->modelQuality((ModelQualityType)type);
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigLineDraftMaterial, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->showLineDraftMaterial();
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigRendererType, this, [=](int type) {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->switchRendererType(type);
		});


		// 团队
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigMark, this, [=]() {
			if (m_renderModelWidgetMgr)
				m_renderModelWidgetMgr->showModerMark();
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigMeasure, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigInvitation, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigShare, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigAttachment, this, [=]() {});
		// 界面
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigWindowLayout, this, [=]() {});
		// 用户
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigProfileSettings, this, [=]() {
			QString profileUrl = urldef::getUrl(UT_ProfileInfo);
			QDesktopServices::openUrl(QUrl(profileUrl));
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigIdentitySwitching, this, [=](const QString& companyId) {
			emit sigUpdateCompanyId(companyId);
			CheckResult(LocalClientInstance);
			LocalClientInstance->asynCall(LSMT_RequestRestart);
		});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sitExit, this, [=]() {
			CheckResult(LocalClientInstance);
			LocalClientInstance->asynCall(LSMT_RequestLogout);
		});
		// 帮助
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigOperatingInstructions, this, &RenderMainWindowMgr::onOperatingInstructions);
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigComment, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigFeedback, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigCheckUpdates, this, [=]() {});
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigAboutUs, this, [=]() {});
		//关闭
		connect(m_toolWidgetMgr, &RenderToolWidgetMgr::sigClose, this, [=]() {
			sendToExitProcess();
		});
		if (m_renderModelWidgetMgr)
		{
			connect(m_renderModelWidgetMgr, &RenderModelWidgetMgr::sigUpdateToolStatus, m_toolWidgetMgr, &RenderToolWidgetMgr::onUpdateToolStatus);
		}
	}
}
void RenderMainWindowMgr::initModelWidgetMgrSC()
{
	CheckResult(m_libraryWidgetMgr);
	CheckResult(m_renderModelWidgetMgr);
	connect(m_renderModelWidgetMgr, &RenderModelWidgetMgr::sigModelSwitch, this, &RenderMainWindowMgr::onModelSwitch);
	connect(m_renderModelWidgetMgr, &RenderModelWidgetMgr::sigOpenModelFinish, m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigOpenModelFinish);
	connect(m_renderModelWidgetMgr, &RenderModelWidgetMgr::sigSaveModelFinish, m_libraryWidgetMgr, &RenderLibraryWidgetMgr::onSaveModelFinish);
	connect(m_renderModelWidgetMgr, &RenderModelWidgetMgr::sigShowLoading, this, &RenderMainWindowMgr::onShowLoading);
	connect(m_renderModelWidgetMgr, &RenderModelWidgetMgr::sigHideLoading, this, &RenderMainWindowMgr::onHideLoading);
}
void RenderMainWindowMgr::initLibraryWidgetMgrSC()
{
	CheckResult(m_libraryWidgetMgr);
	CheckResult(m_renderModelWidgetMgr);
	connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigOpenModel,
		m_renderModelWidgetMgr, &RenderModelWidgetMgr::onSigOpenModel);
	connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigMaterialItemLClicked,
		m_renderModelWidgetMgr, &RenderModelWidgetMgr::onSigMaterialItemLClicked);
	connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigClrItemLClicked,
		m_renderModelWidgetMgr, &RenderModelWidgetMgr::onSigClrItemLClicked);
	connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigPatternItemLClicked,
		m_renderModelWidgetMgr, &RenderModelWidgetMgr::onSigPatternItemLClicked);
	connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigTextureItemLClicked,
		m_renderModelWidgetMgr, &RenderModelWidgetMgr::onSigTextureItemLClicked);
	connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigUpdateRenderEnv,
		m_renderModelWidgetMgr, &RenderModelWidgetMgr::onSigUpdateRenderEnv);
	connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigUpdateRenderLight,
		m_renderModelWidgetMgr, &RenderModelWidgetMgr::onSigUpdateRenderLight);
	connect(m_libraryWidgetMgr, &RenderLibraryWidgetMgr::sigUpdateProjectEvnData,
		m_projectWidgetMgr, &RenderProjectWidgetMgr::sigUpdateProjectEvnData);
}
void RenderMainWindowMgr::initProjectWidgetMgrSC()
{
	CheckResult(m_libraryWidgetMgr);
	CheckResult(m_projectWidgetMgr);
	CheckResult(m_mainWindowWidget);
	connect(core::DocumentCenter::instance(), &core::DocumentCenter::sigActiveDocument, this, [this](const QString &id) {
		QWidget* widget = m_projectWidgetMgr->switchToProjectWidget(id);
		m_mainWindowWidget->setRightDockWidget(widget);
	});
	connect(core::DocumentCenter::instance(), &core::DocumentCenter::sigDocumentClosed, this, [this](const QString &id) {
		m_projectWidgetMgr->closeProjectWidget(id);
	});
}
void RenderMainWindowMgr::onModelSwitch(const QString& workspaceId)
{
	if (m_libraryWidgetMgr)
	{
		m_libraryWidgetMgr->addWorkspaceLib(workspaceId);
		m_libraryWidgetMgr->selectWorkspaceLib(workspaceId);
	}
}
void RenderMainWindowMgr::onShowLoading(const QString &loadingText)
{
	CheckResult(m_mainWindowWidget);
	m_mainWindowWidget->showLoading(loadingText);
}
void RenderMainWindowMgr::onHideLoading(bool fillLastProgress/* = true*/)
{
	CheckResult(m_mainWindowWidget);
	m_mainWindowWidget->hideLoading(fillLastProgress);
}
void RenderMainWindowMgr::onOperatingInstructions()
{
	CheckResult(m_mainWindowWidget);
	QString renderHelpUrl = urldef::getUrl(UT_RenderHelp);
	if (!m_helpWebViewContainerDialg)
		m_helpWebViewContainerDialg = new SdWebViewContainerDialg(m_mainWindowWidget);
	if (!m_helpWwebViewClient)
	{
		m_helpWwebViewClient = new SdWebViewClient(renderHelpUrl, this, WebViewDef::WVT_Normal, m_helpWebViewContainerDialg, true);
		connect(m_helpWwebViewClient, &SdWebViewClient::sigViewClose, this, [=](QString params) {
			if (m_helpWebViewContainerDialg)
				m_helpWebViewContainerDialg->close();
		});
	}
	else
	{
		m_helpWwebViewClient->load(renderHelpUrl);
	}
	m_helpWebViewContainerDialg->setGeometry(m_mainWindowWidget->geometry());
	m_helpWebViewContainerDialg->exec();
}
