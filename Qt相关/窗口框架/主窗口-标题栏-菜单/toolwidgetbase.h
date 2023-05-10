/*
 * @Description: 主窗口-标题栏-菜单
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
#include <QPointer>
#include <QHash>
class QPushButton;
class TextButton;
class ImageButton;

enum class RenderMainWindowMenuId
{
	File = 0,
	Edit,
	Camera,
	Colorway,
	Render,
	Tool,
	Team,
	View,
	User,
	Help,
};
class RenderMainWindowMenuBar : public QWidget
{
	Q_OBJECT
		using Base = QWidget;
public:
	enum WindowBtn
	{
		WB_None = 0x00,
		WB_Min = 0x01,
		WB_Max_Restore = 0x02,
		WB_Close = 0x04,
		WB_ALL = 0xFFFFFFFF
	};
public:
	explicit RenderMainWindowMenuBar(QPointer<QWidget> mainwindow);
	void appendMainMenu(RenderMainWindowMenuId id, const QString& text, const QString& keyText, const QString& key, QWidget* toolWidget);
	void selectMainMenu(RenderMainWindowMenuId id, bool btnNeedChecked = false);
	void setWindowBtns(WindowBtn windowBtns);
public:
	QSize sizeHint() const final;
	QSize minimumSizeHint() const final;

signals:
	void sigClose();
private Q_SLOTS:
	void onButtonClicked(int btnId);
private:
	void paintEvent(QPaintEvent* event) final;
	void mousePressEvent(QMouseEvent* event) final;
	void mouseMoveEvent(QMouseEvent* event) final;
	void mouseReleaseEvent(QMouseEvent* event) final;
	void mouseDoubleClickEvent(QMouseEvent* event) final;
private:
	void initUi();
	QHBoxLayout* createMenuBarLayout();
	enum class MainWindowSize
	{
		Normal,
		Min,
		Max
	};
	void showMainWindow(MainWindowSize size);
	void switchMainWindow();
	void closeMainWindow();
private:
	struct MenuIdAndPtr
	{
		RenderMainWindowMenuId  id;
		QPointer<TextButton>    menuBtn;
		QPointer<QWidget>       toolWidget;
	};
	QPointer<QWidget>       m_mainwindow;
	QPointer<QPushButton>   m_btnMin;
	QPointer<QPushButton>   m_btnMaxRestore;
	QPointer<QPushButton>   m_btnClose;
	QMap<RenderMainWindowMenuId, MenuIdAndPtr> m_mapMenuData;
	QHBoxLayout             *m_menuLayout;
	QButtonGroup            *m_btnGroup;
	QStackedWidget          *m_toolStackedWidget;
	bool                    m_dragging = false;
	QPoint                  m_dragStartPoint;
};