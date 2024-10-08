#include "DataBaseHelper.h"
#include "SystemDiary.h"

//打开数据库
sqlite3* sql_conn = DataBaseOpen("file path");

if (sql_conn == 0)
{
	TW_TRACE_DIARY_TYPE("交易缓存数据库存在问题，无法打开:" + GetDebitRequestCachePath(), TW_TRACE_TYPE_ETCPAYUNIT);
	return false;
}

//todo：add query delete or sth
DataBaseAdd(sql_conn, Parameter&);
DataBaseQuery(sql_conn,Parameter&);
DataBaseDelete(sql_conn, Parameter&);

//关闭数据库
if (DataBaseClose(sql_conn) != 0)
{
	TW_TRACE_DIARY_TYPE("数据库关闭失败:" + GetDebitRequestCachePath(), TW_TRACE_TYPE_ETCPAYUNIT);
	return -1;
}