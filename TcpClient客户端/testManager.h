/*
 * @Description: 具体业务管理类
 * @Author: Ivy
 * @Date: 2022-06-01 15:34:43
 * @LastEditTime: 2023-02-22 20:56:48
 * @LastEditors: XTZJ-2022OQEHLZ
 */
#pragma once
#include "Managerbase.h"
#include "Singleton.h"
#include "GroundNodeParse.h"
#include "EndSmartNodeParse.h"

class TcpClient;
class testManager : public ManagerBase, public Singleton<testManager>
{
public:
	testManager();
	virtual ~testManager();

	//初始化
	virtual void init();

	//接收指令
	virtual void cycle(void* pObject);

private:
	TcpClient *m_pTcpClient;
};
