#include "datacenter.h"

#include<QDebug>
#include<QSqlDatabase>

#include "robotdatabaseapi.h"

DataCenter::DataCenter(QObject *parent)
    : QObject(parent)
{

}

void DataCenter::slotConnectDB()
{
    bool ret = DB_connect();
    emit sigStatus(enDbOperationType::connect, ret);
}

void DataCenter::slotDisconnected()
{
    bool ret = DB_disConnect();
    emit sigStatus(enDbOperationType::disConnected, ret);
}

void DataCenter::slotAddData(string strOrderId, string strEntry, string strMoney, string strPlate, string strType, string strWeight)
{
    bool ret = DB_addData(strOrderId, strEntry, strMoney, strPlate, strType, strWeight);
    emit sigStatus(enDbOperationType::addData, ret);
}

void DataCenter::slotAddMoreData(int count, int index)
{
    QStringList types;
    types << "客一"<< "客二" << "客三" << "客四" << "货一" << "货二" << "货三" << "货四";


    QStringList entrys;
    entrys << "天河"<< "滘口" << "南沙" << "海珠" << "白云" << "增城" << "荔湾" << "越秀";


    tagOrderParam tempParam;


    int sum = index + count;
    bool ret = false;
    for(; index < sum; ++index)
    {
        tempParam.strOrderID = QString::number(index).toStdString();

        tempParam.strEntry = entrys[index%8].toStdString();
        tempParam.strMoney = "99";
        tempParam.strPlate = "粤A777Q1";
        tempParam.strType = types[index%8].toStdString();
        tempParam.strWeight = QString::number(index).toStdString();


        ret = DB_addData(tempParam.strOrderID, tempParam.strEntry, tempParam.strMoney,
                              tempParam.strPlate, tempParam.strType, tempParam.strWeight);

        if(!ret)
        {
            qDebug() << "添加数据失败！";
            break;
        }
    }

    emit sigStatus(enDbOperationType::addMoreData, ret);
}

void DataCenter::slotDeleteDataById(string strOrderd)
{
    bool ret = DB_deleteData(strOrderd);
    emit sigStatus(enDbOperationType::deleteData, ret);
}

void DataCenter::slotUpdateDataById(string strOrderId, string strEntry, string strMoney, string strPlate, string strType, string strWeight)
{
    bool ret = DB_updateData(strOrderId, strEntry, strMoney, strPlate, strType, strWeight);
    emit sigStatus(enDbOperationType::updateData, ret);
}

void DataCenter::slotGetDataById(string strOrderId)
{
    tagOrderParam tempParam;
    bool ret = DB_queryDataById(strOrderId, tempParam.strEntry, tempParam.strMoney, tempParam.strPlate, tempParam.strType, tempParam.strWeight);
    emit sigQueryDataById(ret, tempParam.strEntry, tempParam.strMoney, tempParam.strPlate, tempParam.strType, tempParam.strWeight);
}

void DataCenter::slotGetDbCount()
{
    int count = DB_totalCount();
    emit sigCount(count);
}
