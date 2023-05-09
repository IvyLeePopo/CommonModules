
#include "pch.h"
#include "imagebutton.h"
#include <QPainter>
#include "common/dpiscaled.h"
#include "common/colordef.h"
#include "common/fontdef.h"
#include "general/imageutils.h"
#include "ui/navwidgettips.h"
namespace
{
	const QMargins g_m_pixmapMargins = QMargins(16, 10, 16, 4);
	const QMargins g_textMargins = QMargins(16, 0, 16, 10);
}
ImageButton::ImageButton(const QPixmap& pixmap, const QString& text, QWidget* parent, const QSize& pixmapFixedSize)
	: QPushButton(parent)
	, m_text(text)
	, m_pixmapMargins(_SMM(g_m_pixmapMargins))
	, m_textMargins(_SMM(g_textMargins))
	, m_pixmapFixedSize(pixmapFixedSize)
{
	m_pixmap = pixmap.scaled(pixmapFixedSize);
	m_pixmapHover = m_pixmap;
	m_pixmapDisable = m_pixmap;
	if (text.isEmpty())
		m_textMargins = QMargins(0, 0, 0, 0);
}
ImageButton::ImageButton(const QString& svgNameNoExt, const QString& text, QWidget* parent, const QSize& pixmapFixedSize)
	: QPushButton(parent)
	, m_text(text)
	, m_pixmapMargins(_SMM(g_m_pixmapMargins))
	, m_textMargins(_SMM(g_textMargins))
	, m_pixmapFixedSize(pixmapFixedSize)
{
	m_pixmap = imageutils::getSvgResPixmapNomal(svgNameNoExt, m_pixmapFixedSize);
	m_pixmapHover = imageutils::getSvgResPixmapHover(svgNameNoExt, m_pixmapFixedSize);
	m_pixmapDisable = imageutils::getSvgResPixmapDisable(svgNameNoExt, m_pixmapFixedSize);
	if (text.isEmpty())
		m_textMargins = QMargins(0, 0, 0, 0);
}
QSize ImageButton::sizeHint() const
{
	QRect imageRect = pixmapRect();
	QRect textRect;
	if (!m_text.isEmpty())
	{
		QFontMetrics fm(getFontByState());
		textRect = fm.boundingRect(m_text);
		textRect = textRect.marginsAdded(m_textMargins);
	}
	int w = imageRect.width() > textRect.width() ? imageRect.width() : textRect.width();
	int h = imageRect.height() + textRect.height();
	return QSize(w, h);
}
void ImageButton::setPixmapMargins(const QMargins & margins)
{
	m_pixmapMargins = margins;
	update();
}
void ImageButton::setTextMargins(const QMargins & margins)
{
	m_textMargins = margins;
	update();
}
void ImageButton::showUnderLine(bool show)
{
	m_showUnderLine = show;
	update();
}
void ImageButton::setCanDropList(bool canDrop)
{
	m_canDropList = canDrop;
}
void ImageButton::setText(const QString& text)
{
	m_text = text;
}
void ImageButton::setPixmap(const QString& svgNameNoExt)
{
	m_pixmap = imageutils::getSvgResPixmapNomal(svgNameNoExt, m_pixmapFixedSize);
	m_pixmapHover = imageutils::getSvgResPixmapHover(svgNameNoExt, m_pixmapFixedSize);
	m_pixmapDisable = imageutils::getSvgResPixmapDisable(svgNameNoExt, m_pixmapFixedSize);
}
void ImageButton::updateTextPixmap(const QString& text, const QString& svgNameNoExt)
{
	setText(text);
	setPixmap(svgNameNoExt);
}
void ImageButton::setTipEnable()
{
	m_bEllipsis = true;
	this->installEventFilter(this);
}
QFont ImageButton::getFontByState() const
{
	FontDef::FontWeight fontWeight = FontDef::FontWeight::Regular;
	if (isChecked())
		fontWeight = FontDef::FontWeight::Semibold;
	QFont font = FontDef::getFont(FontDef::FontSize::T1, fontWeight);
	return font;
}
void ImageButton::enterEvent(QEvent *event)
{
	m_hover = true;
	QPushButton::enterEvent(event);
	update();
	emit sigHover(true);
}
void ImageButton::leaveEvent(QEvent *event)
{
	m_hover = false;
	QPushButton::leaveEvent(event);
	update();
	emit sigHover(false);
}
void ImageButton::mousePressEvent(QMouseEvent *e)
{
	m_pressed = true;
	QPushButton::mousePressEvent(e);
}
void ImageButton::mouseReleaseEvent(QMouseEvent *e)
{
	m_pressed = false;
	QPushButton::mouseReleaseEvent(e);
}
void ImageButton::paintEvent(QPaintEvent * event)
{
	Q_UNUSED(event);
	QPainter p(this);
	QRect textRect;
	QString elideNote = m_text;
	if (m_bEllipsis)
	{
		//int width1 = this->width();
		QFontMetrics fontWidth(getFontByState());
		elideNote = fontWidth.elidedText(m_text, Qt::ElideRight, width());
	}
	if (!elideNote.isEmpty())
	{
		QFont font = getFontByState();
		QFontMetrics fm(font);
		textRect = fm.boundingRect(elideNote);
	}
	const QPixmap& pixmap = getCurPixmap();
	QRect imageInnerRect;
	{
		int centralHeight = 0;
		if (!elideNote.isEmpty())
			centralHeight = m_pixmapFixedSize.height() + m_pixmapMargins.bottom() + textRect.height() + m_textMargins.top();
		else
			centralHeight = m_pixmapFixedSize.height();
		p.save();
		int x = (rect().width() - m_pixmapFixedSize.width()) / 2;
		int y = (rect().height() - centralHeight) / 2;
		imageInnerRect.moveTopLeft(QPoint(x, y));
		imageInnerRect.setSize(m_pixmapFixedSize);
		p.drawPixmap(imageInnerRect, pixmap);
		if (m_canDropList)
		{
			QPainterPath trianglePath;
			const int triangleWidth = _S(6);
			const int triangleMarginLeft = _S(8);
			QPoint p1(imageInnerRect.right() + triangleMarginLeft, imageInnerRect.bottom());
			QPoint p2(imageInnerRect.right() + triangleMarginLeft + triangleWidth, imageInnerRect.bottom());
			QPoint p3(imageInnerRect.right() + triangleMarginLeft + triangleWidth, imageInnerRect.bottom() - triangleWidth);
			trianglePath.moveTo(p1);
			trianglePath.lineTo(p2);
			trianglePath.lineTo(p3);
			trianglePath.lineTo(p1);
			p.setPen(Qt::NoPen);
			p.fillPath(trianglePath, getIconBrush());
		}
		p.restore();
	}
	if (!elideNote.isEmpty())
	{
		p.save();
		int x = (rect().width() - textRect.width()) / 2;
		int y = imageInnerRect.bottom() + m_pixmapMargins.bottom() + m_textMargins.top();
		textRect.moveTopLeft(QPoint(x, y));
		p.setPen(getTextColor());
		p.setFont(getFontByState());
		p.drawText(textRect, Qt::TextIncludeTrailingSpaces, elideNote);
		p.restore();
	}
	if (needShowUnderLine())
	{
		const int underLineHeight = _S(2);
		p.save();
		p.setPen(Qt::NoPen);
		p.setBrush(getUnderLineColor());
		p.drawRect(0, height() - underLineHeight, width(), height());
		p.restore();
	}
}
bool ImageButton::eventFilter(QObject *watch, QEvent *event)
{
	if (watch == this)
	{
		if (event->type() == QEvent::Enter)
		{
			showNameTip(true);
		}
		else if (event->type() == QEvent::Leave)
		{
			showNameTip(false);
		}
	}
	return QWidget::eventFilter(watch, event);
}
QRect ImageButton::pixmapRect() const
{
	QRect imageRect;
	imageRect.setSize(m_pixmapFixedSize);
	imageRect = imageRect.marginsAdded(m_pixmapMargins);
	imageRect.moveTopLeft(QPoint(0, 0));
	return imageRect;
}
const QPixmap& ImageButton::getCurPixmap() const
{
	if (!isEnabled())
		return m_pixmapDisable;
	if (isChecked() || m_hover || m_pressed)
		return m_pixmapHover;
	return m_pixmap;
}
bool ImageButton::needShowUnderLine() const
{
	return m_showUnderLine && isEnabled() && (isChecked() || m_hover || m_pressed);
}
QColor ImageButton::getTextColor() const
{
	if (!isEnabled())
		return QColor(ColorDef::clr_font_text_d);
	if (isChecked())
		return QColor(ColorDef::clr_font_text_c);
	if (m_hover)
		return QColor(ColorDef::clr_font_text_h);
	if (m_pressed)
		return QColor(ColorDef::clr_font_text_p);
	return QColor(ColorDef::clr_font_text_n);
}
QColor ImageButton::getUnderLineColor() const
{
	if (isChecked())
		return QColor(ColorDef::clr_primary_c);
	if (m_hover)
		return QColor(ColorDef::clr_primary_h);
	if (m_pressed)
		return QColor(ColorDef::clr_primary_p);
	return QColor();
}
QBrush ImageButton::getIconBrush() const
{
	if (!isEnabled())
		return QBrush(QColor(ColorDef::clr_icon_primary_d));
	if (isChecked() || m_hover || m_pressed)
	{
		QLinearGradient gradient(0, 0, rect().width(), 0);
		if (isChecked())
		{
			gradient.setColorAt(0, QColor(ColorDef::clr_icon_primary_c_L));
			gradient.setColorAt(1, QColor(ColorDef::clr_icon_primary_c_R));
		}
		else if (m_pressed)
		{
			gradient.setColorAt(0, QColor(ColorDef::clr_icon_primary_p_L));
			gradient.setColorAt(1, QColor(ColorDef::clr_icon_primary_p_R));
		}
		else if (m_hover)
		{
			gradient.setColorAt(0, QColor(ColorDef::clr_icon_primary_h_L));
			gradient.setColorAt(1, QColor(ColorDef::clr_icon_primary_h_R));
		}
		return QBrush(gradient);
	}
	return QBrush(QColor(ColorDef::clr_icon_text_n));
}
void ImageButton::showNameTip(bool visable)
{
	if (!m_pTips)
	{
		m_pTips = new NavWidgetTp(this, m_text);
		m_pTips->placeTip(EmTpPosition::TpBottom, _S(4), this);
	}
	m_pTips->setVisible(visable);
}
