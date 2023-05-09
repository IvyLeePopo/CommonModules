/*
 * @Description: 自定义按钮-带图片
 * @Author: Ivy
 * @Date: 2022-05-06 09:34:43
 * @LastEditTime: 2022-05-06 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#pragma once
#include <QPushButton>
#include "common/dpiscaled.h"
class NavWidgetTp;
class ImageButton : public QPushButton
{
	Q_OBJECT
public:
	explicit ImageButton(const QPixmap& pixmap, const QString& text = QString(), QWidget* parent = nullptr, const QSize& pixmapFixedSize = _SS(20, 20));
	explicit ImageButton(const QString& svgNameNoExt, const QString& text = QString(), QWidget* parent = nullptr, const QSize& pixmapFixedSize = _SS(20, 20));
	void setPixmapMargins(const QMargins& margins);
	void setTextMargins(const QMargins& margins);
	void showUnderLine(bool show);
	void setCanDropList(bool canDrop);
	void setText(const QString& text);
	void setPixmap(const QString& svgNameNoExt);
	void updateTextPixmap(const QString& text, const QString& svgNameNoExt);
	void setTipEnable();

public:
	virtual QSize sizeHint() const final;
signals:
	void sigHover(bool hover);
private:
	void enterEvent(QEvent *event) override;
	void leaveEvent(QEvent *event) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void paintEvent(QPaintEvent* event) final;
	bool eventFilter(QObject *watch, QEvent *event) override;
private:
	QRect pixmapRect() const;
	bool needShowUnderLine() const;
	QFont getFontByState() const;
	const QPixmap& getCurPixmap() const;
	QColor getTextColor() const;
	QColor getUnderLineColor() const;
	QBrush getIconBrush() const;
	void showNameTip(bool visable);
private:
	bool m_hover = false;
	bool m_pressed = false;
	QString m_text;
	QPixmap m_pixmap;
	QPixmap m_pixmapHover;
	QPixmap m_pixmapDisable;
	QSize m_pixmapFixedSize;
	QMargins m_textMargins;
	QMargins m_pixmapMargins;
	bool m_canDropList = false;
	bool m_showUnderLine = false;
	QPointer<NavWidgetTp> m_pTips;
	bool m_bEllipsis = false;
};
