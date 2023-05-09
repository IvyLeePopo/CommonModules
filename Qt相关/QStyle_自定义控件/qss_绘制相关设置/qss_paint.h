/*
 * @Description: qss或者绘制相关设置
 * @Author: Ivy
 * @Date: 2022-05-06 09:34:43
 * @LastEditTime: 2022-05-06 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

//1、QWidget使用setStyleSheet设置样式表不生效
void qssNotWork();

//2、在paintEvent中绘制图标
void paintIconInPaintEvent();

//3、给窗口设置QSS，加载qss文件路径
void setWidgetQss(QWidget* widget, const QString& strQssPath)；

//4、绘制边框不连续问题
void paintBoder();

//5、qt布局：重新布局
void reLayout();