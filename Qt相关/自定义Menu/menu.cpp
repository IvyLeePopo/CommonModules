
#include "pch.h"
#include "menu.h"
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QScrollBar>
#include <QToolTip>
#include <QWidgetAction>
#include "common/dpiscaled.h"
#include "common/sdimageresource.h"
#include "common/sdstylesheet.h"
#include "general/autoprocessdef.h"
#include "general/genericdef.h"
#include "general/genericfunc.h"
#include "ui/navwidgettips.h"
#include "ui/shadowborderfunc.h"
#include "ui/tooltip/sdtooltip.h"
#include "ui/tooltip/tooltip.h"
// 菜单顶部空间处高度
#define SPACE_HEIGHT        4
#define DEFAULT_WIDTH       180
#define ITEM_HEIGHT         32
#define ICON_HEIGHT         12
#define BORDER_RADIUS       2
#define LIST_ITEM_BORDER    2
#define ICON_ITEM_WIDTH     110
#define ITEM_TEXT_HEIGHT    26
#define ITEM_HOVER_STYLE    QString("QLabel#ItemLabel{background-color:%0;color:%1}")
#define ITEM_NORMAL_STYLE   QString("QLabel#ItemLabel{background-color:#FFFFFF;color: #333333;}")
#define ITEM_DISABLE_STYLE  QString("QLabel#ItemLabel{background-color:#FFFFFF;color: #BBBBBB;}")
#define MENU_BACKGROUND_CLR         "#2C2C2C"
#define MENU_BACKGROUND_CLR_WHITE   "#FFFFFF"
#define FONT_SIZE           9
#define DEFAULT_FONT  QFont("Microsoft YaHei", FONT_SIZE)
#define CLR_SHORTCUT_TEXT   "#666666"
#define MENU_SHADOW_WIDTH   5

