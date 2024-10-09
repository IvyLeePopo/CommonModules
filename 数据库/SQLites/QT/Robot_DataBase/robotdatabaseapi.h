#ifndef ROBOTDATABASEAPI_H
#define ROBOTDATABASEAPI_H

//定义宏
#if defined(__cplusplus)
#define D_EXTERN_C extern "C"
#else
#define D_EXTERN_C
#endif

#include<string>

#undef IN
#undef OUT
#define IN
#define OUT

using namespace std;


// 基本结构体
struct tagOrderParam
{
    string strOrderID;              // 订单ID
    string strEntry;                // 入口
    string strMoney;                // 金额
    string strPlate;                // 车牌
    string strType;                 // 车型
    string strWeight;               // 重量
};


/**************************************************
功能:
    连接数据库
参数：
    N/A
返回:
    -1,fail;
    =0,success.
***************************************************/
D_EXTERN_C bool DB_connect();

/**************************************************
功能:
    断开数据库
参数：
    N/A
返回:
    -1,fail;
    =0,success.
***************************************************/
D_EXTERN_C bool DB_disConnect();

/**************************************************
功能:
    添加数据
参数：
    IN string orderId：订单id
    IN int entry：入口
    IN int money：金额
    IN int plate：车牌
    IN int type：车型
    IN int weight：重量
返回:
    -1,fail;
    =0,success.
***************************************************/
D_EXTERN_C bool DB_addData(IN string orderId, IN string entry, IN string money, IN string plate, IN string type, IN string weight);


/**************************************************
功能:
    删除数据
参数：
    IN string id：订单id
返回:
    -1,fail;
    =0,success.
***************************************************/
D_EXTERN_C bool DB_deleteData(IN string id);


/**************************************************
功能:
    更新数据
参数：
    IN string orderId：订单id
    IN int entry：入口
    IN int money：金额
    IN int plate：车牌
    IN int type：车型
    IN int weight：重量
返回:
    -1,fail;
    =0,success.
***************************************************/
D_EXTERN_C bool DB_updateData(IN string orderId, IN string entry, IN string money, IN string plate, IN string type, IN string weight);

/**************************************************
功能:
    查询数据
参数：
    IN string id：订单id
返回:
    -1,fail;
    =0,success.
***************************************************/
D_EXTERN_C bool DB_queryDataTypeById(IN string id);
D_EXTERN_C bool DB_queryDataById(IN string id, OUT string& entry, OUT string& money, OUT string& plate, OUT string& type, OUT string& weight);

/**************************************************
功能:
    直接执行sql语句
参数：
    IN string strSql:需要执行的sql语句
返回:
    -1,fail;
    =0,success.
***************************************************/
D_EXTERN_C bool DB_queryExec(IN string strSql);


/**************************************************
功能:
    查询数据库的大小
参数：

返回:
    数据库数据的条数;
***************************************************/
D_EXTERN_C int DB_totalCount();

#endif // ROBOTDATABASEAPI_H
