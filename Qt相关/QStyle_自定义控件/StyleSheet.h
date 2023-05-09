/*
 * @Description: 常用的空间的样式
 * @Author: Ivy
 * @Date: 2022-04-25 09:34:43
 * @LastEditTime: 2023-02-25 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

namespace styleSheetUtils
{
	//滚动条
	QString gScrollbarQSS = QString(
		"QScrollBar:vertical{width:%0px;background: #292929;margin:0px,0px,0px,0px;}"
		"QScrollBar::sub-page:vertical {background: #292929;}"/*修改滚动区域上部分的背景色*/
		"QScrollBar::add-page:vertical {background: #292929;}"/*修改滚动区域下部分的背景色*/
		"QScrollBar::handle:vertical{width:%0px;border-radius:%1px;background:#3c3c3c;}"/*修改滚动条正常状态的背景色*/
		"QScrollBar::handle:vertical:hover{width:%0px;border-radius:%1px;background:#3c3c3c;}"/*修改滚动条hover状态的背景色*/
		"QScrollBar::sub-line:vertical{background-color:#292929; border:none;height:0px;}"
		"QScrollBar::add-line:vertical{background-color:#292929; border:none;height:0px;}"
	);

	//QListWidget
	QListWidget
	{
		setMovement(Static);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setAcceptDrops(true);
		setAutoScroll(false);
		setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		setObjectName("DesignSchemeGroupListWidget");
		setStyleSheet("QListWidget#DesignSchemeGroupListWidget{border:none;background:transparent;outline:none;}"\
			"QListWidget#DesignSchemeGroupListWidget:focus{border:none;background:transparent;outline:none;}"
			"QListWidget#DesignSchemeGroupListWidget::item:selected{border:none;background:transparent;outline:none;}"\
			"QListWidget#DesignSchemeGroupListWidget::item:focus{border:none;background:transparent;outline:none;}");
		verticalScrollBar()->setStyleSheet(gScrollbarQSS.arg(_S(4)).arg(_S(3)));//滚动条的样式
	}

	//QPushButton
	QString gCommonCheckBtnThreeStateQSS = QString(
		"QPushButton{border-image: url(%0);border:none;}"\
		"QPushButton:hover{border-image: url(%1);}"\
		"QPushButton:pressed{border-image: url(%2);}"\
		"QPushButton:checked{border-image: url(%3);}"
	);

	//QLineEdit
	QString gLineEditQSS = QString(
		"QLineEdit{border:1px solid %0;border-radius:%4px %4px;background-color:#1c1c1c;color:#999999;font-family:Microsoft YaHei;padding-left:%3px;selection-background-color: #2878FF;}"
		"QLineEdit:hover{border:1px solid %1;border-radius:%4px %4px;background-color:#2c2c2c;color:#999999;font-family:Microsoft YaHei;padding-left:%3px;}"
		"QLineEdit:focus{border:1px solid %2;border-radius:%4px %4px;background-color:#262626;color:#999999;font-family:Microsoft YaHei;padding-left:%3px;}"
		"QLineEdit:read-only {background: #2c2c2c;color:#444444;}"
	);

	//QCheckBox
	QString gCheckboxQSS = QString(
		"QCheckBox{color:%3;}"
		"QCheckBox::indicator:unchecked{image:url(:/%0/%1.png);}"
		"QCheckBox::indicator:checked{image:url(:/%0/%2.png);}"
		"QCheckBox::indicator:unchecked:disabled{border-image: url(%2);}"
		"QCheckBox::indicator:checked:disabled{border-image: url(%3);}"
	);

	//QRadioButton
	QString gRadioBtnQSS = QString(
		"QRadioButton{color:%5;}"
		"QRadioButton::indicator:unchecked{border-image: url(%0);}"
		"QRadioButton::indicator:checked{border-image: url(%1);}"
		"QRadioButton::indicator:unchecked:disabled{border-image: url(%2);}"
		"QRadioButton::indicator:checked:disabled{border-image: url(%3);}"
	);

	//QMenu
	QString g_cmnMenuQSS = QString(
		"QMenu{background-color: #FFFFFF;border-radius: %0px;font-size:%1px;}"
		"QMenu::item{background-color: #FFFFFF;color:#333333;padding-left:%6px;height:%2px;}"
		"QMenu::item:hover{background-color:#FFFFFF;color:#2878FF;height:%2px;}"
		"QMenu::item:selected{background-color:#FFFFFF;color:#2878FF;height:%2px;}"
		"QMenu::right-arrow{image:url(:/%3/%4.png);}"
		"QMenu::right-arrow:hover{image:url(:/%3/%5.png);}"
	);

	//QComboBox
	QString gStrComboBoxCommonQSS = QString(
		"QComboBox{border:1px solid #DDDDDD;background-color:#2C2C2C;\
                                      border-radius:%2px %2px;padding:%2px %3px;}"
		"QComboBox::down-arrow{image:url(:/%0/%1.png);}"
		"QComboBox QAbstractItemView::item{height:%4px;outline:0px;}"
		"QComboBox::drop-down{border:0px;border-radius:%2px %2px;}"
	);

	//QComboBox
	QString gUploadMaterialBox = QString(
		"QComboBox{border:1px solid #DDDDDD;background-color:#FFFFFF;\
     border-radius:%2px %2px;padding:%2px %3px;}"
		"QComboBox::down-arrow{image:url(:/%0/%1.png);}"
		"QComboBox QAbstractItemView::item{height:%4px;outline:0px solid #DDDDDD; border: 0px solid #DDDDDD;}"
		"QComboBox QAbstractItemView::item:selected{background-color: #FFFFFF;}"
		"QComboBox::drop-down{border:0px;border-radius:%2px %2px;}"
		"QScrollBar:vertical {margin: 13px 0px 13px 0px;background-color: #FFFFFF; border: 0px; width: 12px;}\
                                    QScrollBar::handle:verTIcal {background-color: #CACED5; width: 12px;border-radius:5px;}\
                                    QScrollBar::add-page:verTIcal {background-color: #FFFFFF;}\
                                    QScrollBar::sub-page:verTIcal {background-color: #FFFFFF;}\
                                    QScrollBar::sub-line:vertical {subcontrol-origin: margin; border: 1px solid #FFFFFF; height:0px;}\
                                    QScrollBar::up-arrow:vertical {background-color: #FFFFFF; subcontrol-origin: margin;  height:13px;}\
                                    QScrollBar::down-arrow:vertical {background-color: #FFFFFF;}\
                                    QScrollBar::add-line:vertical {background-color: #FFFFFF;subcontrol-origin: margin; border: 1px solid #FFFFFF; height:0px}"
	);

};


