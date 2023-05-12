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

// 3、宏代替重复代码
void repeatCode()

// 5、实现多语言
void initLanguage();

// 6、判断对象对错：定义宏
void checkResult();

// 7、读取sharedfilemap数据
bool readSharedFileMap(const char* pszDataKey, string& strData);

// 8、清除共享内存的数据
bool clearSharedFileMap(const char* pszDataKey);

// 9、获取特定范围的随机数字
int getRandValue();

