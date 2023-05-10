/*
 * @Description: 窗口的左侧 3
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
class VerticalTabWidget;
class MaterialLibPanelMgr;
class MatLibPlatformWebClient;
class ModelLibPanelMgr;
class ModelParams;
class PaletteEditManager;
class EnvLibPanelMgr;
class PatternCraftsLibPanelMgr;
class SdWebViewContainerDialg;
class TextureLibMgr;
class RenderLibraryWidgetMgr : public QObject
{
	Q_OBJECT
public:
	enum LibraryType
	{
		LT_Model = 0,
		LT_Material,
		LT_Color,
		LT_Pattern,
		LT_Texture,
		LT_Environment,
	};
public:
	explicit RenderLibraryWidgetMgr(QObject* parent);
	QWidget*    createLibraryWidget(QWidget* parent);
	void        getLibraryWidgetMaxMinWidth(int& maxWidth, int& minWidth);
	// 收缩窗口，返回收缩后的宽度
	int         shrinkWidget();
	// 材料平台筛选器变化
	void        reactToMatplatformFilterChanged(const QString& filterJson, const QString& recoveryJson);
	void        addWorkspaceLib(const QString& workspaceId);
	void        selectWorkspaceLib(const QString& workspaceId);         // 传入空表示不显示协作空间
	void        refreshModelListCur();
signals:
	void        sigExpand();
	void        sigBtnClicked(const QString& text);
	// 模型
	void        sigOpenModel(const ModelParams& info);
	// 材质
	void        sigMaterialItemLClicked(const QString &materialId);
	// 颜色
	void        sigClrItemLClicked(const QJsonObject& paletteColorInfoObj);
	// 图案
	void        sigPatternItemLClicked(const QString &patternId);
	// 咬花
	void        sigTextureItemLClicked(const QString &textureId);
	// 环境灯光
	void        sigUpdateRenderEnv(const QVariant& envData);
	void        sigUpdateRenderLight(const QList<QVariant>& lightDatas);
	void        sigUpdateProjectEvnData(const QString &envName);
	// 打开完模型
	void        sigOpenModelFinish(bool finish, const QString &modelId);
public:
	void        onSaveModelFinish();
private slots:
	void        onBtnClicked(int btnId);
	void        onShowMatPlatformFilter(const QString& recoveryJson);   // 材料平台展示“更多”过滤条件
private:
	QWidget*    creatModelWidget(QWidget *parent);
	QWidget*    creatMaterialWidget(QWidget *parent);
	QWidget*    createPaletteWidget(QWidget *parent);
	QWidget*    creatEnvironmentWidget(QWidget *parent);
	QWidget*    createPatternLibWidget(QWidget *parent);
	QWidget*    createTextureLibWidget(QWidget *parent);
private:
	VerticalTabWidget*      m_libraryTabWidget = nullptr;
	ModelLibPanelMgr*       m_modelLibMgr = nullptr;
	MaterialLibPanelMgr*    m_materialLibMgr = nullptr;
	EnvLibPanelMgr*         m_envLibMgr = nullptr;
	PaletteEditManager*     m_paletteEditManger = nullptr;
	PatternCraftsLibPanelMgr* m_patternCraftsLibManager = nullptr;
	TextureLibMgr* m_textureLibManager = nullptr;
	// 面料平台过滤条件内嵌web页面
	QPointer<SdWebViewContainerDialg>   m_matPlatformFilterWebViewContainerDialg;
	QPointer<MatLibPlatformWebClient>   m_matPlatformFilterWwebViewClient;
};
