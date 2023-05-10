 
#include "pch.h"
#include "rendermainwindow.h"
#include <QMenuBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QPainter>
#include "common/dpiscaled.h"
#include "common/colordef.h"
#include "common/fontdef.h"
#include "common/sdstylesheet.h"
#include "ui/loadingwidget/taskloadingwidget.h"
#include "ui/sdshadowborder.h"
#include "general/imageutils.h"
namespace
{
    const char* g_propDockSrc = "dockSrc";
    const char* g_propFold = "isFold";
    const int g_leftRightWidthMin = 300;
    const int g_leftRightWidthMax = 588;
}
RenderMainWindow::RenderMainWindow(QWidget* parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    initUiInner();
}
void RenderMainWindow::onDockTopLevelChanged(bool topLevel)
{
    Q_UNUSED(topLevel);
    auto dock = qobject_cast<QDockWidget*>(sender());
    if (!dock)
        return;
    bool isFold = dock->property(g_propFold).toBool();
    updateDockWidgetSizeLimit(dock, isFold);
}
void RenderMainWindow::initUiInner()
{
    m_pTaskLoading = new ProgressBarStackTaskLoading(this);
    m_pTaskLoading->setText(tr("processing,please waiting..."));
    m_pTaskLoading->setVisible(false);
    stackUnder(m_pTaskLoading);
    m_shadowBorder = new SdShadowBorder(this, this, true, _S(16));
    setContextMenuPolicy(Qt::NoContextMenu);
    setStyleSheet(QString(
        R"(
            %1{background-color:%2;}
            QMainWindow::separator {
                background: %3;
                width: %4px;
                height: %4px;
            }
        )"
    ).arg(metaObject()->className()).arg(ColorDef::clr_page_bg).arg(ColorDef::clr_line).arg(_S(1)));
    setStyle(new RenderMainWindowStyle(style()));
}
void RenderMainWindow::updateDockWidgetSizeLimit(QDockWidget* dockWidget, bool isFold)
{
    if (isFold)
    {
        dockWidget->setFixedWidth(_S(40));
    }
    else
    {
        dockWidget->setMinimumWidth(_S(g_leftRightWidthMin));
        if (dockWidget->isFloating())
            dockWidget->setMaximumWidth(QWIDGETSIZE_MAX);
        else
            dockWidget->setMaximumWidth(_S(g_leftRightWidthMax));
    }
}
 
