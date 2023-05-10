/*
 * @Description: 窗口右侧 4
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once
#include "core/document.h"
class VerticalTabWidget;
class ProjectMaterialPanelManager;
class ProjectEnvPanelManger;
class ComMatEditPanel;
class ProjectPatternCraftsManager;
class RenderProjectWidgetMgr : public QObject
{
	Q_OBJECT
public:
	enum ProjectType
	{
		PT_Component = 0,
		PT_Material,
		PT_Pattern,
		PT_Texture,
		PT_Environment,
	};
public:
	explicit RenderProjectWidgetMgr(QWidget *parentWidget, QObject* parent);
	QWidget* switchToProjectWidget(const QString &id);
	void closeProjectWidget(const QString &id);
	void getProjectWidgetMaxMinWidth(int& maxWidth, int& minWidth);
	// 相机
	void switchDisplayOrthoMode(RenderOrthoPersType type);
	// 收缩窗口，返回收缩后的宽度
	int shrinkWidget();
signals:
	void sigExpand();
	void sigBtnClicked(const QString &text);
	void sigUpdateProjectEvnData(const QString &envName);
private slots:
	void onBtnClicked(int btnId);
private:
	// factory method
	int projectIndex(const QString &id);
	QWidget* createComponentPanel(const QString &id, QWidget *parent);
	QWidget* createEnvPanel(const QString &id, QWidget *parent);
	QWidget* createPatternCraftsEditPanel(const QString &id, QWidget* parent);
	QWidget* createMaterialEditPanel(const QString &id, QWidget* parent);
	QWidget* createTextureEditWidget(const QString &id, QWidget* parent);
private:
	void initConnection();
private:
	QPointer<QWidget> m_parentWigdet;
	QPointer<QStackedWidget> m_stackedWiget;
	QPointer<ComMatEditPanel> m_comMatEditPanel;
	ProjectMaterialPanelManager* m_materialPanelMgr = nullptr;
	ProjectEnvPanelManger* m_envPanelMgr = nullptr;
	ProjectPatternCraftsManager* m_patternCraftsMgr = nullptr;
};
