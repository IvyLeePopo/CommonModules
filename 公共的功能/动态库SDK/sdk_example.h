/*
 * @Description: SDK、加载动态库相关方法
 * @Author: Ivy
 * @Date: 2022-05-17 09:34:43
 * @LastEditTime: 2022-05-17 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once

// 简单调用动态库里面的api
typedef int(*vlprLogin)(int nType, char* sParas);

// 设置回调函数：调用动态库的截图的api，动态库收到指令执行处理，将结果用设置的回调函数返回
typedef void(*CBFun_GetRegResult)(int nHandle, T_VLPINFO* pVlpResult, void *pUser);//T_VLPINFO* 4 识别结果结构体
typedef int(*vlprSetResultCallBack)(int nHandle, CBFun_GetRegResult pFunc, void* pUser);

vlprLogin m_vlprLogin;  //（1）声明一个函数指针，后面接收动态库的接口：
vlprSetResultCallBack m_vlprSetResultCallBack; //声明一个回调函数

// 用window api 加载动态库
void loadDll();