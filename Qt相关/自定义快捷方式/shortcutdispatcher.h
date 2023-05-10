/*
 * @Description: 快捷方式
 * @Author: Ivy
 * @Date: 2022-05-10 09:34:43
 * @LastEditTime: 2022-05-10 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */


#pragma once
class ShortcutDispatcher : public QObject
{
	Q_OBJECT
public:
	static ShortcutDispatcher* instance();
	bool process(QKeyEvent *keyEvent);
signals:
	void sigShotCutSave();
	void sigShotCutSaveAs();
	void sigShotCutEnterPress();
	void sigShotCutReturnPress();
	void sigShotCutUndo();
	void sigShotCutRedo();
private:
	ShortcutDispatcher(QObject *parent);
	~ShortcutDispatcher();
	friend class SdRenderApplication;
	Q_DISABLE_COPY_MOVE(ShortcutDispatcher);
};
