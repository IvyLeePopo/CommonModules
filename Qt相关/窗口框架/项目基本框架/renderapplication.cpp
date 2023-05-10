
#include "pch.h"
#include "renderapplication.h"
#include "profiler/profilerdefs.h"
#include "application/sdloginmanager.h"
#include "application/pubglobalinstances.h"
#include "application/settingsmgr.h"
#include "application/renderversion.h"
#include "datamanager/datamanager.h"
#include "datamanager/material/matlibfiltermanager.h"
#include "globalinstances.h"
#include "localsocket/sdlocalclient.h"
#include "localsocketfunc.h"
#include "datatransfermgr/serverpipesdk/resource-sdk.h"
#include "thread/sdthread.h"
#include "common/dpiscaled.h"
#include "core/documentcenter.h"
#include "renderwidget/rendermainwindowmgr.h"
#include "renderwidget/renderglobalinfomgr.h"
#include "shortcut/shortcutdispatcher.h"
RenderApplication::RenderApplication(int& argc, char ** argv)
	: SdApplicationBase(argc, argv)
{
	// 限制创建真窗口数量，避免打开模型内部调用了winId(qt的winId接口默认会把父亲和兄弟窗口变为真窗口)导致
	// 拉伸渲染窗口整个主窗口闪烁的很厉害的问题
	setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
	new ShortcutDispatcher(this);
}
RenderApplication::~RenderApplication()
{
	core::DocumentCenter::instance()->releaseAllDocuments();
}
bool RenderApplication::OnAppInit()
{
	PROFILER_FUNC();
	m_loginManager = new SdLoginManager();
	PubGlobalInstances::setLoginManager(m_loginManager);
	m_dataManager = new DataManager(this);
	PubGlobalInstances::setDataManager(m_dataManager);
	// 创建渲染信息管理全局实例
	RenderGlobalMgrInstance;
	m_localClient = new SdLocalClient(this);
	m_localSocketHandelObject = new LocalSocketHandleObject(this);
	connect(m_localClient, &SdLocalClient::clientConnected, this, &RenderApplication::onLocalClientConnected, Qt::QueuedConnection);
	connect(m_localClient, &SdLocalClient::clientDisconnected, this, &RenderApplication::onLocalClientDisconnected);
	connect(m_localClient, &SdLocalClient::sigInitProgressSuccess, this, &RenderApplication::onInitProgressSuccess);
	connect(m_localClient, &SdLocalClient::sigClearCacheSuccess, this, &RenderApplication::onClearCacheSuccess);
	connect(m_localClient, &SdLocalClient::sigUploadModelSuccess, this, &RenderApplication::onUploadModelSuccess);
	PubGlobalInstances::setLocalClient(m_localClient);
	bool connectState = m_localClient->connectToServer(SERVER_NAME);
	SettingsMgr::getInstance();
	m_localSocketHandelObject->setClientCallback(m_localClient);
	//登陆成功后，取一下网络的渲染配置参数
	RenderVersionManager::instance();
	m_libFilterMgr = new MatLibFilterManager(this);
	GlobalInstances::setMatLibFilterManager(m_libFilterMgr);
	return connectState;
}
void RenderApplication::OnAppExit()
{
	if (m_mainwindowMgr)
		m_mainwindowMgr->sendToExitProcess();
	PubGlobalInstances::setDataManager(nullptr);
	if (m_dataManager)
	{
		m_dataManager->unInit();
		delete m_dataManager;
		m_dataManager = nullptr;
	}
	PubGlobalInstances::setLocalClient(nullptr);
	if (m_localClient)
	{
		m_localClient->deleteLater();
		m_localClient = nullptr;
	}
	PubGlobalInstances::setLoginManager(m_loginManager);
	if (m_loginManager)
	{
		delete m_loginManager;
		m_loginManager = nullptr;
	}
}
void RenderApplication::onLocalClientConnected()
{
	QJsonObject modelJson = QJsonDocument::fromJson(m_appLaunchArg).object();
	int clientId = modelJson["clientId"].toInt();
	// 获取登录信息,给主进程返回当前的父窗口id，用于后续的通信用的
	CheckResult(m_localClient);
	QJsonObject jsonToServer;
	jsonToServer.insert("clientId", clientId);
	jsonToServer.insert("appType", (int)SAT_Render);
	QByteArray retData = m_localClient->syncCall(LSMT_GetUserInfo, jsonToServer);
	// 返回了用户数据后就可以打开模型了
	QJsonObject json = QJsonDocument::fromJson(retData).object();
	if (!json.contains("token"))
	{
		exitProcess();
		return;
	}
	if (RenderGlobalMgrInstance)
		RenderGlobalMgrInstance->init();
	sendProgressVal(clientId, 5, RenderInitState::RIS_Initializing);
	// 初始化本地的数据
	initInfoFromServer(json);
	sendProgressVal(clientId, 15, RenderInitState::RIS_Initializing);
	if (SdLoginManagerInstance)
	{
		SdLoginManagerInstance->setPDMInfo(modelJson["projectId"].toString(), modelJson["materialCatalogId"].toString(), modelJson["styleId"].toString());
	}
	m_mainwindowMgr = new RenderMainWindowMgr();
	m_mainwindowMgr->createMainWindowWidget();
	sendProgressVal(clientId, 80, RenderInitState::RIS_Initializing);
	connect(m_mainwindowMgr, &RenderMainWindowMgr::sigUpdateCompanyId, m_loginManager, &SdLoginManager::onUpdateCompanyId);
	// 主窗口创建完后才开始初始化，保证初始化完的信号能通知到对应的窗口管理类
	if (m_libFilterMgr)
		m_libFilterMgr->init();
	sendProgressVal(clientId, 100, RenderInitState::RIS_Success);
	PROFILER_PROCEDURE_END(ProfilerDefs::appStartup);
}
void RenderApplication::onLocalClientDisconnected()
{
	exitProcess();
}
void RenderApplication::onInitProgressSuccess()
{
	if (m_mainwindowMgr)
		m_mainwindowMgr->showMainWindow();
}
void RenderApplication::onClearCacheSuccess(bool success)
{
	if (m_mainwindowMgr)
		m_mainwindowMgr->clearCacheSuccessCallback(success);
}
void RenderApplication::onUploadModelSuccess()
{
	if (m_mainwindowMgr)
		m_mainwindowMgr->uploadModelSuccessCallback();
}
void RenderApplication::initInfoFromServer(const QJsonObject & json)
{
	CheckResult(m_loginManager);
	m_loginManager->setUserInfo(json["userId"].toString()
		, json["token"].toString()
		, json["email"].toString());
	m_loginManager->setUserHeadImg(json["userImgPath"].toString());
	m_loginManager->setUserName(json["userName"].toString());
	m_loginManager->updateOssInfo();
	m_loginManager->updateMaterialTypeList();
	m_loginManager->getDictionaryTypeWaiting(false);
	m_loginManager->setCompanyInfo(json["companyId"].toString(), json["companyType"].toInt());
	m_loginManager->initStylePlatformCompanyId();

	if (UploadSdk::sdSetCurrUserInfo)
		UploadSdk::sdSetCurrUserInfo(json["userId"].toString().toUtf8().data(),
			json["companyId"].toString().toUtf8().data());
	// 初始化数据库
	if (m_dataManager)
	{
		m_dataManager->init(
			m_loginManager->getUserId(),
			m_loginManager->getToken());
	}
	if (m_threadManager)
	{
		m_threadManager->init();
	}
}
void RenderApplication::sendProgressVal(int clientId, int initProgress, int initState)
{
	QJsonObject jsonToServer;
	jsonToServer.insert("clientId", clientId);
	jsonToServer.insert("appType", (int)SAT_Render);
	jsonToServer.insert("initProgress", initProgress);
	jsonToServer.insert("initState", initState);
	m_localClient->syncCall(LSMT_Render_Init_Progress, jsonToServer);
}
QWidget* RenderApplication::designMainWidget()
{
	if (!m_mainwindowMgr)
		return nullptr;
	QPointer<QWidget> renderMainWindow = m_mainwindowMgr->getMainWindowWidgetAbstract();
	if (renderMainWindow)
		return renderMainWindow.data();
	return nullptr;
}
bool RenderApplication::notify(QObject *obj, QEvent *event)
{
	switch (event->type())
	{
	case QEvent::KeyPress:
		if (ShortcutDispatcher::instance() && ShortcutDispatcher::instance()->process(static_cast<QKeyEvent*>(event)))
			return true;
	}
	return SdApplicationBase::notify(obj, event);
}
