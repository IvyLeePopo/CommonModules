/*
 * @Description: 窗口的工具栏 2
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
#include "edittoolwidget.h"
#include "filetoolwidget.h"
#include "cameratoolwidget.h"
#include "colorwaytoolwidget.h"
#include "helptoolwidget.h"
#include "subrendertoolwidget.h"
#include "teamtoolwidget.h"
#include "usertoolwidget.h"
#include "utilstoolwidget.h"
#include "viewtoolwidget.h"
#include "../renderpopupwidget/designscheme/designschememanager.h"
#include "../renderpopupwidget/aboutus/aboutuswidget.h"
#include "../renderpopupwidget/preference/preferencewidget.h"
#include "common/defs.h"

enum class RenderMainWindowMenuId;
class RenderToolWidgetMgr : public QObject
{
	Q_OBJECT
public:
	explicit RenderToolWidgetMgr(QObject* parent);
	QWidget* createToolMenuBarWidget(QWidget* parent);
	void    clearCacheSuccess(bool success);
	void    updateMainWindowHeight(int height);
signals:
	// 文件
	void sigOpen();
	void sigSave();
	void sigSaveAs();
	void sigExport();
	void sigHistoryVersion();
	// 编辑
	void sigUndo();
	void sigRedo();
	void sigPreferenceSetting();
	// 相机
	void sigSwitchPerspective(int type);    // 正交透视(参数建议用宏)
	void sigSwitchViewType(int type);   // 切换视角 
	// 配色
	void sigGenerateColorway();
	void sigShowColorwayList();
	// 渲染
	void sigRenderMode(int);    // 性能、光追 (参数建议用宏)
	void sigCloudRenderSetting();
	void sigCloudRenderProgress();
	void sigDesignAlbum();
	// 工具
	void sigExplodedView();
	void sigCostEstimate();
	void sigModelQuality(int);  // 模型质量(参数建议用宏)
	void sigLineDraftMaterial();
	void sigRendererType(int type);
	// 团队
	void sigMark();
	void sigMeasure();
	void sigInvitation();
	void sigShare();
	void sigAttachment();
	// 界面
	void sigWindowLayout();
	// 用户
	void sigProfileSettings();
	void sigIdentitySwitching(const QString& companyId);
	void sitExit();
	// 帮助
	void sigOperatingInstructions();
	void sigComment();
	void sigFeedback();
	void sigCheckUpdates();
	void sigAboutUs();
	//关闭
	void sigClose();
public slots:
	void onUpdateToolStatus(int nOrthographic, int nView, int nModelQuality);
private slots:
	void onSendFileToolType(FileToolWidget::ToolType type);
	void onSendEditToolType(EditToolWidget::ToolType type);
	void onSendCameraToolType(CameraToolWidget::ToolType type, int subType);
	void onSendColorwayToolType(ColorwayToolWidget::ToolType type);
	void onSendSubRenderToolType(SubRenderToolWidget::ToolType type);
	void onSendUtilsToolType(UtilsToolWidget::ToolType type, int subType);
	void onSendTeamToolType(TeamToolWidget::ToolType type);
	void onSendViewToolType(ViewToolWidget::ToolType type);
	void onSendUserToolType(UserToolWidget::ToolType type, const QString& companyId);
	void onSendHelpToolType(HelpToolWidget::ToolType type);
private:
	void showPreferenceWidget();
	void showCreateColorwayWidget();
	void showDesignSchemeWidget();
	void showAboutUsWidget();
private:
	FileToolWidget*         m_fileToolWidget = nullptr;
	EditToolWidget*         m_editToolWidget = nullptr;
	PreferenceWidget*       m_preferenceWidget = nullptr;
	CameraToolWidget*       m_cameraToolWidget = nullptr;
	ColorwayToolWidget*     m_colorwayToolWidget = nullptr;
	SubRenderToolWidget*    m_subRenderToolWidget = nullptr;
	UtilsToolWidget*        m_utilsToolWidget = nullptr;
	TeamToolWidget*         m_teamToolWidget = nullptr;
	ViewToolWidget*         m_viewToolWidget = nullptr;
	UserToolWidget*         m_userToolWidget = nullptr;
	HelpToolWidget*         m_helpToolWidget = nullptr;
	AboutUsWidget*          m_aboutUsWidget = nullptr;
	DesignSchemeManager*    m_designSchemeMgr = nullptr;
};
