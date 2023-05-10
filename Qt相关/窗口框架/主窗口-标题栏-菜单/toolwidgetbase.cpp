
#include "pch.h"
#include "mainwindowmenubar.h"
#include <QMenuBar>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include "common/dpiscaled.h"
#include "common/colordef.h"
#include "common/sdstylesheet.h"
#include "general/imageutils.h"
#include "ui/button/textbutton.h"
#include "ui/button/imagebutton.h"
namespace
{
	const int g_menuBarHeight = 28;
	const int g_menuBarLogoWidth = 46;
	const int g_menuBarLogoHeight = 11;
	const QString g_menuBarBgColor = ColorDef::combineAlphaF(0.08, ColorDef::clr_content_bg);
	const int g_toolBarHeight = 60;
	const QString g_toolBarBgColor = ColorDef::combineAlphaF(0.10, ColorDef::clr_content_bg);
}
RenderMainWindowMenuBar::RenderMainWindowMenuBar(QPointer<QWidget> mainwindow)
	: QWidget(mainwindow)
	, m_mainwindow(mainwindow)
{
	initUi();
}

void RenderMainWindowMenuBar::appendMainMenu(RenderMainWindowMenuId id, const QString& keyText, const QString& showText, const QString& key, QWidget* toolWidget)
{
	if (!m_menuLayout || !m_btnGroup || !toolWidget)
		return;
	if (m_mapMenuData.count(id) > 0)
		return;
	QString btnShowText = showText + tr("(") + key + tr(")");
	MenuIdAndPtr menuData = { id, new TextButton(btnShowText, this), toolWidget };
	m_mapMenuData.insert(id, menuData);
	menuData.menuBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
	m_menuLayout->addWidget(menuData.menuBtn);
	m_btnGroup->addButton(menuData.menuBtn, static_cast<int>(menuData.id));
	m_toolStackedWidget->addWidget(toolWidget);
	// 解析设置快捷键
	menuData.menuBtn->setShortcut(QKeySequence::mnemonic(keyText));
}

void RenderMainWindowMenuBar::selectMainMenu(RenderMainWindowMenuId id, bool btnNeedChecked)
{
	auto menuData = m_mapMenuData.find(id);
	QPointer<QWidget> toolWidget = menuData.value().toolWidget;
	if (toolWidget)
		m_toolStackedWidget->setCurrentWidget(toolWidget);
	if (btnNeedChecked)
	{
		QPointer<TextButton> menuBtn = menuData.value().menuBtn;
		if (menuBtn)
			menuBtn->setChecked(true);
	}
}

QSize RenderMainWindowMenuBar::sizeHint() const
{
	return QSize(-1, _S(g_menuBarHeight + g_toolBarHeight));
}

QSize RenderMainWindowMenuBar::minimumSizeHint() const
{
	return sizeHint();
}
void RenderMainWindowMenuBar::setWindowBtns(WindowBtn windowBtns)
{
	if (m_btnMin)
		m_btnMin->setVisible(windowBtns & WindowBtn::WB_Min);
	if (m_btnMaxRestore)
		m_btnMaxRestore->setVisible(windowBtns & WindowBtn::WB_Max_Restore);
	if (m_btnClose)
		m_btnClose->setVisible(windowBtns & WindowBtn::WB_Close);
}

void RenderMainWindowMenuBar::onButtonClicked(int btnId)
{
	selectMainMenu(static_cast<RenderMainWindowMenuId>(btnId));
}

