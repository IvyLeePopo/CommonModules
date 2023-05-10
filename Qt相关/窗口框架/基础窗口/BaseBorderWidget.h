/*
 * @Description: 有阴影边框的基础窗口
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once
#include <QWidget>
#include "basepopupdialog.h"

class CustomShadowEffect;

class BaseBorderWidget : public BasePopupWidget
{
	Q_OBJECT
public:
	BaseBorderWidget(int tickness, int radius = 0, QWidget *parent = nullptr);
	~BaseBorderWidget();
	void setShadowTickness(int tickness);
	void setShadowRadius(int radius);

protected:
	virtual void moveEvent(QMoveEvent *event);
	virtual void resizeEvent(QResizeEvent *event);
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
	virtual void showEvent(QShowEvent *event) override;
	virtual void paintEvent(QPaintEvent *event) override;
	virtual bool eventFilter(QObject *o, QEvent *e) override;

public slots:
	void onMinimize();
	void onMaximize();

private:
	// 自定义阴影
	CustomShadowEffect* m_pShadowEffect;
};

/**************************** CustomShadowEffect **********************************/
class CustomShadowEffect : public QWidget
{
public:
	CustomShadowEffect(int tickness, int radius, QWidget* attachWid, QWidget* parent = nullptr);
	~CustomShadowEffect();
	void setShadowTickness(int tickness) { m_nTickness = tickness; }
	void setShadowRadius(int radius) { m_nRadius = radius; }
	void updateGeometry();
protected:
	void paintEvent(QPaintEvent *event) override;
private:
	int          m_nTickness;
	int          m_nRadius;
	QWidget*     m_pAttachWid;
};
