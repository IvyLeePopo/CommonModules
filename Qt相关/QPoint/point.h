/*
 * @Description: QPoint的常见用法
 * @Author: Ivy
 * @Date: 2022-05-09 09:34:43
 * @LastEditTime: 2022-05-09 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

//1、设置弹出菜单的位置；
void popupPos();

//2、获得该控件的左上角位置；
QPoint getLeftTopPos();

//3、判断当前光标是否在控件内；
bool isContain();

/**
4、QT中的相对位置，绝对位置之间的转换(maptoglobal, mapfromglobal)
（1） 相对位置：每个Qwidget都能通过pos()获取到相对自己父类窗口的位置，
（2） 绝对位置：pWidget->mapToGlobal(QPoint(0, 0)); 将当前控件的相对位置转换为屏幕绝对位置
（3） 绝对位置转为相对位置： pWidget->mapFromGlobal(QPoint(0, 0)), 将绝对位置对应到控件的相对位置
*/
