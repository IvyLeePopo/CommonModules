/*
 * @Description: 悬浮提示Tip
 * @Author: Ivy
 * @Date: 2022-05-06 09:34:43
 * @LastEditTime: 2022-05-06 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#pragma once
#include <QDialog>
#include <QEvent>
#include <QHelpEvent>
#include <QPainter>
#include <QPoint>
#include <QTimer>
#include <QVBoxLayout>
#include <QPointer>
#include "common/sdstylefunc.h"
#include "general/genericdef.h"
class NavWidgetTp : public QDialog
{
	Q_OBJECT
public:
	NavWidgetTp(QWidget* bindWidget, const QString& content);
	~NavWidgetTp();
	QPoint placeTip(EmTpPosition position, int interval, QWidget* w);
	QPoint placeTip(EmTpPosition position, int interval, QRect widgetRect);
	void setTipColor(const QColor &bgColor, const QColor &textColor);
	void setBorderColor(const QColor &borderColor);
	void setTipInterval(int interval);
	void setTipPosition(EmTpPosition position);
	void setTips(const QString &tips);
	void setTipsWidthLineBreak(const QString &tips);
	void setTipsWithWidth(const QString &tips, int maxWidth, const QMargins &margins);
	void hideTips();
	void setAncestorWidget(QWidget *ancestor);
	void setAncestorWidgetInterval(int x1, int y1, int x2, int y2);
	void unBindWidget();
	void checkBoundOfAncestorWidget(const QPoint& p);
	void setSimpleFix(bool value);
protected:
	void paintEvent(QPaintEvent *event) override;
	bool eventFilter(QObject *o, QEvent *event) override;
private:
	// 目前不支持透明度
	QString makeColorString(const QColor &color);
	void    updateTextColor();
	void    fixPositionIfNecessary(const QPoint &pos);
	QPointer<QWidget> m_pWidget;
	QString         m_content;
	QColor          m_textColor;
	QColor          m_backGroundColor;
	QColor          m_borderColor;
	QLabel          *m_pLabel;
	EmTpPosition    m_tpPosition;
	int             m_interval;
	QPointer<QWidget> m_pAncestorWidget;
	int             m_ancestorWidgetIntervalX1;
	int             m_ancestorWidgetIntervalY1;
	int             m_ancestorWidgetIntervalX2;
	int             m_ancestorWidgetIntervalY2;
	bool            m_simpleFix = false;
};
