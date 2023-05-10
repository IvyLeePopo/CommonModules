
#include "pch.h"
#include "projectwidgetmgr.h"
#include "common/dpiscaled.h"
#include "common/colordef.h"
#include "general/genericdef.h"
#include "../tabwidget/verticaltabwidget.h"
#include "componentpanel/commateditpanel.h"
#include "environment/projectenvpanel.h"
#include "environment/projectenvpanelmanager.h"
#include "patterncrafts/projectpatterncraftspanel.h"
#include "patterncrafts/projectpatterncraftsmanager.h"
#include "material/materialeditpanel.h"
#include "material/projectmaterialpanelmgr.h"
#include  "texture/projecttexturepanel.h"
#include "core/documentcenter.h"

namespace
{
	const int g_tabWidgetMaxWidth = 588;
	const int g_tabWidgetMinWidth = 300;
	const char prop_token[] = "prop_token";
}
RenderProjectWidgetMgr::RenderProjectWidgetMgr(QWidget *parentWidget, QObject* parent)
	: QObject(parent)
	, m_parentWigdet(parentWidget)
{

}
QWidget* RenderProjectWidgetMgr::switchToProjectWidget(const QString &id)
{
	if (!m_stackedWiget)
		m_stackedWiget = new QStackedWidget(m_parentWigdet);
	int index = projectIndex(id);
	if (index != -1)
	{
		// already exist, just to set widget visible
		m_stackedWiget->setCurrentIndex(index);
		return m_stackedWiget;
	}
	// no exist, need to create ui component
	VerticalTabWidget *projectTabWidget = new VerticalTabWidget(m_stackedWiget, false);
	projectTabWidget->appendTabWidget(ProjectType::PT_Component, "component", tr("Component"), createComponentPanel(id, projectTabWidget));
	projectTabWidget->appendTabWidget(ProjectType::PT_Material, "material", tr("Material"), createMaterialEditPanel(id, projectTabWidget));
	projectTabWidget->appendTabWidget(ProjectType::PT_Pattern, "pattern", tr("Patterns"), createPatternCraftsEditPanel(id, projectTabWidget));
	projectTabWidget->appendTabWidget(ProjectType::PT_Texture, "texture", tr("Mould texture"), createTextureEditWidget(id, projectTabWidget));
	projectTabWidget->appendTabWidget(ProjectType::PT_Environment, "environment", tr("Environment"), createEnvPanel(id, projectTabWidget));
	projectTabWidget->selectTabWidget(ProjectType::PT_Component, true);
	projectTabWidget->setProperty(prop_token, id);
	int added = m_stackedWiget->addWidget(projectTabWidget);
	m_stackedWiget->setCurrentIndex(added);
	connect(projectTabWidget, &VerticalTabWidget::sigBtnClicked, this, &RenderProjectWidgetMgr::onBtnClicked);
	initConnection();
	return m_stackedWiget;
}
void RenderProjectWidgetMgr::closeProjectWidget(const QString &id)
{
	const int index = projectIndex(id);
	if (index == -1)
		return;
	QWidget *w = m_stackedWiget->widget(index);
	m_stackedWiget->removeWidget(w);
	w->deleteLater();
}
int RenderProjectWidgetMgr::projectIndex(const QString &id)
{
	if (!m_stackedWiget)
		return -1;
	for (int i = 0; i < m_stackedWiget->count(); ++i)
	{
		QWidget *widget = m_stackedWiget->widget(i);
		if (!widget)
			continue;
		const QString strId = widget->property(prop_token).toString();
		if (strId == id)
			return i;
	}

	return -1;
}
void RenderProjectWidgetMgr::getProjectWidgetMaxMinWidth(int& maxWidth, int& minWidth)
{
	maxWidth = _S(g_tabWidgetMaxWidth);
	minWidth = _S(g_tabWidgetMinWidth);
}
void RenderProjectWidgetMgr::switchDisplayOrthoMode(RenderOrthoPersType type)
{
	m_envPanelMgr->switchDisplayOrthoMode(type);
}
int RenderProjectWidgetMgr::shrinkWidget()
{
	if (!m_stackedWiget)
		return 0;
	VerticalTabWidget *projectTabWidget = qobject_cast<VerticalTabWidget*>(m_stackedWiget->currentWidget());
	if (projectTabWidget)
		return projectTabWidget->shrinkWidget();
	return 0;
}
void RenderProjectWidgetMgr::onBtnClicked(int btnId)
{
	if (!m_stackedWiget)
		return;
	VerticalTabWidget *projectTabWidget = qobject_cast<VerticalTabWidget*>(m_stackedWiget->currentWidget());
	if (projectTabWidget)
		projectTabWidget->restoreWidget();
	QString text = "";
	switch (btnId)
	{
	case RenderProjectWidgetMgr::PT_Component:
		text = tr("Component");
		break;
	case RenderProjectWidgetMgr::PT_Material:
		text = tr("Material");
		break;
	case RenderProjectWidgetMgr::PT_Pattern:
		text = tr("Patterns");
		break;
	case RenderProjectWidgetMgr::PT_Texture:
		text = tr("Mould texture");
		break;
	case RenderProjectWidgetMgr::PT_Environment:
		text = tr("Environment");
		break;
	default:
		break;
	}
	emit sigExpand();
	emit sigBtnClicked(text);
}
QWidget* RenderProjectWidgetMgr::createComponentPanel(const QString &id, QWidget* parent)
{
	m_comMatEditPanel = new ComMatEditPanel(id, parent);
	return m_comMatEditPanel;
}
QWidget* RenderProjectWidgetMgr::createEnvPanel(const QString &id, QWidget *parent)
{
	m_envPanelMgr = new ProjectEnvPanelManger(id, parent);
	QWidget* panel = m_envPanelMgr->creatPanel(parent);
	connect(this, &RenderProjectWidgetMgr::sigUpdateProjectEvnData, m_envPanelMgr, &ProjectEnvPanelManger::onSigUpdateProjectEvnData);
	return panel;
}
QWidget* RenderProjectWidgetMgr::createPatternCraftsEditPanel(const QString &id, QWidget* parent)
{
	m_patternCraftsMgr = new ProjectPatternCraftsManager(id, parent);
	QWidget *panel = m_patternCraftsMgr->createPanel(id, parent);
	return panel;
}
QWidget* RenderProjectWidgetMgr::createMaterialEditPanel(const QString &id, QWidget* parent)
{
	m_materialPanelMgr = new ProjectMaterialPanelManager(id, this);
	QWidget* panel = m_materialPanelMgr->createPanel(parent);
	return panel;
}
QWidget* RenderProjectWidgetMgr::createTextureEditWidget(const QString &id, QWidget* parent)
{
	ProjectTexturePanel* texturePanel = new ProjectTexturePanel(id, parent);
	return texturePanel;
}
void RenderProjectWidgetMgr::initConnection()
{
	CheckResult(m_comMatEditPanel);
	CheckResult(m_materialPanelMgr);
	connect(m_comMatEditPanel, &ComMatEditPanel::sigUpdateMatEditPanel, m_materialPanelMgr, &ProjectMaterialPanelManager::onSigUpdateMatEditPanel);

}