SdMenu::SdMenu(QWidget *parent, bool white)
	: QMenu(parent)
	, m_font("Microsoft YaHei", 10)
	, m_width(_S(DEFAULT_WIDTH))
	, m_height(_S(0))
	, m_itemHeight(_S(ITEM_HEIGHT))
	, m_firstSpaceItem(nullptr)
	, m_lastSpaceItem(nullptr)
	, m_menuType(NormalMode)
	, m_iconModeAction(nullptr)
	, m_listWidget(nullptr)
	, m_iconListItemSize(_SS(ICON_ITEM_WIDTH, ICON_ITEM_WIDTH / 2 + ITEM_TEXT_HEIGHT))
	, m_itemSpacing(_S(11))
	, m_itemHoverClr("#3A3A3A")
	, m_itemHoverTextClr("#CCCCCC")
	, m_mulitSelect(nullptr)
	, m_iconSize(16, 16)
	, m_menuTips(nullptr)
	, m_whiteStyle(white)
{
	initUI();
	installEventFilter(this);
}
SdMenu::~SdMenu()
{
	m_actionList.clear();
}
void SdMenu::initUI()
{
	setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setStyleSheet(QString("QMenu{border-radius: %1px;}").arg(BORDER_RADIUS)
		+ QString("QMenu::item{border-radius:%1px;min-height:%2px;}").arg(BORDER_RADIUS).arg(m_itemHeight));
	m_firstSpaceItem = appendSpaceItem();
	m_lastSpaceItem = appendSpaceItem();
	this->setContentsMargins(MENU_SHADOW_WIDTH, MENU_SHADOW_WIDTH, MENU_SHADOW_WIDTH, MENU_SHADOW_WIDTH);
}
void SdMenu::adjustMenuSize()
{
	int spaceHeight = SPACE_HEIGHT;
	if (m_menuType == NormalMode)
	{
		this->setFixedSize(QSize(m_width + _S(MENU_SHADOW_WIDTH) * 2, m_height + _S(MENU_SHADOW_WIDTH) * 2 + _S(spaceHeight)));
	}
	else if (m_menuType == ListMode)
	{
		if (m_whiteStyle)
			spaceHeight = 0;
		int menuHeight = 0;
		if (m_listWidget->count() * m_itemHeight > this->maximumHeight() - _S(MENU_SHADOW_WIDTH) * 2)
			menuHeight = this->maximumHeight() - _S(MENU_SHADOW_WIDTH) * 2 - _S(spaceHeight) * 2;
		else
			menuHeight = m_listWidget->count() * m_itemHeight;
		m_listWidget->setFixedHeight(menuHeight);
		m_listWidget->setFixedWidth(m_width);
		this->resize(QSize(m_width + _S(MENU_SHADOW_WIDTH) * 2, menuHeight + _S(MENU_SHADOW_WIDTH) * 2 + _S(spaceHeight) * 2));
	}
}
void SdMenu::handleListWidgetMouseMove(QPoint globalPos)
{
	if (m_menuType == NormalMode)
		return;
	if (!m_listWidget)
	{
		if (m_menuTips)
			m_menuTips->hide();
		return;
	}
	QPoint widgetPos = m_listWidget->mapFromGlobal(globalPos);
	QListWidgetItem *itemWidget = m_listWidget->itemAt(widgetPos);
	if (!itemWidget)
	{
		if (m_menuTips)
			m_menuTips->hide();
		return;
	}
	if (!m_menuTips)
	{
		m_menuTips = new NavWidgetTp(NULL, "bug");
	}

	int textWidth = SdStyleFunc::getTextWidth(m_font, itemWidget->text());
	if (textWidth > m_width)
	{
		QRect rc = m_listWidget->visualItemRect(itemWidget);
		QPoint pos = mapToGlobal(rc.topLeft());
		rc.setX(pos.x());
		rc.setY(pos.y());
		rc.setHeight(m_itemHeight);
		rc.setWidth(m_width);
		m_menuTips->setTips(itemWidget->text());
		m_menuTips->placeTip(EmTpPosition::TpBottom, _S(8), rc);
		m_menuTips->show();
	}
	else
	{
		if (m_menuTips)
			m_menuTips->hide();
	}
}
QWidgetAction *SdMenu::appendSpaceItem()
{
	QWidgetAction *action = new QWidgetAction(this);
	QLabel *spaceLabel = new QLabel();

	if (m_whiteStyle)
	{
		spaceLabel->hide();
	}
	else
	{
		spaceLabel->setFixedHeight(_S(SPACE_HEIGHT));
		spaceLabel->setStyleSheet(QString("QLabel{background-color:%2;border-radius: %1px;}").arg(BORDER_RADIUS).arg(MENU_BACKGROUND_CLR));
		m_height += spaceLabel->height();
	}
	action->setDefaultWidget(spaceLabel);
	//action->setSeparator(true);
	addAction(action);
	return action;
}
void SdMenu::setMenuType(MenuType menuType)
{
	m_menuType = menuType;
	CheckResult(!m_listWidget);
	switch (menuType)
	{
	case IconMode:
	{
		initIconListWidget();
	}
	break;
	case ListMode:
	{
		initListWidget();
	}
	break;
	}
}
QListWidget *SdMenu::getListWidget()
{
	return m_listWidget;
}
void SdMenu::setCurrentSelItem(int index)
{
	if (!m_listWidget)
		return;
	if (index < 0 || index >= m_listWidget->count())
		return;
	AutoBlockSignal autoBlock(m_listWidget);
	m_listWidget->setItemSelected(m_listWidget->item(index), true);
}
void SdMenu::setIconSize(const QSize &iconSize)
{
	m_iconSize = iconSize;
}
QWidgetAction *SdMenu::appendSeparatorItem(const QColor& color, int margin)
{
	QWidgetAction *action = new QWidgetAction(this);
	MenuSeparator *SeparatorLabel = new MenuSeparator();
	SeparatorLabel->setFixedHeight(_S(SPACE_HEIGHT) * 2);
	SeparatorLabel->setSeparatorColor(color);
	SeparatorLabel->setSeparatorMargin(_S(margin));
	SeparatorLabel->setStyleSheet("background-color:transparent;");
	action->setDefaultWidget(SeparatorLabel);
	insertAction(m_lastSpaceItem, action);
	m_height += SeparatorLabel->height();
	return action;
}
MenuAction *SdMenu::appendMenu(const QString &text, const QPixmap &icon)
{
	// 处理列表图标模式的菜单
	if (m_menuType == IconMode)
	{
		if (!m_iconModeAction)
		{
			m_iconModeAction = new MenuAction(this);
			m_iconModeAction->setDefaultWidget(m_listWidget);
			m_iconModeAction->setText(text);
			insertAction(m_lastSpaceItem, m_iconModeAction);
		}
		addItemToListWidget(text, icon);
		return m_iconModeAction;
	}

	MenuAction *action = new MenuAction(this);
	// QLabel创建时要指定父窗口指针，不然会导致stylesheet中的padding-left等失效
	MenuLabelItem *itemLabel = new MenuLabelItem(text, this, icon, "", m_iconSize);
	itemLabel->setFixedWidth(m_width);
	itemLabel->setFixedHeight(m_itemHeight);
	itemLabel->setFocusPolicy(Qt::NoFocus);
	action->setDefaultWidget(itemLabel);
	action->setText(text);
	//addAction(action);
	insertAction(m_lastSpaceItem, action);
	connect(action, &QAction::triggered, this, [=]() {
		emit triggered(action);
	});
	connect(action, &QAction::changed, this, [=]() {
		itemLabel->setEnabled(action->isEnabled());
	});
	m_actionList.push_back(action);
	connect(itemLabel, &MenuLabelItem::sigItemEnterOrLeave, this, &SdMenu::onItemEnterOrLeave);
	m_height += itemLabel->height();
	return action;
}
MenuAction *SdMenu::appendMenu(const QString &text
	, const QPixmap &icon
	, const QString &shortcutText)
{
	MenuAction *action = new MenuAction(this);
	// QLabel创建时要指定父窗口指针，不然会导致stylesheet中的padding-left等失效
	MenuLabelItem *itemLabel = new MenuLabelItem(text, this, icon, shortcutText, m_iconSize);
	itemLabel->setFixedHeight(m_itemHeight);
	itemLabel->setFocusPolicy(Qt::NoFocus);
	itemLabel->setFixedWidth(m_width);
	action->setDefaultWidget(itemLabel);
	action->setText(text);
	//addAction(action);
	insertAction(m_lastSpaceItem, action);
	connect(action, &QAction::changed, this, [=]() {
		itemLabel->setEnabled(action->isEnabled());
	});
	m_actionList.push_back(action);
	connect(itemLabel, &MenuLabelItem::sigItemEnterOrLeave, this, &SdMenu::onItemEnterOrLeave);
	m_height += itemLabel->height();
	connect(itemLabel, &MenuLabelItem::sigSubMenuHide, this, &SdMenu::sigSubMenuHide);
	connect(this, &SdMenu::sigSubMenuHide, itemLabel, &MenuLabelItem::onOtherSubMenuHide);
	adjustMenuSize();
	return action;
}
MenuAction *SdMenu::appendMenu(const QString &text)
{
	// 处理列表模式的菜单
	if (m_menuType == ListMode)
	{
		if (!m_iconModeAction)
		{
			m_iconModeAction = new MenuAction(this);
			m_iconModeAction->setDefaultWidget(m_listWidget);
			m_iconModeAction->setText(text);
			insertAction(m_lastSpaceItem, m_iconModeAction);
		}
		return addItemToListWidget(text);
	}
	MenuAction *action = new MenuAction(this);
	// QLabel创建时要指定父窗口指针，不然会导致stylesheet中的padding-left等失效
	MenuLabelItem *itemLabel = new MenuLabelItem(text, this);
	itemLabel->setFixedHeight(m_itemHeight);
	//itemLabel->setFixedWidth(m_width);
	itemLabel->adjustSize();
	action->setDefaultWidget(itemLabel);
	action->setText(text);
	insertAction(m_lastSpaceItem, action);
	connect(action, &QAction::triggered, this, [=]() {
		emit triggered(action);
	});
	connect(action, &QAction::changed, this, [=]() {
		itemLabel->setEnabled(action->isEnabled());
	});
	m_actionList.push_back(action);
	connect(itemLabel, &MenuLabelItem::sigItemEnterOrLeave, this, &SdMenu::onItemEnterOrLeave);
	m_height += itemLabel->height();
	connect(itemLabel, &MenuLabelItem::sigSubMenuHide, this, &SdMenu::sigSubMenuHide);
	connect(this, &SdMenu::sigSubMenuHide, itemLabel, &MenuLabelItem::onOtherSubMenuHide);
	adjustMenuSize();
	return action;
}
MenuAction *SdMenu::appendMenu(const QString &text, bool showCheckedPix)
{
	const int checkBoxWidth = 12;
	const int leftMargin = 16;
	MenuAction *action = new MenuAction(this);
	// QLabel创建时要指定父窗口指针，不然会导致stylesheet中的padding-left等失效
	// 设置checkbox 默认图片
	MenuLabelItem *itemLabel = new MenuLabelItem(text, this);
	itemLabel->setCheckPixmap(global::getResImgDPIFullPath("menuitemchoose"));
	itemLabel->setFixedHeight(m_itemHeight);
	itemLabel->setFixedWidth(m_width);
	itemLabel->setCheckedBackgroundColor(m_itemSeletedColor);
	action->setDefaultWidget(itemLabel);
	action->setText(text);
	insertAction(m_lastSpaceItem, action);
	connect(action, &QAction::triggered, this, [=]() {
		emit triggered(action);
		onChooseItemTriggered(action);
	});
	m_actionList.push_back(action);
	connect(itemLabel, &MenuLabelItem::sigItemEnterOrLeave, this, &SdMenu::onItemEnterOrLeave);
	m_height += itemLabel->height();
	connect(itemLabel, &MenuLabelItem::sigSubMenuHide, this, &SdMenu::sigSubMenuHide);
	connect(this, &SdMenu::sigSubMenuHide, itemLabel, &MenuLabelItem::onOtherSubMenuHide);
	adjustMenuSize();
	return action;
}
MenuAction *SdMenu::appendMenu(QWidget *itemWidget)
{
	MenuAction *action = new MenuAction(this);
	action->setDefaultWidget(itemWidget);
	insertAction(m_lastSpaceItem, action);
	m_actionList.push_back(action);
	m_height += itemWidget->height();
	return action;
}
MenuAction *SdMenu::appendMenuWithCloseBtn(const QString &text, int closeWidth /*= 38*/)
{
	MenuAction *action = new MenuAction(this);
	// QLabel创建时要指定父窗口指针，不然会导致stylesheet中的padding-left等失效
	MenuLabelItem *itemLabel = new MenuLabelItem(text, this);
	itemLabel->setFixedHeight(m_itemHeight);
	itemLabel->setCloseBtnVisible(true);
	action->setDefaultWidget(itemLabel);
	action->setText(text);
	insertAction(m_lastSpaceItem, action);
	m_actionList.push_back(action);
	connect(itemLabel, &MenuLabelItem::sigItemEnterOrLeave, this, &SdMenu::onItemEnterOrLeave);
	m_height += itemLabel->height();
	connect(itemLabel, &MenuLabelItem::sigCloseBtnClicked, this, [=]() {
		QAction *action = actionAt(mapFromGlobal(QCursor::pos()));
		if (action)
		{
			emit closeBtnClicked(action);
		}
		close();
	});
	connect(action, &QAction::changed, this, [=]() {
		itemLabel->setEnabled(action->isEnabled());
	});
	connect(itemLabel, &MenuLabelItem::sigSubMenuHide, this, &SdMenu::sigSubMenuHide);
	connect(this, &SdMenu::sigSubMenuHide, itemLabel, &MenuLabelItem::onOtherSubMenuHide);
	return action;
}
MenuAction * SdMenu::appendMenuWithIconAndChooseState(const QString &text, const QPixmap &icon)
{
	MenuAction *action = new MenuAction(this);
	MenuLabelItem *itemLabel = new MenuLabelItem(text, this, icon, "", m_iconSize);
	itemLabel->setFixedHeight(m_itemHeight);
	itemLabel->setCheckPixmap(global::getResImgDPIFullPath("menuitemchoose"));
	action->setDefaultWidget(itemLabel);
	action->setText(text);
	insertAction(m_lastSpaceItem, action);

	connect(action, &QAction::triggered, this, [=]() {
		emit triggered(action);
		onChooseItemTriggered(action);
	});
	m_actionList.push_back(action);
	connect(itemLabel, &MenuLabelItem::sigItemEnterOrLeave, this, &SdMenu::onItemEnterOrLeave);
	m_height += itemLabel->height();
	connect(itemLabel, &MenuLabelItem::sigSubMenuHide, this, &SdMenu::sigSubMenuHide);
	connect(this, &SdMenu::sigSubMenuHide, itemLabel, &MenuLabelItem::onOtherSubMenuHide);
	return action;
}
MenuAction * SdMenu::insertMenu(QAction *beforeAction, const QString &text, const QPixmap &icon)
{
	// 处理列表图标模式的菜单
	if (m_menuType == IconMode)
	{
		if (!m_iconModeAction)
		{
			m_iconModeAction = new MenuAction(this);
			m_iconModeAction->setDefaultWidget(m_listWidget);
			m_iconModeAction->setText(text);
			insertAction(beforeAction, m_iconModeAction);
		}
		return addItemToListWidget(text, icon);
	}
	MenuAction *action = new MenuAction(this);
	// QLabel创建时要指定父窗口指针，不然会导致stylesheet中的padding-left等失效
	MenuLabelItem *itemLabel = new MenuLabelItem(text, this, icon);
	itemLabel->setFixedHeight(m_itemHeight);
	itemLabel->setFocusPolicy(Qt::NoFocus);
	action->setDefaultWidget(itemLabel);
	action->setText(text);
	//addAction(action);
	insertAction(beforeAction, action);
	connect(action, &QAction::triggered, this, [=]() {
		emit triggered(action);
	});
	connect(action, &QAction::changed, this, [=]() {
		itemLabel->setEnabled(action->isEnabled());
	});
	m_actionList.push_back(action);
	connect(itemLabel, &MenuLabelItem::sigItemEnterOrLeave, this, &SdMenu::onItemEnterOrLeave);
	m_height += itemLabel->height();
	connect(itemLabel, &MenuLabelItem::sigSubMenuHide, this, &SdMenu::sigSubMenuHide);
	connect(this, &SdMenu::sigSubMenuHide, itemLabel, &MenuLabelItem::onOtherSubMenuHide);
	return action;
}
MenuAction * SdMenu::insertMenuWithIconAndChooseState(QAction *beforeAction, const QString &text, const QPixmap &icon)
{
	MenuAction *action = new MenuAction(this);
	MenuLabelItem *itemLabel = new MenuLabelItem(text, this, icon);
	itemLabel->setFixedHeight(m_itemHeight);
	action->setDefaultWidget(itemLabel);
	action->setText(text);
	insertAction(beforeAction, action);
	connect(action, &QAction::triggered, this, [=]() {
		emit triggered(action);
		onChooseItemTriggered(action);
	});
	m_actionList.push_back(action);
	connect(itemLabel, &MenuLabelItem::sigItemEnterOrLeave, this, &SdMenu::onItemEnterOrLeave);
	m_height += itemLabel->height();
	connect(itemLabel, &MenuLabelItem::sigSubMenuHide, this, &SdMenu::sigSubMenuHide);
	connect(this, &SdMenu::sigSubMenuHide, itemLabel, &MenuLabelItem::onOtherSubMenuHide);
	return action;
}
MenuAction *SdMenu::append2DMenu(const QString &text, const bool &isShilt)
{
	MenuAction *action = new MenuAction(this);
	// QLabel创建时要指定父窗口指针，不然会导致stylesheet中的padding-left等失效
	MenuLabelItem *itemLabel = new MenuLabelItem(text, this);
	itemLabel->setFixedHeight(m_itemHeight);
	action->setDefaultWidget(itemLabel);
	action->setText(text);
	insertAction(m_lastSpaceItem, action);
	//m_actionList.push_back(action);
	m_height += itemLabel->height();
	connect(action, &QAction::triggered, this, [=]() {
		emit triggered(action);
	});
	connect(itemLabel, &MenuLabelItem::sigSubMenuHide, this, &SdMenu::sigSubMenuHide);
	connect(this, &SdMenu::sigSubMenuHide, itemLabel, &MenuLabelItem::onOtherSubMenuHide);
	return action;
}
MenuAction * SdMenu::findMenuAction(const QString &text)
{
	QAction *action = NULL;
	QList<QAction *> actionList = actions();
	foreach(QAction *action, actionList)
	{
		if (action->isSeparator())
			continue;
		QString actext = action->text();
		if (actext == text)
			return (MenuAction*)action;
	}
	return NULL;
}
QAction *SdMenu::popup(const QPoint &point)
{
	// 临时禁止内嵌网页抢焦点
	AutoForbidWebFocus forbidFocus;
	adjustMenuSize();
	QAction *curAction = exec(QPoint(point.x() - getShadowWidth(), point.y()));
	return curAction;
}
void SdMenu::setWidth(int menuWidth)
{
	m_width = menuWidth;
}
int SdMenu::getWidth()
{
	return m_width;
}
void SdMenu::setItemHeight(int itemHeight)
{
	setStyleSheet(QString("QMenu{border-radius: %1px;}").arg(BORDER_RADIUS)
		+ QString("QMenu::item{border-radius:%1px;min-height:%2px;}").arg(BORDER_RADIUS).arg(_S(itemHeight)));
	m_itemHeight = itemHeight;
	if (m_listWidget)
	{
		if (m_whiteStyle)
		{
			m_listWidget->setStyleSheet(QString("QListWidget#listWidget{background-color:#FFFFFF;border-radius: %0px;padding:0px;border:none;}"
				"QListWidget#listWidget::item{border-radius:%0px;height:%1px;padding-left:%2;color:#222222;}"
				"QListWidget#listWidget::item:hover{background-color:#EAF2FF;color:#2878FF;}"
				"QListWidget#listWidget::item::selected{background-color:#EAF2FF;color:#2878FF;}").arg(_S(2)).arg(m_itemHeight).arg(_S(16)));
		}
		else
		{
			m_listWidget->setStyleSheet(QString("QListWidget#listWidget{background-color:#2C2C2C;border-radius: %0px;padding:0px;border:none;}"
				"QListWidget#listWidget::item{border-radius:%0px;height:%1px;padding-left:%2;color:#999999;}"
				"QListWidget#listWidget::item:hover{background-color:#3A3A3A;color:#999999;}"
				"QListWidget#listWidget::item::selected{background-color:#3A3A3A;color:#999999;}").arg(_S(2)).arg(m_itemHeight).arg(_S(16)));
		}
	}
}
void SdMenu::showTopBottomSpace(bool showSpace)
{
	if (m_firstSpaceItem)
	{
		m_firstSpaceItem->setVisible(showSpace);
	}
	if (m_lastSpaceItem)
	{
		m_lastSpaceItem->setVisible(showSpace);
	}
}
void SdMenu::setFont(QFont font)
{
	m_font = font;
}
void SdMenu::onTriggeredByListWidget(int index)
{
	CheckResult(m_listWidget);
	if (index >= 0 && index < m_listWidget->count())
	{
		emit triggeredByListWidget(m_listWidget->item(index)->text());
		emit triggeredByListWidget(index);
	}
	if (index >= 0 && index < m_listItemAction.size())
	{
		emit triggered(m_listItemAction.at(index));
	}

	if (m_menuType == ListMode)
	{
		close();
	}
}
void SdMenu::onChooseItemTriggered(MenuAction *action)
{
	if (!m_mulitSelect)
	{
		QList<QAction *> actionList = actions();
		foreach(QAction *item, actionList)
		{
			if (item->isSeparator())
				continue;

			QWidgetAction *widgetAction = (QWidgetAction*)item;
			if (!widgetAction->text().isEmpty())
			{
				MenuLabelItem *widgetItem = (MenuLabelItem *)widgetAction->defaultWidget();
				if (widgetItem)
				{
					if (action == item)
					{
						widgetItem->setCheckable(true);
					}
					else
					{
						widgetItem->setCheckable(false);
					}
				}
			}
		}
	}
	else
	{
		QWidgetAction *widgetAction = (QWidgetAction*)action;
		MenuLabelItem *widgetItem = (MenuLabelItem *)widgetAction->defaultWidget();
		if (widgetItem)
		{
			widgetItem->setCheckable(!widgetItem->getCheckedState());
		}
	}
}
void SdMenu::onItemEnterOrLeave(MenuLabelItem *menuItem, bool enter)
{
	CheckResult(menuItem);
	if (!menuItem->text().isEmpty())
	{
		if (!m_itemTips)
		{
			m_itemTips = QSharedPointer<NavWidgetTp>(new NavWidgetTp(this, ""));
			m_itemTips->hide();
		}
		foreach(MenuAction *item, m_actionList)
		{
			if (item && item->defaultWidget() == menuItem)
			{
				QString itemTipStr = item->customToolTip();
				if (!enter || itemTipStr.isEmpty())
				{
					m_itemTips->hide();
				}
				else
				{
					m_itemTips->setTipsWithWidth(itemTipStr, 200, _SM(5, 5, 5, 5));
					QRect tipRect = m_itemTips->rect();
					QPoint pos = menuItem->mapToGlobal(QPoint(0, 0));
					pos.setX(pos.x() - tipRect.width() - _S(4));
					m_itemTips->move(pos);
					m_itemTips->show();
				}
				break;
			}
		}
	}
}
void SdMenu::clearData()
{
	m_actionList.clear();
	if (m_menuType == ListMode)
	{
		for (auto listItem : m_listItemAction)
		{
			if (listItem)
				delete listItem;
		}
		m_listItemAction.clear();
		if (m_listWidget)
			m_listWidget->clear();
		return;
	}
	m_height = 0;
	this->clear();
}
void SdMenu::setHoverColor(const QString &hoverBkCol, const QString &hoverTextCol)
{
	m_itemHoverClr = hoverBkCol;
	m_itemHoverTextClr = hoverTextCol;
	QList<QAction *> actionList = actions();
	foreach(QAction *itemAction, actionList)
	{
		if (itemAction->isSeparator())
			continue;
		QWidgetAction *action = (QWidgetAction*)itemAction;
		if (!action->text().isEmpty())
		{
			MenuLabelItem *labelItem = (MenuLabelItem *)action->defaultWidget();
			if (labelItem)
			{
				//labelItem->setHoverColor(hoverBkCol, hoverTextCol);
			}
		}
	}
}
void SdMenu::setNormalColor(const QString &norTextColor, const QString &normalBgColor)
{
	foreach(MenuAction *itemAction, m_actionList)
	{
		MenuLabelItem *labelItem = (MenuLabelItem *)itemAction->defaultWidget();
		if (labelItem)
		{
			labelItem->setNormalColor(norTextColor/*, normalBgColor*/);
		}
	}
}
void SdMenu::initIconListWidget()
{
	if (m_listWidget)
		return;
	m_listWidget = new SdMenuListWidget(this);
	m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	//m_listWidget->setMinimumHeight(_S(540));
	m_listWidget->setIconSize(QSize(m_iconListItemSize.width(), m_iconListItemSize.height() - _S(ITEM_TEXT_HEIGHT)));
	m_listWidget->setResizeMode(QListView::Adjust);
	m_listWidget->setViewMode(QListView::IconMode);
	m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_listWidget->horizontalScrollBar()->setEnabled(false);
	m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_listWidget->setWrapping(true);
	m_listWidget->setDragEnabled(false);
	m_listWidget->setFocusPolicy(Qt::NoFocus);
	if (m_whiteStyle)
	{
		m_listWidget->setStyleSheet(QString("QListWidget{border: 0px;margin-left:0px;margin-right:0px;margin-top:0px;margin-bottom:0px;}"
			"QListWidget::Item{margin-left:0px;margin-right:0px;margin-bottom:0px;margin-top:0px;border:%2px solid transparent;border-radius:%1px;color:#222222;}"
			"QListWidget::Item:hover{background:transparent; border:%2px solid transparent;border-radius:%1px;}"
			"QListWidget::item:selected{background:transparent; border:%2px solid transparent;border-radius:%1px;}"
			"QListWidget::item:selected:!active{background:transparent; border:%2px solid transparent;border-radius:%1px;}")
			.arg(_S(ROUND_RADIU)).arg(LIST_ITEM_BORDER));
	}
	else
	{
		m_listWidget->setStyleSheet(QString("QListWidget{border: 0px;margin-left:0px;margin-right:0px;margin-top:0px;margin-bottom:0px;}"
			"QListWidget::Item{margin-left:0px;margin-right:0px;margin-bottom:0px;margin-top:0px;border:%2px solid transparent;border-radius:%1px;color:#999999;}"
			"QListWidget::Item:hover{background:transparent; border:%2px solid transparent;border-radius:%1px;}"
			"QListWidget::item:selected{background:transparent; border:%2px solid transparent;border-radius:%1px;}"
			"QListWidget::item:selected:!active{background:transparent; border:%2px solid transparent;border-radius:%1px;}")
			.arg(_S(ROUND_RADIU)).arg(LIST_ITEM_BORDER));
	}
	m_listWidget->verticalScrollBar()->setStyleSheet(gScrollbarQSS.arg(_S(14)).arg(_S(6)));
	m_listWidget->setMovement(QListView::Static);
	m_listWidget->setSpacing(m_itemSpacing);
	m_listWidget->setItemDelegate(new MenuIconBorderDelegate(this));
	connect(m_listWidget, &QListWidget::currentRowChanged, this, &SdMenu::onTriggeredByListWidget);
	m_listWidget->installEventFilter(this);
}
void SdMenu::initListWidget()
{
	//需要menu显示滚动条，并且限制长度时使用，此时只需要设置最大高度即可
	if (m_listWidget)
		return;
	m_listWidget = new SdMenuListWidget(this);
	m_listWidget->setObjectName("listWidget");
	m_listWidget->setFocusPolicy(Qt::NoFocus);
	m_listWidget->setWindowFlags(Qt::ToolTip);
	m_listWidget->setDragEnabled(false);
	m_listWidget->setViewMode(QListView::ListMode);
	m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_listWidget->horizontalScrollBar()->setEnabled(false);
	if (m_whiteStyle)
	{
		m_listWidget->setStyleSheet(QString("QListWidget#listWidget{background-color:#FFFFFF;border-radius: %0px;padding:0px;border:none;}"
			"QListWidget#listWidget::item{border-radius:%0px;height:%1px;padding-left:%2;color:#222222;}"
			"QListWidget#listWidget::item:hover{background-color:#EAF2FF;color:#2878FF;}"
			"QListWidget#listWidget::item::selected{background-color:#EAF2FF;color:#2878FF;}").arg(_S(2)).arg(m_itemHeight).arg(_S(16)));
		m_listWidget->verticalScrollBar()->setStyleSheet(
			gScrollBarQSS.arg(_S(5)).arg(_S(2)).arg("#FFFFFF").arg("#BBBBBB"));
	}
	else
	{
		m_listWidget->setStyleSheet(QString("QListWidget#listWidget{background-color:#2C2C2C;border-radius: %0px;padding:0px;border:none;}"
			"QListWidget#listWidget::item{border-radius:%0px;height:%1px;padding-left:%2;color:#999999;}"
			"QListWidget#listWidget::item:hover{background-color:#3A3A3A;color:#999999;}"
			"QListWidget#listWidget::item::selected{background-color:#3A3A3A;color:#999999;}").arg(_S(2)).arg(m_itemHeight).arg(_S(16)));
		m_listWidget->verticalScrollBar()->setStyleSheet(
			gScrollBarQSS.arg(_S(5)).arg(_S(2)).arg("#262626").arg("#3C3C3C"));
	}
	m_listWidget->setMovement(QListView::Static);
	m_listWidget->setResizeMode(QListView::Adjust);
	m_listWidget->setMouseTracking(true);
	connect(m_listWidget, &QListWidget::currentRowChanged, this, &SdMenu::onTriggeredByListWidget);
	connect(m_listWidget, &SdMenuListWidget::sigMouseMove, this, [=](QPoint globalPos) {
		handleListWidgetMouseMove(globalPos);
	});
}
void SdMenu::updateListItemIcon(int index, const QPixmap &image)
{
	if (index < 0 || index >= m_listWidget->count())
		return;
	CheckResult(m_listWidget);
	QListWidgetItem *listWidgetItem = m_listWidget->item(index);
	const QSize itemSize(m_iconListItemSize.width() - 4, m_iconListItemSize.height() - _S(ITEM_TEXT_HEIGHT));// -4是边框宽度
	QPixmap pixmap = genericfunc::pixmapToRound(image.scaled(itemSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation), _S(ROUND_RADIU));
	QIcon icon;
	icon.addPixmap(pixmap, QIcon::Normal);
	icon.addPixmap(pixmap, QIcon::Selected);
	listWidgetItem->setIcon(icon);
}
MenuAction * SdMenu::findActionByText(const QString &text)
{
	for (int i = 0; i < actions().size(); i++)
	{
		if (actions().at(i)->text().isEmpty())
			continue;
		if (actions().at(i)->text() == text)
			return static_cast<MenuAction *>(actions().at(i));
	}
	return nullptr;
}
MenuAction * SdMenu::findActionByIndex(int index)
{
	if (index > m_actionList.size())
		return nullptr;
	return m_actionList.at(index);
}
void SdMenu::setChooseItem(MenuAction *action)
{
	onChooseItemTriggered(action);
}
void SdMenu::clearAllSelected()
{
	QList<QAction *> actionList = actions();
	foreach(QAction *item, actionList)
	{
		if (item->isSeparator())
			continue;
		QWidgetAction *widgetAction = (QWidgetAction*)item;
		if (!widgetAction->text().isEmpty())
		{
			MenuLabelItem *widgetItem = (MenuLabelItem *)widgetAction->defaultWidget();
			if (widgetItem)
			{
				widgetItem->setCheckable(false);
			}
		}
	}
}
void SdMenu::enableMenuItem(QAction *action, bool enable)
{
	MenuLabelItem *labelItem = static_cast<MenuLabelItem *>(((QWidgetAction *)action)->defaultWidget());
	CheckResult(labelItem);
	labelItem->setEnabled(enable);
}
void SdMenu::deleteAction(const QString &actionText)
{
	QList<QAction *> actionList = actions();
	foreach(QAction *pAction, actionList)
	{
		if (pAction->text() == actionText)
			removeAction(pAction);
	}
	m_height -= m_itemHeight;
}
int SdMenu::getShadowWidth()
{
	return MENU_SHADOW_WIDTH;
}
int SdMenu::getItemCount()
{
	return m_actionList.size();
}
void SdMenu::setItemSelectedColor(const QString & color)
{
	m_itemSeletedColor = color;
}
void SdMenu::setFixedWidth(int width)
{
	m_width = width;
}
void SdMenu::setMaximumHeight(int height)
{
	if (m_listWidget)
	{
		m_listWidget->setMaximumHeight(height);
	}
	QMenu::setMaximumHeight(height + MENU_SHADOW_WIDTH * 2 + _S(SPACE_HEIGHT) * 2);
}
void SdMenu::setIconListItemSize(const QSize &size)
{
	m_iconListItemSize = size;
}
QSize SdMenu::getIconListItemSize()
{
	return m_iconListItemSize;
}
int SdMenu::getIconListSpacing()
{
	return m_itemSpacing;
}

