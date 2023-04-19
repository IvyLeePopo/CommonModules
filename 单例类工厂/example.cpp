/*
 * @Description: XXX 管理类
 */
#pragma once
#include "Singleton.h"

class testManager : public Singleton<testManager>
{
    friend class Singleton<testManager>;//要声明友元类
	
public:
    virtual ~testManager();
	
	void getData();
	void setData();
	
private:
    testManager();
}


// 实例演示：包含该类的头文件
testManager::getInstance()->getData();