void RenderMainWindowMenuBar::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QPainter p(this);
	auto menuBarRect = rect();
	menuBarRect.setHeight(_S(g_menuBarHeight));
	{
		p.setBrush(QColor(g_menuBarBgColor));
		p.setPen(Qt::transparent);
		p.drawRect(menuBarRect);
		QPen pen;
		pen.setColor(ColorDef::clr_line);
		pen.setWidth(_S(1));
		p.setPen(pen);
		p.drawLine(QPoint(0, menuBarRect.height() - pen.width()), QPoint(menuBarRect.width() - pen.width(), menuBarRect.height() - pen.width()));
	}

	auto toolBarRect = rect();
	toolBarRect.setY(_S(g_menuBarHeight));
	toolBarRect.setHeight(_S(g_toolBarHeight));
	{
		p.setBrush(QColor(g_toolBarBgColor));
		//p.setBrush(QColor("#FF000"));
		p.setPen(Qt::transparent);
		p.drawRect(toolBarRect);
	}
}
void RenderMainWindowMenuBar::mousePressEvent(QMouseEvent* event)
{
	m_dragging = true;
	m_dragStartPoint = event->pos();
	Base::mousePressEvent(event);
}
void RenderMainWindowMenuBar::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_mainwindow || m_mainwindow->isMaximized())
	{
		Base::mouseMoveEvent(event);
		return;
	}
	if (m_dragging)
	{
		int x = m_mainwindow->x() + (event->pos() - m_dragStartPoint).x();
		int y = m_mainwindow->y() + (event->pos() - m_dragStartPoint).y();
		m_mainwindow->move(x, y);
	}
	Base::mouseMoveEvent(event);
}
void RenderMainWindowMenuBar::mouseReleaseEvent(QMouseEvent* event)
{
	m_dragging = false;
	m_dragStartPoint = QPoint();
	Base::mouseReleaseEvent(event);
}
void RenderMainWindowMenuBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (!m_mainwindow)
	{
		Base::mouseDoubleClickEvent(event);
		return;
	}
	switchMainWindow();
	// 不要调用父类双击事件，否则还原窗口大小时位置不对
	//Base::mouseDoubleClickEvent(event);
}
void RenderMainWindowMenuBar::initUi()
{
	QVBoxLayout* thisLayout = new QVBoxLayout(this);
	thisLayout->setContentsMargins(_SM(0, 0, 0, 0));
	thisLayout->setSpacing(_S(0));
	QHBoxLayout* menuBarLayout = createMenuBarLayout();
	m_toolStackedWidget = new QStackedWidget(this);
	m_toolStackedWidget->setFixedHeight(_S(g_toolBarHeight));
	m_toolStackedWidget->setStyleSheet(QString("background:transparent;"));
	thisLayout->addLayout(menuBarLayout);
	thisLayout->addWidget(m_toolStackedWidget);
}
QHBoxLayout* RenderMainWindowMenuBar::createMenuBarLayout()
{
	QHBoxLayout* menuBarLayout = new QHBoxLayout;
	menuBarLayout->setContentsMargins(_SM(12, 0, 0, 0));
	menuBarLayout->setSpacing(_S(0));
	m_menuLayout = new QHBoxLayout;
	m_menuLayout->setContentsMargins(_SM(0, 0, 0, 0));
	m_menuLayout->setSpacing(_S(0));
	QLabel* logo = new QLabel(this);
	logo->setFixedSize(_SS(g_menuBarLogoWidth, g_menuBarLogoHeight));
	logo->setStyleSheet(gNormalImgLabel.arg(imageutils::getSvgResPathNomal("logo")));
	m_btnGroup = new QButtonGroup(this);
	connect(m_btnGroup, qOverload<int>(&QButtonGroup::buttonClicked), this, &RenderMainWindowMenuBar::onButtonClicked);
	QHBoxLayout* btnLayout = new QHBoxLayout;
	btnLayout->setContentsMargins(_SM(12, 0, 12, 0));
	btnLayout->setSpacing(_S(16));
	const QSize btnSize = _SS(16, 16);
	m_btnMin = new QPushButton(this);
	m_btnMin->setFixedSize(btnSize);
	m_btnMin->setStyleSheet(gCommonBtnThreeStateQSS.arg(imageutils::getSvgResPathNomal("minimizeMain"))
		.arg(imageutils::getSvgResPathHover("minimizeMain"))
		.arg(imageutils::getSvgResPathCliecked("minimizeMain")));
	m_btnMaxRestore = new QPushButton(this);
	m_btnMaxRestore->setFixedSize(btnSize);
	m_btnMaxRestore->setStyleSheet(gCommonBtnThreeStateQSS.arg(imageutils::getSvgResPathNomal("maximizeMain"))
		.arg(imageutils::getSvgResPathHover("maximizeMain"))
		.arg(imageutils::getSvgResPathCliecked("maximizeMain")));
	m_btnClose = new QPushButton(this);
	m_btnClose->setFixedSize(btnSize);
	m_btnClose->setStyleSheet(gCommonBtnThreeStateQSS.arg(imageutils::getSvgResPathNomal("closeMain"))
		.arg(imageutils::getSvgResPathHover("closeMain"))
		.arg(imageutils::getSvgResPathCliecked("closeMain")));
	btnLayout->addWidget(m_btnMin);
	btnLayout->addWidget(m_btnMaxRestore);
	btnLayout->addWidget(m_btnClose);
	menuBarLayout->addWidget(logo);
	menuBarLayout->addSpacing(_S(12));
	menuBarLayout->addLayout(m_menuLayout);
	menuBarLayout->addStretch();
	menuBarLayout->addLayout(btnLayout);
	connect(m_btnMin, &QPushButton::clicked, this, [=]() {
		showMainWindow(MainWindowSize::Min);
	});
	connect(m_btnMaxRestore, &QPushButton::clicked, this, [=]() {
		switchMainWindow();
	});
	connect(m_btnClose, &QPushButton::clicked, this, [=]() {
		closeMainWindow();
	});
	return menuBarLayout;
}
void RenderMainWindowMenuBar::showMainWindow(MainWindowSize size)
{
	switch (size)
	{
	case RenderMainWindowMenuBar::MainWindowSize::Normal:
	{
		if (m_mainwindow)
			m_mainwindow->showNormal();
	}
	break;
	case RenderMainWindowMenuBar::MainWindowSize::Min:
	{
		if (m_mainwindow)
			m_mainwindow->showMinimized();
	}
	break;
	case RenderMainWindowMenuBar::MainWindowSize::Max:
	{
		if (m_mainwindow)
			m_mainwindow->showMaximized();
	}
	break;
	default:
		break;
	}
}
void RenderMainWindowMenuBar::switchMainWindow()
{
	if (!m_mainwindow)
		return;
	if (m_mainwindow->isMaximized())
		showMainWindow(MainWindowSize::Normal);
	else
		showMainWindow(MainWindowSize::Max);
}
void RenderMainWindowMenuBar::closeMainWindow()
{
	if (!m_mainwindow)
		return;
	m_mainwindow->close();
	emit sigClose();
}
