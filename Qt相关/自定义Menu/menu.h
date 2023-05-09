/*
 * @Description: 的用法
 * @Author: Ivy
 * @Date: 2022-05-06 09:34:43
 * @LastEditTime: 2022-05-06 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#include "sdmenulabelitem.h"
class QListWidget;
class NavWidgetTp;
class SdMenu;
class SdMenuListWidget;

class SdMenu : public QMenu
{
	Q_OBJECT
public:
	SdMenu(QWidget *parent = nullptr, bool white = false);
	~SdMenu();
	enum MenuType
	{
		NormalMode,
		ListMode,
		IconMode,
	};
	// 在调用appendMenu前先调用这个函数来调整菜单的模式
	void setMenuType(MenuType menuType);

	// 获取listWidget的原因很可能是为了修改它的样式或是设置固定宽度，
	//让系统自动判断在左还是在右显示，且显示的位置比较合适
	QListWidget *getListWidget();
	//设置菜单当前选中项
	void setCurrentSelItem(int index);
	//设置图标大小
	void setIconSize(const QSize &iconSize);
	//设置一个菜单项的空位
	QWidgetAction *appendSpaceItem();

	//插入一个线条分隔符
	//@color:线条颜色
	//@margin:左右间距，默认为0
	QWidgetAction *appendSeparatorItem(const QColor& color = QColor("#1C1C1C"),
		int margin = 0);

	MenuAction *appendMenu(const QString &text, const QPixmap &icon);
	MenuAction *appendMenu(const QString &text, const QPixmap &icon,
		const QString &shortcutText);
	MenuAction *appendMenu(const QString &text);
	//具体例子参照TypeCombox中增加的menu
	//当前接口中在点击item后，在菜单信号中triggered中会调用onChooseItemTriggered
	//当前方法需进行重构
	MenuAction *appendMenu(const QString &text, bool showCheckedPix);
	//Note:注意插入的Widget的宽度不要超过Menu的宽度，否则会影响二级菜单
	MenuAction *appendMenu(QWidget *itemWidget);
	MenuAction *appendMenuWithCloseBtn(const QString &text, int closeBtnWidth = 38);
	MenuAction *appendMenuWithIconAndChooseState(const QString &text, const QPixmap &icon);
	MenuAction *insertMenu(QAction *action, const QString &text, const QPixmap &icon);
	MenuAction *insertMenuWithIconAndChooseState(QAction *action, const QString &text, const QPixmap &icon);
	//>2D linedraft 2d 配色
	MenuAction *append2DMenu(const QString &text, const bool &isShilt);
	MenuAction *findMenuAction(const QString &text);
	QAction *popup(const QPoint &point);
	void setWidth(int menuWidth);
	int getWidth();
	void setItemHeight(int itemHeight);
	void showTopBottomSpace(bool showSpace);
	void setFont(QFont font);
	void clearData();
	void setHoverColor(const QString &hoverBkCol, const QString &hoverTextCol);
	void setNormalColor(const QString &norTextColor, const QString &normalBgColor);
	void updateListItemIcon(int index, const QPixmap &icon);
	MenuAction *findActionByText(const QString &text);
	MenuAction *findActionByIndex(int index);
	//仅用于带choose状态的item
	void setChooseItem(MenuAction *action);
	void clearAllSelected();
	// 设置列表图标菜单的子项的大小
	void setIconListItemSize(const QSize &size);
	QSize getIconListItemSize();
	int getIconListSpacing();
	void setMulitSelect(bool mulitSelect);
	void enableMenuItem(QAction *action, bool enable);
	void deleteAction(const QString &actionText);
	int getShadowWidth();
	int getItemCount();
	// 这个函数作用有限，慎用
	void setItemSelectedColor(const QString& color);
signals:
	// 这个信号是给checkbox类型的菜单用的
	void stateChanged(bool checked, QAction *action);
	void closeBtnClicked(QAction *action);
	void triggered(QAction *action);
	void triggeredByListWidget(int index);
	void triggeredByListWidget(QString text);
	void sigSubMenuHide();
public:
	virtual void setFixedWidth(int width);
	virtual void setMaximumHeight(int height);
private:
	void initIconListWidget();
	void initListWidget();
	MenuAction *addItemToListWidget(const QString &text, const QPixmap &icon);
	MenuAction *addItemToListWidget(const QString &text);
protected:
	virtual void paintEvent(QPaintEvent *event) override;
	virtual bool eventFilter(QObject *watched, QEvent *event) override;
	virtual void showEvent(QShowEvent *event) override;
private slots:
	void onTriggeredByListWidget(int index);
	//用于接收带choose状态的item的点击事件
	void onChooseItemTriggered(MenuAction *action);
	void onItemEnterOrLeave(MenuLabelItem *menuItem, bool enter);
private:
	void initUI();
	void adjustMenuSize();
	void handleListWidgetMouseMove(QPoint mousePos);
private:
	QFont       m_font;

	//菜单项中整个宽度和长度，不包括阴影部分
	int         m_width;
	int         m_height;

	int         m_itemHeight;
	MenuType    m_menuType;
	QAction     *m_firstSpaceItem;
	QAction     *m_lastSpaceItem;
	QList<MenuAction *> m_actionList;
	//item中的icon大小（注：是普通dpi的大小）
	QSize       m_iconSize;
	// 图标菜单样式下的action,下面变量都是图标菜单使用的
	MenuAction  *m_iconModeAction;
	SdMenuListWidget *m_listWidget;
	QList<MenuAction *> m_listItemAction;
	QSize       m_iconListItemSize;
	int         m_itemSpacing;
	// 子项的颜色样式
	QString     m_itemHoverClr;
	QString     m_itemHoverTextClr;
	// 标记checked状态是否可以多选
	bool        m_mulitSelect;
	//标记是否已经添加了阴影的高度值
	bool        m_hasResizeShadow;
	// menu子项过长的提示
	NavWidgetTp *m_menuTips;
	bool        m_whiteStyle;
	QString     m_itemSeletedColor;
	// 自定义tooltip提示只在普通模式下支持
	QSharedPointer<NavWidgetTp> m_itemTips;
};
#ifdef _TEST_
class ItemWidgetTest : public QWidget
{
	Q_OBJECT
public:
	ItemWidgetTest(int width, int height, QWidget* parent = NULL);
	~ItemWidgetTest();
};
class MenuWidgetTest : public QPushButton
{
	Q_OBJECT
public:
	MenuWidgetTest(QWidget* parent = NULL);
	~MenuWidgetTest();
};