void SdMenu::setMulitSelect(bool mulitSelect)
{
	m_mulitSelect = mulitSelect;
}
MenuAction *SdMenu::addItemToListWidget(const QString &text, const QPixmap &image)
{
	QFont font("Microsoft YaHei", 9);
	const QSize itemSize(m_iconListItemSize.width() - 4, m_iconListItemSize.height() - _S(ITEM_TEXT_HEIGHT));// -4是边框宽度
	QListWidgetItem* listWidgetItem = new QListWidgetItem();
	listWidgetItem->setSizeHint(m_iconListItemSize);
	listWidgetItem->setFont(font);
	listWidgetItem->setText(text);
	QPixmap pixmap = genericfunc::pixmapToRound(image.scaled(itemSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation), _S(ROUND_RADIU));
	QIcon icon;
	icon.addPixmap(pixmap, QIcon::Normal);
	icon.addPixmap(pixmap, QIcon::Selected);
	listWidgetItem->setIcon(icon);
	m_listWidget->addItem(listWidgetItem);
	MenuAction *action = new MenuAction(this);
	action->setText(text);
	m_listItemAction.push_back(action);
	adjustMenuSize();
	return action;
}
MenuAction *SdMenu::addItemToListWidget(const QString &text)
{
	m_listWidget->addItem(text);
	MenuAction *action = new MenuAction(this);
	action->setText(text);
	m_listItemAction.push_back(action);
	adjustMenuSize();
	return action;
}
void SdMenu::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (m_whiteStyle)
		shadowborderfunc::_drawBorder(&painter, rect(), MENU_SHADOW_WIDTH, 2.4f, BORDER_RADIUS, QColor(99, 99, 99));
	else
		shadowborderfunc::_drawBorder(&painter, rect(), MENU_SHADOW_WIDTH, 2.4f, BORDER_RADIUS, QColor(0, 0, 0));
	QRect rc = this->rect();
	rc.adjust(MENU_SHADOW_WIDTH, MENU_SHADOW_WIDTH, -MENU_SHADOW_WIDTH, -MENU_SHADOW_WIDTH);//减去阴影
	//获取绘制路径
	QPainterPath path = shadowborderfunc::_getBoundPath(rc, BORDER_RADIUS, shadowborderfunc::ROUND_ALL);
	painter.setRenderHint(QPainter::Antialiasing);
	if (m_whiteStyle)
		painter.fillPath(path, QColor(MENU_BACKGROUND_CLR_WHITE));
	else
		painter.fillPath(path, QColor(MENU_BACKGROUND_CLR));
}

