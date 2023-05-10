
#include "pch.h"
#include "librarywidgetmgr.h"
#include "application/applicationbase.h"
#include "application/pubglobalinstances.h"
#include "common/dpiscaled.h"
#include "common/colordef.h"
#include "general/genericdef.h"
#include "localsocket/localsocketdefs.h"
#include "localsocket/sdlocalclient.h"
#include "materialpanel/matlibmatplatformwebclient.h"
#include "materialpanel/matlibpanelmgr.h"
#include "environmentpanel/envlibpanelmgr.h"
#include "../tabwidget/verticaltabwidget.h"
#include "../tabwidget/horizontaltabwidget.h"
#include "modelpanel/modellibpanelmgr.h"
#include "renderwidget/librarypanel/palettepanel/paletteeditmgr.h"
#include "patterncraftspanel/patterncraftslibpanelmgr.h"
#include "texturepanel/texturelibmgr.h"
#include "urlmanager/urldef.h"
namespace
{
	const int g_tabWidgetMinWidth = 300;
	const int g_tabWidgetMaxWidth = 588;
}
RenderLibraryWidgetMgr::RenderLibraryWidgetMgr(QObject* parent)
	: QObject(parent)
{
}
QWidget* RenderLibraryWidgetMgr::createLibraryWidget(QWidget* parent)
{
	m_libraryTabWidget = new VerticalTabWidget(parent);
	m_libraryTabWidget->appendTabWidget(LibraryType::LT_Model, "model", tr("Model"), creatModelWidget(m_libraryTabWidget));
	m_libraryTabWidget->appendTabWidget(LibraryType::LT_Material, "materials", tr("Materials"), creatMaterialWidget(m_libraryTabWidget));
	m_libraryTabWidget->appendTabWidget(LibraryType::LT_Color, "color", tr("Colors"), createPaletteWidget(m_libraryTabWidget));
	m_libraryTabWidget->appendTabWidget(LibraryType::LT_Pattern, "pattern", tr("Patterns"), createPatternLibWidget(m_libraryTabWidget));
	m_libraryTabWidget->appendTabWidget(LibraryType::LT_Texture, "texture", tr("Mould texture"), createTextureLibWidget(m_libraryTabWidget));
	m_libraryTabWidget->appendTabWidget(LibraryType::LT_Environment, "environment", tr("Environment"), creatEnvironmentWidget(m_libraryTabWidget));
	m_libraryTabWidget->selectTabWidget(LibraryType::LT_Model, true);
	connect(m_libraryTabWidget, &VerticalTabWidget::sigBtnClicked, this, &RenderLibraryWidgetMgr::onBtnClicked);
	return m_libraryTabWidget;
}
void RenderLibraryWidgetMgr::getLibraryWidgetMaxMinWidth(int& maxWidth, int& minWidth)
{
	maxWidth = _S(g_tabWidgetMaxWidth);
	minWidth = _S(g_tabWidgetMinWidth);
}
QWidget* RenderLibraryWidgetMgr::creatModelWidget(QWidget *parent)
{
	if (!m_modelLibMgr)
	{
		m_modelLibMgr = new ModelLibPanelMgr(this);
		connect(m_modelLibMgr, &ModelLibPanelMgr::sigOpenModel, this, &RenderLibraryWidgetMgr::sigOpenModel);
	}
	return m_modelLibMgr->getModelPanelLibWidget(parent);
}
QWidget* RenderLibraryWidgetMgr::creatMaterialWidget(QWidget *parent)
{
	if (!m_materialLibMgr)
	{
		m_materialLibMgr = new MaterialLibPanelMgr(this);
		connect(m_materialLibMgr, &MaterialLibPanelMgr::sigShowMatPlatformFilter, this, &RenderLibraryWidgetMgr::onShowMatPlatformFilter);
		connect(m_materialLibMgr, &MaterialLibPanelMgr::sigMaterialItemLClicked, this, &RenderLibraryWidgetMgr::sigMaterialItemLClicked);
	}
	return m_materialLibMgr->createMaterialLibWidget(parent);
}

