#include "example .h"
/**
	实例演示：包含其头文件，直接用
*/

void showNameTip(bool visable)
{
	QLabel *pImageName = findChild<QLabel *>("pImageName");//设置监听
	CheckResult(pImageName);
	if (!m_pImageNameTips)
	{
		m_pImageNameTips = new NavWidgetTp(pImageName, pImageName->text());
		m_pImageNameTips->placeTip(EmTpPosition::TpBottom, _S(4), pImageName);
	}
	m_pImageNameTips->setVisible(visable);
}

bool eventFilter(QObject *watch, QEvent *event)
{
	if (watch == findChild<QLabel*>("pImageName"))
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

//父控件的文本设置省略号
QString newStrMsg = "1234567890abcdefghijklmnopqrstuvwxyz";

QFontMetrics fontWidth(pImageName->font());//得到每个字符的宽度
QString elideNote = fontWidth.elidedText(newStrMsg, Qt::ElideRight, 150);//最大宽度150像素

pImageName->setText(elideNote);//显示省略好的字符串
pImageName->setToolTip(newStrMsg);//设置tooltips



