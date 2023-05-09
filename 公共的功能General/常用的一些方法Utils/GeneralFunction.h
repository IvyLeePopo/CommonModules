/*
 * @Description: 常用的方法
 * @Author: Ivy
 * @Date: 2022-05-09 09:34:43
 * @LastEditTime: 2022-05-09 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

// 1、将当前窗口置于屏幕中间位置
void setWgtCenter();

// 2、测试时间间隔
void calTimeGap();

// 3、宏代替重复代码
void repeatCode();

//4、判断软件是否重复启动
bool IsRepeatStart(const QRect& rect);