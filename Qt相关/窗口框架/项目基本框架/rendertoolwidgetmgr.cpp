
#include "pch.h"
#include "rendertoolwidgetmgr.h"
#include "common/dpiscaled.h"
#include "common/colordef.h"
#include "general/genericdef.h"
#include "mainwindowmenubar.h"
#include "ui/button/textbutton.h"
#include "ui/button/imagebutton.h"

RenderToolWidgetMgr::RenderToolWidgetMgr(QObject* parent)
	: QObject(parent)
{
}

QWidget* RenderToolWidgetMgr::createToolMenuBarWidget(QWidget* parent)
{
	RenderMainWindowMenuBar* toolMenuBar = new RenderMainWindowMenuBar(parent);
	m_fileToolWidget = new FileToolWidget(toolMenuBar);
	m_editToolWidget = new EditToolWidget(toolMenuBar);
	m_cameraToolWidget = new CameraToolWidget(toolMenuBar);
	m_colorwayToolWidget = new ColorwayToolWidget(toolMenuBar);
	m_subRenderToolWidget = new SubRenderToolWidget(toolMenuBar);
	m_utilsToolWidget = new UtilsToolWidget(toolMenuBar);
	m_teamToolWidget = new TeamToolWidget(toolMenuBar);
	m_viewToolWidget = new ViewToolWidget(toolMenuBar);
	m_userToolWidget = new UserToolWidget(toolMenuBar);
	m_helpToolWidget = new HelpToolWidget(toolMenuBar);
	using MenuId = RenderMainWindowMenuId;
	toolMenuBar->appendMainMenu(MenuId::File, "File(&F)", tr("File"), "F", m_fileToolWidget);
	toolMenuBar->appendMainMenu(MenuId::Edit, "Edit(&E)", tr("Edit"), "E", m_editToolWidget);
	toolMenuBar->appendMainMenu(MenuId::Camera, "Camera(&R)", tr("Camera"), "R", m_cameraToolWidget);
	toolMenuBar->appendMainMenu(MenuId::Colorway, "Colorway(&U)", tr("Colorway"), "U", m_colorwayToolWidget);
	toolMenuBar->appendMainMenu(MenuId::Render, "Render(&I)", tr("Render"), "I", m_subRenderToolWidget);
	toolMenuBar->appendMainMenu(MenuId::Tool, "Tool(&O)", tr("Tool"), "O", m_utilsToolWidget);
	toolMenuBar->appendMainMenu(MenuId::Team, "Team(&T)", tr("Team"), "T", m_teamToolWidget);
	toolMenuBar->appendMainMenu(MenuId::View, "Interface(&G)", tr("Interface"), "G", m_viewToolWidget);
	toolMenuBar->appendMainMenu(MenuId::User, "User(&K)", tr("User"), "K", m_userToolWidget);
	toolMenuBar->appendMainMenu(MenuId::Help, "Help(&H)", tr("Help"), "H", m_helpToolWidget);
	toolMenuBar->selectMainMenu(MenuId::File, true);
	connect(toolMenuBar, &RenderMainWindowMenuBar::sigClose, this, &RenderToolWidgetMgr::sigClose);
	connect(m_fileToolWidget, &FileToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendFileToolType);
	connect(m_editToolWidget, &EditToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendEditToolType);
	connect(m_cameraToolWidget, &CameraToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendCameraToolType);
	connect(m_colorwayToolWidget, &ColorwayToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendColorwayToolType);
	connect(m_subRenderToolWidget, &SubRenderToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendSubRenderToolType);
	connect(m_utilsToolWidget, &UtilsToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendUtilsToolType);
	connect(m_teamToolWidget, &TeamToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendTeamToolType);
	connect(m_viewToolWidget, &ViewToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendViewToolType);
	connect(m_userToolWidget, &UserToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendUserToolType);
	connect(m_helpToolWidget, &HelpToolWidget::sigSendToolType, this, &RenderToolWidgetMgr::onSendHelpToolType);
	return toolMenuBar;
}
void RenderToolWidgetMgr::clearCacheSuccess(bool success)
{
	if (m_preferenceWidget)
	{
		m_preferenceWidget->clearCacheFinished(success);
	}
}
void RenderToolWidgetMgr::updateMainWindowHeight(int height)
{
	if (m_userToolWidget)
	{
		m_userToolWidget->setMainWindowHeight(height);
	}
}
void RenderToolWidgetMgr::onUpdateToolStatus(int nOrthographic, int nView, int nModelQuality)
{
	if (m_cameraToolWidget)
	{
		m_cameraToolWidget->refreshUI(nOrthographic, nView);
	}
	if (m_utilsToolWidget)
	{
		m_utilsToolWidget->refreshUI(nModelQuality);
	}
}
void RenderToolWidgetMgr::onSendFileToolType(FileToolWidget::ToolType type)
{
	using ToolType = FileToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_Open:
		emit sigOpen();
		break;
	case ToolType::TY_Save:
		emit sigSave();
		break;
	case ToolType::TY_SaveAs:
		emit sigSaveAs();
		break;
	case ToolType::TY_Export:
		emit sigExport();
		break;
	case ToolType::TY_HistoryVersion:
		emit sigHistoryVersion();
		break;
	default:
		break;
	}
}
void RenderToolWidgetMgr::onSendEditToolType(EditToolWidget::ToolType type)
{
	using ToolType = EditToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_Undo:
		emit sigUndo();
		break;
	case ToolType::TY_Redo:
		emit sigRedo();
		break;
	case ToolType::TY_PreferenceSetting:
		showPreferenceWidget();
		emit sigPreferenceSetting();
		break;
	default:
		break;
	}
}
void RenderToolWidgetMgr::onSendCameraToolType(CameraToolWidget::ToolType type, int subType)
{
	using ToolType = CameraToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_SwitchViewType:
		emit sigSwitchViewType(subType);
		break;
	case ToolType::TY_SwitchPerspective:
		emit sigSwitchPerspective(subType);
		break;
	default:
		break;
	}
}
void RenderToolWidgetMgr::onSendColorwayToolType(ColorwayToolWidget::ToolType type)
{
	using ToolType = ColorwayToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_GenerateColorway:
		showCreateColorwayWidget();
		emit sigGenerateColorway();
		break;
	case ToolType::TY_DesignScheme:
		showDesignSchemeWidget();
		emit sigShowColorwayList();
		break;
	default:
		break;
	}
}
void RenderToolWidgetMgr::onSendSubRenderToolType(SubRenderToolWidget::ToolType type)
{
	using ToolType = SubRenderToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_RenderMode:
		emit sigRenderMode(0);
		break;
	case ToolType::TY_CloudRenderSetting:
		emit sigCloudRenderSetting();
		break;
	case ToolType::TY_CloudRenderProgress:
		emit sigCloudRenderProgress();
		break;
	case ToolType::TY_DesignAlbum:
		emit sigDesignAlbum();
		break;
	default:
		break;
	}
}
void RenderToolWidgetMgr::onSendUtilsToolType(UtilsToolWidget::ToolType type, int subType)
{
	using ToolType = UtilsToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_ExplodedView:
		emit sigExplodedView();
		break;
	case ToolType::TY_CostEstimate:
		emit sigCostEstimate();
		break;
	case ToolType::TY_ModelQuality:
		emit sigModelQuality(subType);
		break;
	case ToolType::TY_LineDraftMaterial:
		emit sigLineDraftMaterial();
		break;
	case ToolType::TY_RendererType:
		emit sigRendererType(subType);
		break;
	default:
		break;
	}
}
void RenderToolWidgetMgr::onSendTeamToolType(TeamToolWidget::ToolType type)
{
	using ToolType = TeamToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_Mark:
		emit sigMark();
		break;
	case ToolType::TY_Measure:
		emit sigMeasure();
		break;
	case ToolType::TY_Invitation:
		emit sigInvitation();
		break;
	case ToolType::TY_Share:
		emit sigShare();
		break;
	case ToolType::TY_Attachment:
		emit sigAttachment();
		break;
	default:
		break;
	}
}
void RenderToolWidgetMgr::onSendViewToolType(ViewToolWidget::ToolType type)
{
	using ToolType = ViewToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_WindowLayout:
		emit sigWindowLayout();
		break;
	default:
		break;
	}
}

