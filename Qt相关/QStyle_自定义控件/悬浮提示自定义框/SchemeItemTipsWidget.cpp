
#include "SchemeItemTipsWidget.h"
#include <QRect>
#include <QLabel>

///////////////////////////////////////////////////////////////////////
///SchemeItemTipsWidget
SchemeItemTipsWidget::SchemeItemTipsWidget(QWidget *parent /*= nullptr*/)
	: QDialog(QApplication::desktop())
	, m_intervel(_S(0))
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::Tool);
	setAttribute(Qt::WA_ShowModal, false);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_TransparentForMouseEvents, true);
	setAttribute(Qt::WA_ShowWithoutActivating, true);
	QVBoxLayout* pLayout = new QVBoxLayout(this);
	//top:COL_TIPS_MARGIN_TOP
	pLayout->setContentsMargins(_SM(TIPS_MARGIN_LEFT, 0, TIPS_MARGIN_LEFT, 0));
	pLayout->setSpacing(8);
	QFont font = SdStyleFunc::getAppFont(_S(12));
	m_nameLabel = new QLabel(this);
	m_nameLabel->setFont(font);
	m_nameLabel->setStyleSheet(gNormalColorLabel.arg("#CCCCCC"));
	m_componentLabel = new QLabel(this);
	m_componentLabel->setFont(font);
	m_componentLabel->setStyleSheet(gNormalColorLabel.arg("#CCCCCC"));
	pLayout->addStretch();
	pLayout->addWidget(m_nameLabel);
	pLayout->addWidget(m_componentLabel);
	pLayout->addStretch();
	m_nameLabel->setVisible(false);
	m_componentLabel->setVisible(false);
}
SchemeItemTipsWidget::~SchemeItemTipsWidget()
{
}
void SchemeItemTipsWidget::clear()
{
	CheckResult(m_nameLabel);
	CheckResult(m_componentLabel);
	m_nameLabel->setVisible(false);
	m_nameLabel->setText("");
	m_componentLabel->setVisible(false);
	m_componentLabel->setText("");
}
void SchemeItemTipsWidget::show()
{
	if (!containsData())
		return;
	adjustWidth();
	adjustHeight();
	QPoint parentPos = m_parentRc.topLeft();
	// 默认显示在下方
	int offsetX = qAbs(m_parentRc.width() - this->width()) / 2;
	QPoint movePos = parentPos;
	if (m_parentRc.width() > this->width())
		movePos.setX(parentPos.x() + offsetX);
	else
		movePos.setX(parentPos.x() - offsetX);
	int offset = _S(6);
	movePos.setY(parentPos.y() + m_parentRc.height() + m_intervel + _S(offset));
	if (movePos.y() > (QApplication::desktop()->availableGeometry().height() - this->height() - m_intervel))
	{
		movePos.setY(parentPos.y() - this->height() - m_intervel + _S(offset));
	}
	this->move(movePos);
	QDialog::show();
}
bool SchemeItemTipsWidget::containsData()
{
	CheckResultWithRet(m_nameLabel, false);
	CheckResultWithRet(m_componentLabel, false);
	if (!m_nameLabel->text().isEmpty()
		|| !m_componentLabel->text().isEmpty())
		return true;
	return false;
}
void SchemeItemTipsWidget::setName(const QString &name)
{
	CheckResult(m_nameLabel);
	m_nameLabel->setText(name);
	m_nameLabel->setVisible(true);
}
void SchemeItemTipsWidget::setComponents(const QString &componentName)
{
	CheckResult(m_componentLabel);
	m_componentLabel->setText(componentName);
	m_componentLabel->setVisible(true);
}
void SchemeItemTipsWidget::setParentGrometry(const QRect &rc)
{
	m_parentRc = rc;
}
void SchemeItemTipsWidget::setShowIntervel(int intervel)
{
	m_intervel = intervel;
}
void SchemeItemTipsWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);
	p.setBrush(QColor("#1C1C1C"));
	p.setPen(QColor("#131314"));
	int w = rect().width();
	int h = rect().height();
	p.drawRoundedRect(rect(), _S(4), _S(4));
	QWidget::paintEvent(event);
}
void SchemeItemTipsWidget::adjustWidth()
{
	CheckResult(m_nameLabel);
	QLabel *m_componentLabel = findChild<QLabel *>("m_componentLabel");
	CheckResult(m_componentLabel);
	int nameWidth = SdStyleFunc::getTextWidth(m_nameLabel->font(), m_nameLabel->text());
	int componentWidth = SdStyleFunc::getTextWidth(m_componentLabel->font(), m_componentLabel->text());
	int max = qMax(nameWidth, componentWidth);
	setFixedWidth(max + _S(TIPS_MARGIN_LEFT) * 2);
}
void SchemeItemTipsWidget::adjustHeight()
{
	CheckResult(m_nameLabel);
	CheckResult(m_componentLabel);
	int visibleNum = 0;
	if (!m_nameLabel->text().isEmpty())
		visibleNum++;
	if (!m_componentLabel->text().isEmpty())
		visibleNum++;
	int h = visibleNum * m_nameLabel->fontMetrics().height() + _S(TIPS_MARGIN_TOP) * 2 + _S(16);
	setFixedHeight(h);
}
