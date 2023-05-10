/*
 * @Description: 真正的窗口类，QMainWindow，可以随意移动QDockWidget
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QHash>
#include <QProxyStyle>
#include <QDockWidget>

class QPushButton;
class ProgressBarStackTaskLoading;
class SdShadowBorder;
enum FoldSwitchSource
{
	FSS_Left,
	FSS_Right
};

class RenderMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit RenderMainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
	void initUi(QWidget* topWidget, QWidget* leftWidget, QWidget* rightWidget, QWidget* centralWidget);
	void setRightDockWidget(QWidget *widget);
	void showLoading(const QString &loadingText);
	void hideLoading(bool fillLastProgress = true);

public Q_SLOTS:
	void foldSwitch(FoldSwitchSource src, bool isFold);

Q_SIGNALS:
	// 折叠状态切换
	void sigFoldSwitch(FoldSwitchSource src, bool isFold);
	void sigDockTitleChange(const QString& text, FoldSwitchSource src);
	void sigChangeHeight(int height);

protected:
	virtual void resizeEvent(QResizeEvent *event) override;
private Q_SLOTS:
	void onDockTopLevelChanged(bool topLevel);
private:
	void initUiInner();
	void updateDockWidgetSizeLimit(QDockWidget* dockWidget, bool isFold);
private:
	QPointer<SdShadowBorder>        m_shadowBorder;
	QPointer<QDockWidget>           m_leftDock;
	QPointer<QDockWidget>           m_rightDock;
	QPointer<ProgressBarStackTaskLoading>   m_pTaskLoading;
};

class RenderMainWindowStyle : public QProxyStyle
{
	Q_OBJECT
		using Base = QProxyStyle;
public:
	explicit RenderMainWindowStyle(QStyle* style = nullptr);
protected:
	void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w = nullptr) const override;
	void drawControl(ControlElement element, const QStyleOption* option, QPainter* p, const QWidget* widget) const override;
};

class RenderMainWindowDockTitleBar : public QWidget
{
	Q_OBJECT
		using Base = QWidget;
public:
	explicit RenderMainWindowDockTitleBar(const QString& name, const QString& currentText, FoldSwitchSource src, QWidget* parent = nullptr);
	void foldSwitch(bool fold);
	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
Q_SIGNALS:
	void sigFoldSwitch(FoldSwitchSource src, bool isFold);
public Q_SLOTS:
	void onCurrentTextChanged(const QString& text, FoldSwitchSource src);
protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
private:
	void initUi();
	void foldSwitchUi();
	QPointer<QLabel> m_nameLabel;
	QPointer<QLabel> m_currentTextLabel;
	QPointer<QPushButton> m_foldBtn;
	QString m_name;
	QString m_currentText;
	FoldSwitchSource m_src = FoldSwitchSource::FSS_Left;
	bool m_fold = false;
};

class RenderMainWindowDockWidget : public QDockWidget
{
	Q_OBJECT
		using Base = QDockWidget;
public:
	explicit RenderMainWindowDockWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
protected:
	void paintEvent(QPaintEvent* event) override;
};
