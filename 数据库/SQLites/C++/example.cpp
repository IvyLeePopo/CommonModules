#include "DataBaseHelper.h"
#include "SystemDiary.h"

//�����ݿ�
sqlite3* sql_conn = DataBaseOpen("file path");

if (sql_conn == 0)
{
	TW_TRACE_DIARY_TYPE("���׻������ݿ�������⣬�޷���:" + GetDebitRequestCachePath(), TW_TRACE_TYPE_ETCPAYUNIT);
	return false;
}

//todo��add query delete or sth
DataBaseAdd(sql_conn, Parameter&);
DataBaseQuery(sql_conn,Parameter&);
DataBaseDelete(sql_conn, Parameter&);

//�ر����ݿ�
if (DataBaseClose(sql_conn) != 0)
{
	TW_TRACE_DIARY_TYPE("���ݿ�ر�ʧ��:" + GetDebitRequestCachePath(), TW_TRACE_TYPE_ETCPAYUNIT);
	return -1;
}