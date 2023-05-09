
#include "pch.h"
#include "navwidgettips.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include "common/dpiscaled.h"
#include "common/sdstylefunc.h"
#include "common/fontdef.h"
#include "mainwindow.h"
const int gLbMargin = 5;
const int gDefaultTipsHeight = 30;
NavWidgetTp::NavWidgetTp(QWidget* bindWidget, const QString& content)
	: QDialog(QApplication::desktop()) // QApplication::desktop()防止窗口出来时把父窗口位置提前了
	, m_textColor(0x999999)
	, m_backGroundColor(0x1C1C1C)
	, m_borderColor(0x131314)
	, m_pLabel(nullptr)
	, m_content(content)
	, m_pAncestorWidget(nullptr)
	, m_ancestorWidgetIntervalX1(0)
	, m_ancestorWidgetIntervalY1(0)
	, m_ancestorWidgetIntervalX2(0)
	, m_ancestorWidgetIntervalY2(0)
	, m_tpPosition(TpTop)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::Tool);
	setAttribute(Qt::WA_ShowModal, false);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_TransparentForMouseEvents, true);
	setAttribute(Qt::WA_ShowWithoutActivating, true);
	QVBoxLayout* pLayout = new QVBoxLayout(this);
	pLayout->setContentsMargins(_SM(12, 2, 12, 2));
	pLayout->setSpacing(0);
	QFont font = FontDef::getFont();
	// 之前使用QPalette的方式不够正确，在父控件处于disable时，文字会有重影，目前改用stylesheet设置文字颜色
	/*QPalette pal;
	pal.setColor(QPalette::WindowText, m_textColor);*/
	QFontMetrics mc(font);
	m_pLabel = new QLabel(this);
	m_pLabel->setFixedHeight(_S(gDefaultTipsHeight - gLbMargin));
	m_pLabel->setText(content);
	m_pLabel->setFont(font);
	//m_pLabel->setPalette(pal);
	updateTextColor();
	pLayout->addWidget(m_pLabel);
	setLayout(pLayout);
	int width = mc.width(content) + _S(12) * 2;
	setFixedSize(width, _S(gDefaultTipsHeight));
	setFocusPolicy(Qt::NoFocus);
	setMouseTracking(false);
	if (bindWidget)
	{
		m_pWidget = bindWidget;
		m_pWidget->installEventFilter(this);
		// 因为this以QApplication::desktop()为parent，但this所需要的生命周期不长于m_pWidget，因此在m_pWidget释放的时候也释放this
		connect(m_pWidget, &QObject::destroyed, this, [=]() {
			deleteLater();
		});
	}
}
NavWidgetTp::~NavWidgetTp()
{
}
QPoint NavWidgetTp::placeTip(EmTpPosition position, int interval, QWidget* w)
{
	QPoint pt;
	int xOffset = 0;
	int yOffset = 0;
	//int tempOffset = 2;
	m_tpPosition = position;
	m_interval = interval;

	if (w)
		m_pWidget = w;
	switch (position)
	{
	case EmTpPosition::TpTop:
	{
		pt = w->mapToGlobal(w->rect().topLeft());
		xOffset = (w->width() - this->width()) / 2;
		yOffset = -(this->height() + interval);
	}
	break;
	case EmTpPosition::TpBottom:
	{
		pt = w->mapToGlobal(w->rect().bottomLeft());
		xOffset = (w->width() - this->width()) / 2;
		yOffset = interval;
	}
	break;
	case EmTpPosition::TpRight:
	{
		pt = w->mapToGlobal(w->rect().topRight());
		yOffset = (w->height() - this->height()) / 2;
		xOffset = interval;
	}
	break;
	case EmTpPosition::TpLeft:
	{
		pt = w->mapToGlobal(w->rect().topLeft());
		yOffset = (w->height() - this->height()) / 2;
		xOffset = -(this->width() + interval);
	}
	break;
	case EmTpPosition::TpLeftBottom:
	{
		pt = w->mapToGlobal(w->rect().bottomLeft());
		xOffset = -(this->width() + interval);
		yOffset = interval;
	}
	break;
	case EmTpPosition::TpRightBottom:
	{
		pt = w->mapToGlobal(w->rect().bottomRight());
		xOffset = -(w->width());
		yOffset = interval;
	}
	break;
	default:
		break;
	}
	pt.setX(pt.x() + xOffset);
	pt.setY(pt.y() + yOffset);
	this->move(pt);
	return pt;
}
QPoint NavWidgetTp::placeTip(EmTpPosition position, int interval, QRect widgetRect)
{
	QPoint pt;
	int xOffset = 0;
	int yOffset = 0;
	//int tempOffset = 2;
	m_tpPosition = position;
	m_interval = interval;
	switch (position)
	{
	case EmTpPosition::TpTop:
	{
		pt = widgetRect.topLeft();
		xOffset = (widgetRect.width() - this->width()) / 2;
		yOffset = -(this->height() + interval);
	}
	break;
	case EmTpPosition::TpBottom:
	{
		pt = widgetRect.bottomLeft();
		xOffset = (widgetRect.width() - this->width()) / 2;
		yOffset = interval;
	}
	break;
	case EmTpPosition::TpRight:
	{
		pt = widgetRect.topRight();
		yOffset = (widgetRect.height() - this->height()) / 2;
		xOffset = interval;
	}
	break;
	case EmTpPosition::TpLeft:
	{
		pt = widgetRect.topLeft();
		yOffset = (widgetRect.height() - this->height()) / 2;
		xOffset = -(this->width() + interval);
	}
	break;
	case EmTpPosition::TpLeftBottom:
	{
		pt = widgetRect.bottomLeft();
		xOffset = -(this->width() + interval);
		yOffset = interval;
	}
	break;
	case EmTpPosition::TpRightBottom:
	{
		pt = widgetRect.bottomRight();
		xOffset = -(widgetRect.width());
		yOffset = interval;
	}
	break;
	default:
		break;
	}
	pt.setX(pt.x() + xOffset);
	pt.setY(pt.y() + yOffset);
	this->move(pt);
	return pt;
}
void NavWidgetTp::hideTips()
{
	close();
	deleteLater();
}
void NavWidgetTp::setTipColor(const QColor &bgColor, const QColor &textColor)
{
	m_textColor = textColor;
	m_backGroundColor = bgColor;
	/*QPalette pal;
	pal.setColor(QPalette::WindowText, m_textColor);
	m_pLabel->setPalette(pal);*/
	updateTextColor();
	update();
}
void NavWidgetTp::setBorderColor(const QColor &borderColor)
{
	m_borderColor = borderColor;
	update();
}
void NavWidgetTp::setTipInterval(int interval)
{
	m_interval = interval;
	update();
}
void NavWidgetTp::setTipPosition(EmTpPosition position)
{
	m_tpPosition = position;
}
void NavWidgetTp::setTips(const QString &tips)
{
	const int maxWidth = _S(500);
	m_content = tips;
	m_pLabel->setText(m_content);
	QFontMetrics mc(FontDef::getFont());
	int width = mc.width(m_content) + _S(12) * 2;
	if (width > maxWidth)
	{
		int newHeight = SdStyleFunc::getTextHeight(FontDef::getFont(), maxWidth, tips);
		setFixedSize(maxWidth, newHeight);
		m_pLabel->setFixedHeight(newHeight - _S(gLbMargin));
		m_pLabel->setWordWrap(true);
		update();
		return;
	}
	m_pLabel->setFixedHeight(_S(gDefaultTipsHeight) - _S(gLbMargin));
	setFixedSize(width, _S(gDefaultTipsHeight));
	update();
}
void NavWidgetTp::setTipsWidthLineBreak(const QString &tips)
{
	m_content = tips;
	m_pLabel->setText(m_content);
	QFontMetrics mc(FontDef::getFont());
	int minLineWidth = 0;
	int lineHeight = 0;
	int lineCount = 1;
	QStringList strList = m_content.split("\r\n");
	for (int i = 0; i < strList.size(); i++)
	{
		QRect tipsRect = mc.boundingRect(strList.at(i));
		if (minLineWidth < tipsRect.width())
			minLineWidth = tipsRect.width();
		lineHeight = tipsRect.height();
		lineCount++;
	}
	setFixedSize(minLineWidth + _S(12) * 2, lineCount * lineHeight);
	m_pLabel->setFixedHeight(height());
	update();
}
void NavWidgetTp::setTipsWithWidth(const QString &tips, int maxWidth, const QMargins &margins)
{
	if (!m_pLabel)
		return;
	QLayout* layout = this->layout();
	if (layout)
		layout->setContentsMargins(margins);
	// 取消高度限制
	m_pLabel->setMinimumHeight(_S(0));
	m_pLabel->setMaximumHeight(_S(1000));
	m_content = tips;
	m_pLabel->setText(m_content);
	QFontMetrics mc(FontDef::getFont());
	int width = mc.width(m_content) + _S(12) * 2;
	if (width > maxWidth)
	{
		// 让Label控件根据固定宽度自动计算高度
		m_pLabel->setWordWrap(true);
		m_pLabel->setFixedWidth(maxWidth);
		m_pLabel->adjustSize();
	}
	else
	{
		// 取消宽度限制
		m_pLabel->setMinimumWidth(_S(0));
		m_pLabel->setMaximumWidth(_S(maxWidth));
		m_pLabel->adjustSize();
	}
	int newWidth = m_pLabel->width() + margins.left() + margins.right();
	int newHeight = m_pLabel->height() + margins.top() + margins.bottom();
	setFixedSize(newWidth, newHeight);
	update();
}
void NavWidgetTp::setAncestorWidget(QWidget *ancestor)
{
	m_pAncestorWidget = ancestor;
	update();
}
void NavWidgetTp::setAncestorWidgetInterval(int x1, int y1, int x2, int y2)
{
	m_ancestorWidgetIntervalX1 = x1;
	m_ancestorWidgetIntervalY1 = y1;
	m_ancestorWidgetIntervalX2 = x2;
	m_ancestorWidgetIntervalY2 = y2;
	update();
}
void NavWidgetTp::unBindWidget()
{
	if (m_pWidget)
	{
		m_pWidget->removeEventFilter(this);
	}
}
void NavWidgetTp::checkBoundOfAncestorWidget(const QPoint& p)
{
	if (m_pAncestorWidget)
		fixPositionIfNecessary(p);
}
void NavWidgetTp::setSimpleFix(bool value)
{
	m_simpleFix = value;
}
void NavWidgetTp::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
	p.setBrush(QColor(m_backGroundColor));
	p.setPen(m_borderColor);
	//int w = rect().width();
	//int h = rect().height();
	p.drawRoundedRect(rect(), _S(4), _S(4));
	QWidget::paintEvent(event);
}
bool NavWidgetTp::eventFilter(QObject *o, QEvent *event)
{
	if (!m_pWidget || o != m_pWidget)
		return QWidget::eventFilter(o, event);
	switch (event->type())
	{
	case QEvent::Enter:
	{
		if (m_content.isEmpty())
			return QWidget::eventFilter(o, event);
		QPoint p = placeTip(m_tpPosition, m_interval, m_pWidget);
		if (m_pAncestorWidget)
			fixPositionIfNecessary(p);
		this->raise();
		this->show();
		break;
	}
	case QEvent::Leave:
		this->hide();
		break;
	default:
		break;
	}
	return QWidget::eventFilter(o, event);
}
QString NavWidgetTp::makeColorString(const QColor &color)
{
	return QString("rgb(%1, %2, %3)")
		.arg(color.red())
		.arg(color.green())
		.arg(color.blue());
}
void NavWidgetTp::updateTextColor()
{
	if (m_pLabel)
		m_pLabel->setStyleSheet(QString("QLabel{background:transparent;color:%1;border:none;}").arg(makeColorString(m_textColor)));
}
void NavWidgetTp::fixPositionIfNecessary(const QPoint &p)
{
	if (!m_pAncestorWidget || !m_pWidget)
		return;
	// 判断是否超过边界，理论上（左边和右边，上边和下边，分别最多只有一边越界）
	QRect ancestorRealRect = m_pAncestorWidget->rect().adjusted(m_ancestorWidgetIntervalX1, m_ancestorWidgetIntervalY1, m_ancestorWidgetIntervalX2, m_ancestorWidgetIntervalY2);
	QPoint ancestorTopLeft = m_pAncestorWidget->mapToGlobal(ancestorRealRect.topLeft());
	QPoint ancestorBottomRight = m_pAncestorWidget->mapToGlobal(ancestorRealRect.bottomRight());
	// 本来selfTopLeft想取this->mapToGlobal(this->rect().topLeft())的，但placeTip()里面this->move()并没有立即改变this的全局pos，所以获取的值不对，最终使用了placeTip的返回值。
	QPoint selfTopLeft = p;//this->mapToGlobal(this->rect().topLeft());
	QPoint selfBottomRight = p + QPoint(this->width(), this->height());//this->mapToGlobal(this->rect().bottomRight());
	if (m_simpleFix)
	{
		int finalX = p.x();
		int finalY = p.y();
		if (selfTopLeft.y() < ancestorTopLeft.y())
		{
			finalY += height();
		}
		else if (selfBottomRight.y() > ancestorBottomRight.y())
		{
			finalY -= height();
		}
		if (selfTopLeft.x() < ancestorTopLeft.x())
		{
			finalX = ancestorTopLeft.x();
		}
		else if (selfBottomRight.x() > ancestorBottomRight.x())
		{
			finalX = ancestorTopLeft.x() - width();
		}
		move(finalX, finalY);
		return;
	}
	if (selfTopLeft.y() < ancestorTopLeft.y())
	{
		// 上边越界
		if (m_tpPosition == EmTpPosition::TpTop)
		{
			m_tpPosition = EmTpPosition::TpBottom;
			placeTip(m_tpPosition, m_interval, m_pWidget);
		}
		else if (m_tpPosition != EmTpPosition::TpBottom)
		{
			QPoint pWidgetTopLeft = m_pWidget->mapToGlobal(m_pWidget->rect().topLeft());
			this->move(QPoint(p.x(), pWidgetTopLeft.y()));
		}
	}
	else if (selfBottomRight.y() > ancestorBottomRight.y())
	{
		// 下边越界
		if (m_tpPosition == EmTpPosition::TpBottom)
		{
			m_tpPosition = EmTpPosition::TpTop;
			placeTip(m_tpPosition, m_interval, m_pWidget);
		}
		else if (m_tpPosition != EmTpPosition::TpTop)
		{
			QPoint pWidgetBottomLeft = m_pWidget->mapToGlobal(m_pWidget->rect().bottomLeft());
			this->move(QPoint(p.x(), pWidgetBottomLeft.y()));
		}
	}
	if (selfTopLeft.x() < ancestorTopLeft.x())
	{
		// 左边越界
		if (m_tpPosition == EmTpPosition::TpLeft)
		{
			m_tpPosition = EmTpPosition::TpRight;
			placeTip(m_tpPosition, m_interval, m_pWidget);
		}
		else if (m_tpPosition != EmTpPosition::TpRight)
		{
			QPoint pWidgetTopLeft = m_pWidget->mapToGlobal(m_pWidget->rect().topLeft());
			this->move(QPoint(pWidgetTopLeft.x(), p.y()));
		}
	}
	else if (selfBottomRight.x() > ancestorBottomRight.x())
	{
		// 右边越界
		if (m_tpPosition == EmTpPosition::TpRight)
		{
			m_tpPosition = EmTpPosition::TpLeft;
			placeTip(m_tpPosition, m_interval, m_pWidget);
		}
		else if (m_tpPosition != EmTpPosition::TpLeft)
		{
			QPoint pWidgetTopRight = m_pWidget->mapToGlobal(m_pWidget->rect().topRight());
			this->move(QPoint(pWidgetTopRight.x() - this->width(), p.y()));
		}
	}
}