void RenderMainWindow::setRightDockWidget(QWidget *widget)
{
    if (!m_rightDock)
    {
        m_rightDock = new RenderMainWindowDockWidget(this);
        m_rightDock->setStyleSheet(QString("QDockWidget {border: 1px solid %1;}").arg(ColorDef::clr_page_bg));
        m_rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
        m_rightDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        m_leftDock->setProperty(g_propDockSrc, FSS_Right);
        auto rightTitleBar = new RenderMainWindowDockTitleBar(tr("Project"), tr("Component"), FSS_Right, m_rightDock);
        connect(rightTitleBar, &RenderMainWindowDockTitleBar::sigFoldSwitch, this, &RenderMainWindow::foldSwitch);
        connect(this, &RenderMainWindow::sigDockTitleChange, rightTitleBar, &RenderMainWindowDockTitleBar::onCurrentTextChanged);
        m_rightDock->setTitleBarWidget(rightTitleBar);
        addDockWidget(Qt::RightDockWidgetArea, m_rightDock);
        connect(m_rightDock, &QDockWidget::topLevelChanged, this, &RenderMainWindow::onDockTopLevelChanged);
    }
    QWidget *lastWidget = m_rightDock->widget();
    if (lastWidget != widget)
    {
        m_rightDock->setWidget(widget);
    }
    
    foldSwitch(FSS_Right, false);
}
void RenderMainWindow::showLoading(const QString &str)
{
    CheckResult(m_pTaskLoading);
    if (!str.isEmpty())
        m_pTaskLoading->setText(str);
    QPoint pt = mapToGlobal(QPoint(0, 0));
    m_pTaskLoading->setRect(rect());
    m_pTaskLoading->move(pt);
    m_pTaskLoading->raise();
    m_pTaskLoading->setVisible(true);
    m_pTaskLoading->setLoadingState(true);
}
void RenderMainWindow::hideLoading(bool fillLastProgress)
{
    CheckResult(m_pTaskLoading);
    m_pTaskLoading->setVisible(false, fillLastProgress);
    m_pTaskLoading->setLoadingState(false);
}
void RenderMainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_pTaskLoading && m_pTaskLoading->isVisible())
    {
        QPoint pt = mapToGlobal(QPoint(0, 0));
        m_pTaskLoading->setRect(rect());
        m_pTaskLoading->move(pt);
    }
    emit sigChangeHeight(height());
}
void RenderMainWindow::initUi(QWidget* topWidget, QWidget* leftWidget, QWidget* rightWidget, QWidget* centralWidget)
{
    if (topWidget)
    {
        QDockWidget* topDock = new QDockWidget(this);
        topDock->setAllowedAreas(Qt::TopDockWidgetArea);
        topDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        topDock->setTitleBarWidget(new QWidget(topDock));
        topDock->setWidget(topWidget);
        topDock->setFixedHeight(_S(88));
        addDockWidget(Qt::TopDockWidgetArea, topDock);
    }
    if (leftWidget)
    {
        m_leftDock = new RenderMainWindowDockWidget(this);
        m_leftDock->setStyleSheet(QString("QDockWidget {border: 1px solid %1;}").arg(ColorDef::clr_page_bg));
        m_leftDock->setAllowedAreas(Qt::LeftDockWidgetArea);
        m_leftDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        m_leftDock->setProperty(g_propDockSrc, FSS_Left);
        auto leftTitleBar = new RenderMainWindowDockTitleBar(tr("Asset"), tr("Model"), FSS_Left, m_leftDock);
        connect(leftTitleBar, &RenderMainWindowDockTitleBar::sigFoldSwitch, this, &RenderMainWindow::foldSwitch);
        connect(this, &RenderMainWindow::sigDockTitleChange, leftTitleBar, &RenderMainWindowDockTitleBar::onCurrentTextChanged);
        m_leftDock->setTitleBarWidget(leftTitleBar);
        m_leftDock->setWidget(leftWidget);
        foldSwitch(FSS_Left, false);
        addDockWidget(Qt::LeftDockWidgetArea, m_leftDock);
        connect(m_leftDock, &QDockWidget::topLevelChanged, this, &RenderMainWindow::onDockTopLevelChanged);
    }
    if (rightWidget)
    {
        setRightDockWidget(rightWidget);
    }
    
    if (centralWidget)
        setCentralWidget(centralWidget);
}
void RenderMainWindow::foldSwitch(FoldSwitchSource src, bool isFold)
{
    QDockWidget* dock = nullptr;
    switch (src)
    {
    case FSS_Left:
        dock = m_leftDock;
        break;
    case FSS_Right:
        dock = m_rightDock;
        break;
    default:
        break;
    }
    if (dock)
    {
        dock->setProperty(g_propFold, isFold);
        updateDockWidgetSizeLimit(dock, isFold);
        auto titleBar = qobject_cast<RenderMainWindowDockTitleBar*>(dock->titleBarWidget());
        if (titleBar)
            titleBar->foldSwitch(isFold);
    }
    Q_EMIT sigFoldSwitch(src, isFold);
}
RenderMainWindowStyle::RenderMainWindowStyle(QStyle* style)
    : Base(style)
{
}
void RenderMainWindowStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption * opt, QPainter * p, const QWidget * w) const
{
    Base::drawPrimitive(pe, opt, p, w);
}
void RenderMainWindowStyle::drawControl(ControlElement element, const QStyleOption * option, QPainter * p, const QWidget * widget) const
{
    if (element == QStyle::CE_RubberBand)
    {
        if (qstyleoption_cast<const QStyleOptionRubberBand*>(option))
        {
            p->save();
            QColor borderColor = ColorDef::clr_primary_c;
            QColor bgColor = ColorDef::combineAlphaF(0.10, ColorDef::clr_primary_c);
            p->setPen(borderColor);
            p->setBrush(bgColor);
            p->drawRect(option->rect.adjusted(0, 0, -1, -1));
            p->restore();
            return;
        }
    }
    Base::drawControl(element, option, p, widget);
}
RenderMainWindowDockTitleBar::RenderMainWindowDockTitleBar(const QString& name, const QString& currentText, FoldSwitchSource src, QWidget* parent)
    : Base(parent)
    , m_name(name)
    , m_currentText(currentText)
    , m_src(src)
{
    initUi();
}
void RenderMainWindowDockTitleBar::foldSwitch(bool fold)
{
    m_fold = fold;
    foldSwitchUi();
}
QSize RenderMainWindowDockTitleBar::sizeHint() const
{
    return minimumSizeHint();
}
QSize RenderMainWindowDockTitleBar::minimumSizeHint() const
{
    return QSize(_S(40), _S(20));
}
void RenderMainWindowDockTitleBar::onCurrentTextChanged(const QString& text, FoldSwitchSource src)
{
    if (m_src == src && m_currentTextLabel)
    {
        m_currentTextLabel->setText(text);
        update();
    }
}
void RenderMainWindowDockTitleBar::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    QDockWidget* dock = qobject_cast<QDockWidget*>(parentWidget());
    bool isFloating = dock ? dock->isFloating() : false;
    QPainter p(this);
    QRect r = rect();
    p.setPen(Qt::NoPen);
    if (isFloating)
    {
        p.setBrush(QColor(ColorDef::clr_page_bg));
        p.drawRect(r);
    }
    
    p.setBrush(QColor(ColorDef::combineAlphaF(0.10, ColorDef::clr_content_bg)));
    p.drawRect(r);
    p.setPen(QColor(ColorDef::clr_line));
    p.drawLine(r.bottomLeft(), r.bottomRight());
}
void RenderMainWindowDockTitleBar::mouseDoubleClickEvent(QMouseEvent * event)
{
    Q_UNUSED(event);
    // 什么也不做，避免QDockWidget监听到titleBar双击事件而切换悬浮状态
}
void RenderMainWindowDockTitleBar::initUi()
{
    setStyleSheet(QString("%1{border:none;}").arg(metaObject()->className()));
    QHBoxLayout* thisLayout = new QHBoxLayout(this);
    thisLayout->setContentsMargins(_SM(0, 0, 0, 0));
    
    m_nameLabel = new QLabel(this);
    m_nameLabel->setFixedWidth(minimumSizeHint().width());
    m_nameLabel->setText(m_name);
    m_nameLabel->setStyleSheet(QString("color:%1; padding-left:%2")
        .arg(ColorDef::clr_font_text_n)
        .arg(_S(11)));
    m_nameLabel->setFont(FontDef::getFont(FontDef::FontSize::T1, FontDef::FontWeight::Regular));
    m_currentTextLabel = new QLabel(this);
    m_currentTextLabel->setText(m_currentText);
    m_currentTextLabel->setStyleSheet(QString("color:%1").arg(ColorDef::clr_font_text_n));
    m_currentTextLabel->setFont(FontDef::getFont(FontDef::FontSize::T1, FontDef::FontWeight::Regular));
    m_foldBtn = new QPushButton(this);
    m_foldBtn->setFixedSize(_SS(16, 16));
    m_foldBtn->setCheckable(true);
    m_foldBtn->setChecked(m_fold);
    if (m_src == FSS_Left)
    {
        m_foldBtn->setStyleSheet(gCommonBtnThreeStateQSS.arg(imageutils::getSvgResPathNomal("fold_left"))
            .arg(imageutils::getSvgResPathHover("fold_left"))
            .arg(imageutils::getSvgResPathCliecked("fold_left")));
    }
    else
    {
        m_foldBtn->setStyleSheet(gCommonBtnThreeStateQSS.arg(imageutils::getSvgResPathNomal("fold_right"))
            .arg(imageutils::getSvgResPathHover("fold_right"))
            .arg(imageutils::getSvgResPathCliecked("fold_right")));
    }
    thisLayout->addWidget(m_nameLabel);
    thisLayout->addStretch();
    thisLayout->addWidget(m_currentTextLabel);
    thisLayout->addStretch();
    thisLayout->addWidget(m_foldBtn);
    thisLayout->addSpacing(_S(12));
    connect(m_foldBtn, &QPushButton::clicked, this, [=](bool checked) {
        m_fold = checked;
        m_foldBtn->setVisible(!m_fold);
        Q_EMIT sigFoldSwitch(m_src, checked);
    });
}
void RenderMainWindowDockTitleBar::foldSwitchUi()
{
    if (m_foldBtn)
    {
        m_foldBtn->setChecked(m_fold);
        m_foldBtn->setVisible(!m_fold);
    }
    if (m_currentTextLabel)
        m_currentTextLabel->setVisible(!m_fold);
}
RenderMainWindowDockWidget::RenderMainWindowDockWidget(QWidget * parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
}
void RenderMainWindowDockWidget::paintEvent(QPaintEvent* event)
{
    // QDockWidget只在使用默认标题栏时才绘制边框，因此重写paintEvent
    if (isFloating())
    {
        QStylePainter p(this);
        QStyleOptionFrame framOpt;
        framOpt.init(this);
        p.drawPrimitive(QStyle::PE_FrameDockWidget, framOpt);
        return;
    }
    Base::paintEvent(event);
}
