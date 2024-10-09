#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>
#include <string>

using namespace std;

enum enDbOperationType
{
    connect = 0,
    disConnected = 1,
    addData,
    addMoreData,
    deleteData,
    updateData,
    queryData,
};

Q_DECLARE_METATYPE(enDbOperationType);

class DataCenter : public QObject
{
    Q_OBJECT

public:
    DataCenter(QObject *parent = nullptr);


public slots:
    // 连接/断开
    void slotConnectDB();
    void slotDisconnected();

    // 添加
    void slotAddData(string strOrderId, string strEntry, string strMoney, string strPlate, string strType, string strWeight);
    void slotAddMoreData(int count, int index);

    // 删除
    void slotDeleteDataById(string strOrderd);

    // 更新
    void slotUpdateDataById(string strOrderId, string strEntry, string strMoney, string strPlate, string strType, string strWeight);

    // 查询
    void slotGetDataById(string strOrderId);

    //　数据库大小
    void slotGetDbCount();
signals:
    void sigStatus(enDbOperationType type, bool flag);
    void sigCount(int count);
    void sigQueryDataById(bool ret, string strEntry, string strMoney, string strPlate, string strType, string strWeight);
};

#endif // DATACENTER_H
