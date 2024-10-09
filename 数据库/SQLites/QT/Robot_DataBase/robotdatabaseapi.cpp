#include "robotdatabaseapi.h"

#include "robotdatabasemanager.h"

bool DB_connect()
{
    //链接数据库
    return RobotDataBaseManager::getInstance()->connectDB();
}

bool DB_disConnect()
{
    return RobotDataBaseManager::getInstance()->disConnectDB();
}

bool DB_addData(string orderId, string entry, string money, string plate, string type, string weight)
{
    return RobotDataBaseManager::getInstance()->addData2DB(orderId, entry, money, plate, type, weight);
}

bool DB_deleteData(string id)
{
    return RobotDataBaseManager::getInstance()->DeleteDataById(id);
}

bool DB_updateData(string orderId, string entry, string money, string plate, string type, string weight)
{
    return RobotDataBaseManager::getInstance()->UpdateDataById(orderId, entry, money, plate, type, weight);
}

bool DB_queryDataTypeById(string id)
{
    return RobotDataBaseManager::getInstance()->getTypeById(id);
}

bool DB_queryDataById(string id, string &entry, string &money, string &plate, string &type, string &weight)
{
    return RobotDataBaseManager::getInstance()->getDataById(id, entry, money, plate, type, weight);
}

bool DB_queryExec(string strSql)
{
    return RobotDataBaseManager::getInstance()->queryExec(strSql);
}

int DB_totalCount()
{
    return RobotDataBaseManager::getInstance()->countDB();
}
