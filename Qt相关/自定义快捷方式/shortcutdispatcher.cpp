
#include "pch.h"
#include "shortcutdispatcher.h"

QPointer<ShortcutDispatcher> g_shortCutMgr;

ShortcutDispatcher* ShortcutDispatcher::instance()
{
	Q_ASSERT_X(g_shortCutMgr, "instance", "should call new ShortcutsManager in renderapplication first");
	return g_shortCutMgr;
}

bool ShortcutDispatcher::process(QKeyEvent *keyEvent)
{
	if (!keyEvent)
		return false;
	QWidget *curWidget = QApplication::focusWidget();
	if (!curWidget
		|| !curWidget->metaObject()
		|| strcmp(curWidget->metaObject()->className(), "QLineEdit") != 0)
	{
		if (keyEvent->key() == Qt::Key_S && keyEvent->modifiers() == Qt::ControlModifier)
		{
			emit sigShotCutSave();
			return true;
		}
		if (keyEvent->key() == Qt::Key_S&& keyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
		{
			emit sigShotCutSaveAs();
			return true;
		}
		if (keyEvent->key() == Qt::Key::Key_Enter)
		{
			emit sigShotCutEnterPress();
			return true;
		}
		if (keyEvent->key() == Qt::Key::Key_Return)
		{
			emit sigShotCutReturnPress();
			return true;
		}
		if (keyEvent->key() == Qt::Key_Z && keyEvent->modifiers() == Qt::ControlModifier)
		{
			emit sigShotCutUndo();
			return true;
		}
		if (keyEvent->key() == Qt::Key_Z && keyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
		{
			emit sigShotCutRedo();
			return true;
		}
	}
	return false;
}

ShortcutDispatcher::ShortcutDispatcher(QObject *parent)
	: QObject(parent)
{
	g_shortCutMgr = this;
}

ShortcutDispatcher::~ShortcutDispatcher()
{
}