void RenderToolWidgetMgr::onSendUserToolType(UserToolWidget::ToolType type, const QString& companyId)
{
	using ToolType = UserToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_ProfileSettings:
		emit sigProfileSettings();
		break;
	case ToolType::TY_IdentitySwitching:
		emit sigIdentitySwitching(companyId);
		break;
	case ToolType::TY_Exit:
		emit sitExit();
		break;
	default:
		break;
	}
}
void RenderToolWidgetMgr::onSendHelpToolType(HelpToolWidget::ToolType type)
{
	using ToolType = HelpToolWidget::ToolType;
	switch (type)
	{
	case ToolType::TY_OperatingInstructions:
		emit sigOperatingInstructions();
		break;
	case ToolType::TY_Comment:
		emit sigComment();
		break;
	case ToolType::TY_Feedback:
		emit sigFeedback();
		break;
	case ToolType::TY_CheckUpdates:
		emit sigCheckUpdates();
		break;
	case ToolType::TY_AboutUs:
		showAboutUsWidget();
		emit sigAboutUs();
		break;
	default:
		break;
	}
}
void RenderToolWidgetMgr::showPreferenceWidget()
{
	if (!m_preferenceWidget)
		m_preferenceWidget = new PreferenceWidget();
	m_preferenceWidget->show();
}
void RenderToolWidgetMgr::showCreateColorwayWidget()
{
	if (!m_designSchemeMgr)
		m_designSchemeMgr = new DesignSchemeManager(this);
	m_designSchemeMgr->showCreateSchemeWidget();
}
void RenderToolWidgetMgr::showDesignSchemeWidget()
{
	if (!m_designSchemeMgr)
		m_designSchemeMgr = new DesignSchemeManager(this);
	m_designSchemeMgr->showDesignSchemeWidget();
}
void RenderToolWidgetMgr::showAboutUsWidget()
{
	if (!m_aboutUsWidget)
	{
		m_aboutUsWidget = new AboutUsWidget();
	}
	m_aboutUsWidget->show();
}
