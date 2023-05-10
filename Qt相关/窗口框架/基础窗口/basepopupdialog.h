/*
 * @Description: 最BaseWidget
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

 //BasePopupDialog 基础控件


#pragma once

#define Q_OS_WIN 1

#include <QDialog>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
class BasePopupDialog : public QDialog
{
	Q_OBJECT
public:
	BasePopupDialog(QWidget* parent = nullptr);
	~BasePopupDialog();
private:
	bool wndIsVisible();
	void rePosWnd();
	QWidget *getAncestorWidget();
	WId getAncestorWidgetId();
private:
#ifdef Q_OS_WIN
	static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,
		HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
#endif
protected:
	virtual bool eventFilter(QObject *o, QEvent *e)override;
	virtual bool nativeEvent(const QByteArray &eventType, void *message,
		long *result) override;
private:
	//检查显示器大小用的,防止窗口在某些情况下被移动到屏幕不可见区域
	bool m_bFirstCheckMainMonitor;
#ifdef Q_OS_WIN
	QList<RECT> m_listMonitorRect;
#endif
};

class WindowStyle;
class CustomShadowEffect;

class BasePopupWidget : public QWidget
{
	Q_OBJECT
public:
	BasePopupWidget(QWidget* parent = nullptr);
	~BasePopupWidget();
	void setResizable(bool b);
	void setPopupFlags(bool b);
private:
	bool wndIsVisible();
	void rePosWnd();
	QWidget *getAncestorWidget();
	WId getAncestorWidgetId();
private:
#ifdef Q_OS_WIN
	static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor,
		HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
#endif
protected:
	virtual bool eventFilter(QObject *o, QEvent *e)override;
	virtual bool nativeEvent(const QByteArray &eventType, void *message,
		long *result) override;
private:
	//检查显示器大小用的,防止窗口在某些情况下被移动到屏幕不可见区域
	bool m_bFirstCheckMainMonitor;
	WindowStyle*  m_resizeObject;
#ifdef Q_OS_WIN
	QList<RECT> m_listMonitorRect;

#endif
};