QWidget* RenderLibraryWidgetMgr::creatEnvironmentWidget(QWidget *parent)
{
	if (!m_envLibMgr)
	{
		m_envLibMgr = new EnvLibPanelMgr(this);
		connect(m_envLibMgr, &EnvLibPanelMgr::sigUpdateRenderEnv, this, &RenderLibraryWidgetMgr::sigUpdateRenderEnv);
		connect(m_envLibMgr, &EnvLibPanelMgr::sigUpdateRenderLight, this, &RenderLibraryWidgetMgr::sigUpdateRenderLight);
		connect(m_envLibMgr, &EnvLibPanelMgr::sigUpdateProjectEvnData, this, &RenderLibraryWidgetMgr::sigUpdateProjectEvnData);
		connect(this, &RenderLibraryWidgetMgr::sigOpenModelFinish, m_envLibMgr, &EnvLibPanelMgr::onSigOpenModelFinish);
	}
	return m_envLibMgr->createEnvLibWidget(parent);
}
QWidget* RenderLibraryWidgetMgr::createPatternLibWidget(QWidget *parent)
{
	if (!m_patternCraftsLibManager)
	{
		m_patternCraftsLibManager = new PatternCraftsLibPanelMgr(this);
		connect(m_patternCraftsLibManager, &PatternCraftsLibPanelMgr::sigPatternItemLClicked,
			this, &RenderLibraryWidgetMgr::sigPatternItemLClicked);
	}
	return m_patternCraftsLibManager->createPatternCraftsLibPanel(parent);
}
QWidget* RenderLibraryWidgetMgr::createTextureLibWidget(QWidget *parent)
{
	if (!m_textureLibManager)
	{
		m_textureLibManager = new TextureLibMgr(this);
		connect(m_textureLibManager, &TextureLibMgr::sigTextureItemLClicked, this, &RenderLibraryWidgetMgr::sigTextureItemLClicked);
	}
	return m_textureLibManager->getTextureLibPanel(parent);
}
QWidget* RenderLibraryWidgetMgr::createPaletteWidget(QWidget *parent)
{
	if (!m_paletteEditManger)
	{
		m_paletteEditManger = new PaletteEditManager(this);
		connect(m_paletteEditManger, &PaletteEditManager::sigColorChange, this, &RenderLibraryWidgetMgr::sigClrItemLClicked);
	}
	return m_paletteEditManger->getPaletteWidget(parent);
}
int RenderLibraryWidgetMgr::shrinkWidget()
{
	if (m_libraryTabWidget)
		return m_libraryTabWidget->shrinkWidget();
	return 0;
}
void RenderLibraryWidgetMgr::onBtnClicked(int btnId)
{
	if (m_libraryTabWidget)
		m_libraryTabWidget->restoreWidget();
	QString text = "";
	switch (btnId)
	{
	case LibraryType::LT_Model:
		text = tr("Model");
		break;
	case LibraryType::LT_Material:
		text = tr("Materials");
		break;
	case LibraryType::LT_Color:
		text = tr("Colors");
		m_paletteEditManger->getPaletteWidget(nullptr)->setVisible(true);
		break;
	case LibraryType::LT_Pattern:
		text = tr("Patterns");
		break;
	case LibraryType::LT_Texture:
		text = tr("Mould texture");
		break;
	case LibraryType::LT_Environment:
		//m_envLibMgr->initData();
		text = tr("Environment");
		break;
	default:
		break;
	}
	emit sigExpand();
	emit sigBtnClicked(text);
}
void RenderLibraryWidgetMgr::onShowMatPlatformFilter(const QString& recoveryJson)
{
	QTimer::singleShot(0, this, [=] {
		CheckResult(SdApplicationInstance);
		QWidget *designMainWidget = SdApplicationInstance->designMainWidget();
		CheckResult(designMainWidget);
		QString url = urldef::getMatPlatformFilterUrl(recoveryJson);
		if (!m_matPlatformFilterWebViewContainerDialg)
			m_matPlatformFilterWebViewContainerDialg = new SdWebViewContainerDialg(designMainWidget);
		if (!m_matPlatformFilterWwebViewClient)
		{
			m_matPlatformFilterWwebViewClient = new MatLibPlatformWebClient(url, this, WebViewDef::WVT_MatPlatformFilter, m_matPlatformFilterWebViewContainerDialg);
			connect(m_matPlatformFilterWwebViewClient, &MatLibPlatformWebClient::sigFilterChanged, this, [=](const QString& filterJson, const QString& recoveryJson) {
				reactToMatplatformFilterChanged(filterJson, recoveryJson);
			});
			connect(m_matPlatformFilterWwebViewClient, &SdWebViewClient::sigViewClose, this, [=](QString params) {
				if (m_matPlatformFilterWebViewContainerDialg)
					m_matPlatformFilterWebViewContainerDialg->close();
			});
		}
		else
		{
			m_matPlatformFilterWwebViewClient->load(url);
		}
		m_matPlatformFilterWebViewContainerDialg->setGeometry(designMainWidget->geometry());
		m_matPlatformFilterWebViewContainerDialg->exec();
	});
}
void RenderLibraryWidgetMgr::reactToMatplatformFilterChanged(const QString& filterJson, const QString& recoveryJson)
{
	if (m_materialLibMgr)
		m_materialLibMgr->reactToMatplatformFilterChanged(filterJson, recoveryJson);
}
void RenderLibraryWidgetMgr::addWorkspaceLib(const QString& workspaceId)
{
	if (m_materialLibMgr)
		m_materialLibMgr->addWorkspaceLib(workspaceId);
}
void RenderLibraryWidgetMgr::selectWorkspaceLib(const QString& workspaceId)
{
	if (m_materialLibMgr)
		m_materialLibMgr->selectWorkspaceLib(workspaceId);
}
void RenderLibraryWidgetMgr::refreshModelListCur()
{
	CheckResult(m_modelLibMgr);
	m_modelLibMgr->refreshModelListCur();
}
void RenderLibraryWidgetMgr::onSaveModelFinish()
{
	refreshModelListCur();
}
