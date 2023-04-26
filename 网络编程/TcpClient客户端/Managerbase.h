/*
 * @Description: 业务管理基类（主要是进行TCP消息的分发，应要虚函数进行）
 * @Author: Ivy
 * @Date: 2022-06-01 15:34:43
 * @LastEditTime: 2023-02-28 18:12:28
 * @LastEditors: XTZJ-2022OQEHLZ
 */
#pragma once
#include <string>
#include <QObject>
#define ManagerType_Base "管理基础类"
#define ManagerType_HeadSmartNode "岛头管理类"
#define ManagerType_GroundNode "地感管理类"

struct tagReqGlobalLaneInfo;
using namespace std;
class ManagerBase : public QObject
{
    Q_OBJECT

public:
    ManagerBase();
    virtual ~ManagerBase();

public:
    // 初始化数据
    virtual void init();

    // 接收数据,可以仿照 HeadSmartNodeManager 类接收方式
    virtual void cycle(void* pObject);

    // 设置/获取Manager的类型
    virtual void setType(string strType) { m_strType = strType; }

    // 获取Manager的类型
    virtual string getType() { return m_strType; }

protected:
    string m_strType;                                   // 区别各个管理类的标识
};