bool SdMenu::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == this && m_menuType == NormalMode)
	{
		if (event->type() == QEvent::MouseMove)
		{
			QPointF point = ((QMouseEvent*)event)->localPos();
			QList<QAction *> actionList = actions();
			foreach(QAction *item, actionList)
			{
				if (item->isSeparator())
					continue;
				QWidgetAction *widgetAction = (QWidgetAction*)item;
				if (!widgetAction->text().isEmpty())
				{
					MenuLabelBaseItem *widgetItem = static_cast<MenuLabelBaseItem *>(widgetAction->defaultWidget());
					if (widgetItem)
					{
						QRect rc = actionGeometry(widgetAction);
						if (rc.contains(point.toPoint()))
							widgetItem->setMouseHover(true);
						else
							widgetItem->setMouseHover(false);
					}
				}
			}
		}
	}

	if (m_listWidget && event->type() == QEvent::Resize && m_menuType != NormalMode)
	{
		this->setFixedSize(QSize(m_width + MENU_SHADOW_WIDTH * 2, m_listWidget->height() + MENU_SHADOW_WIDTH * 2));
	}
	return QMenu::eventFilter(watched, event);
}
void SdMenu::showEvent(QShowEvent *event)
{
	adjustMenuSize();
	QMenu::showEvent(event);
}
#ifdef _TEST_
MenuWidgetTest::MenuWidgetTest(QWidget* parent /*= NULL*/)
	: QPushButton(parent)
{
	setFixedSize(_SS(400, 200));
	SdMenu* pMenu = new SdMenu(this);
	pMenu->setFixedWidth(_S(180));
	pMenu->setObjectName("RenderMenu");
	pMenu->appendSpaceItem();
	//一级菜单
	{
		pMenu->appendSeparatorItem();
		pMenu->appendSeparatorItem("#222222");
		pMenu->appendSeparatorItem("#3ddddd", 5);
		pMenu->appendMenu("12333");
		pMenu->appendMenu("12333333");
		pMenu->appendMenu("12333445555");
		pMenu->appendMenu("12333fssssssssssssssssssssssssssssss4dfdfsf45555");
		pMenu->appendMenu("1633dd", global::getResImgDPIFullPath("2dwidget/rulermenu"));
		pMenu->appendMenu("633dfsfs3", global::getResImgDPIFullPath("2dwidget/rulermenu"));
		pMenu->appendMenu(tr("Save165464646464646464645646545646464"), QPixmap(), "Ctrl+S");
		pMenu->appendMenu(tr("Save2"), QPixmap(), "Ctrl+S");
		pMenu->appendMenu(tr("Save3"), QPixmap(), "Ctrl+S");
		pMenu->appendMenu(tr("Save3"), true);
		//插入QWidget作为Item项
		pMenu->appendMenu(new ItemWidgetTest(_S(80), _S(60), this));
	}
	{
		//二级菜单
		MenuAction* pAction = pMenu->appendMenu("2Menu");
		{
			SdMenu* pSecMenu = new SdMenu;
			pSecMenu->setFixedWidth(_S(180));
			pSecMenu->setObjectName("2RenderMenu");
			pSecMenu->appendMenu("12333");
			pSecMenu->appendMenu("12333333");
			pSecMenu->appendMenu("12333445555");
			pSecMenu->appendMenu("12333fssssssssssssssssssssssssssssss4dfdfsf45555");
			pAction->setMenu(pSecMenu);
		}
	}
	setMenu(pMenu);
}
MenuWidgetTest::~MenuWidgetTest()
{
}

ItemWidgetTest::ItemWidgetTest(int width, int height, QWidget* parent /*= NULL*/)
	: QWidget(parent)
{
	setFixedSize(width, height);
	QVBoxLayout *pLayout = new QVBoxLayout;
	pLayout->setContentsMargins(_SM(10, 0, 10, 0));
	pLayout->setSpacing(_S(10));
	QLabel* pLabel = new QLabel(this);
	pLabel->setText("fafafaf3333333333333333333333333332222223333333333");
	pLabel->setFont(QFont("Microsoft YaHei", 10));
	pLabel->setFixedHeight(_S(20));
	QLabel* pLabel1 = new QLabel(this);
	pLabel1->setText("fafafaf3333333333333333333333333332222223333333333");
	pLabel1->setFont(QFont("Microsoft YaHei", 14));
	pLabel1->setFixedHeight(_S(40));
	pLayout->addWidget(pLabel);
	pLayout->addWidget(pLabel1);
	setLayout(pLayout);
}
ItemWidgetTest::~ItemWidgetTest()
{
}
