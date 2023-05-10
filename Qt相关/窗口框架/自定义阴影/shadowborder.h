/*
 * @Description: 自定义阴影类ShadowBorder
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#include <QDialog>
#include <QTimer>
#include <QPointer>

#define Q_OS_WIN 1

class ShadowBorder : public QDialog
{
	Q_OBJECT
public:
	enum TYPE_BORDER
	{
		SZ_NOTHING = 0x0000,
		SZ_SIZElEFT = 0xf001,
		SZ_SIZERIGHT = 0xf002,
		SZ_SIZETOP = 0xf003,
		SZ_SIZETOPLEFT = 0xf004,
		SZ_SIZETOPRIGHT = 0xf005,
		SZ_SIZEBOTTOM = 0xf006,
		SZ_SIZEBOTTOMLEFT = 0xf007,
		SZ_SIZEBOTTOMRIGHT = 0xf008,
	};

	ShadowBorder(QWidget *hostWidget, QWidget *parent = NULL, bool bNeedResize = true, int nThickness = 16);
	void setNeedResize(bool b);
	void setChangeZOrder(bool value);
	void setColorMultiplier(float fMultiplier);
	void reset();

protected:
	bool eventFilter(QObject *obj, QEvent *e);
	void paintEvent(QPaintEvent *e);
	void leaveEvent(QEvent *event);

#ifdef Q_OS_WIN
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
	bool winEvent(MSG *message, long *result);
	void changeEvent(QEvent *event);
#endif

	QWidget* host() const;
	int borderThickness() const;
	void setBorderThickness(int nThickness);
	void delayHandleShowEvent();
	void resetGeometry();
	virtual bool isNeedHide() const;
	void makesureBorderShow();

private slots:
	void onDelayedShowTimer();
	void onMakesureBorderShowTimer();

private:
	bool onWndNCHittest(MSG *msg, long *result);
	bool onWndNCLButtonDown(MSG *msg, long *result);

private:
	QPointer<QWidget> m_host;
	TYPE_BORDER  m_pressedType;
	TYPE_BORDER  m_hoverType;
	uint m_borderThickness;
	float m_fColMutiplier;
	bool m_bNeedResize;
	bool m_bChangeZOrder;
	QTimer m_delayedInitShowTimer;
	// 增加个定时器确保阴影边框显示在父窗口那里，因为有部分情况阴影会没有附加到父窗口上
	QTimer m_makesureShowBorder;
};
