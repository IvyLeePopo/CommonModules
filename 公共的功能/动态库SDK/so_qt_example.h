/*
 * @Description: linux下，加载.so动态库的用法
 * @Author: Ivy
 * @Date: 2022-05-17 09:34:43
 * @LastEditTime: 2022-05-17 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once


 // 动态库接口声明
typedef int(*TYPE_JT_OpenReader)(int nMode, char *sParas);
// 声明一个变量
TYPE_JT_OpenReader    JT_OpenReader;

 // 用qt 加载动态库
void loadDll();