#ifndef DATA_BASE_HELPER_H
#define DATA_BASE_HELPER_H
#include "sqlite3_agent.h"
#include "ETCPayHelper.h"
#include "PayHelper.h"
//#include "md5.h"

namespace trawe
{
sqlite3* DataBaseOpen(const std::string& file_path);
int DataBaseCreateTable(sqlite3*, const std::string& table_name, const std::string& create_sql);
int DataBaseClose(sqlite3*);
std::string DataBaseGetLastError(sqlite3*);
int DataBaseReleaseDataSet(sqlite3_stmt* sql_stmt);

int DataBaseRecordReadInt(sqlite3_stmt* sql_stmt,int pos,int&val);
int DataBaseRecordReadVarchar(sqlite3_stmt* sql_stmt,int pos,char* dest,int dest_size);
int DataBaseRecordReadTimet(sqlite3_stmt* sql_stmt,int pos,time_t& val);

int DataBaseAddDebitQuest(sqlite3*, const ETCDebitRecord& );
int DataBaseQueryDebitQuest(sqlite3*, ETCDebitRecord& );

int DataBaseDeleteDebitQuest(sqlite3*, const ETCDebitRecord&);

int DataBaseAddRecord(sqlite3*, const ETCDebitRecord&);
int DataBaseQueryRecord(sqlite3*, ETCDebitRecord& );
int DataBaseDeleteRecord(sqlite3*, const ETCDebitRecord&);


int DataBaseAdd(sqlite3*, const DebitCancleParameter&);
int DataBaseQuery(sqlite3*, DebitCancleParameter& );
int DataBaseDelete(sqlite3*, const DebitCancleParameter&);

//发票撤销
int DataBaseAdd(sqlite3*, const InvoiceCancelParameter&);
int DataBaseQuery(sqlite3*, InvoiceCancelParameter& );
int DataBaseDelete(sqlite3*, const InvoiceCancelParameter&);

//发票上传
int DataBaseAdd(sqlite3*, const InvoiceParameter&);
int DataBaseQuery(sqlite3* sql_handle, InvoiceParameter&);
int DataBaseDelete(sqlite3*, const InvoiceParameter&);


std::string DataBaseGetDebitCancleTableName();
std::string DataBaseGetDebitCancleTableDefine();

//发票撤销缓存
std::string DataBaseGetInvoiceCancleTableName();
std::string DataBaseGetInvoiceCancleTableDefine();
//发票上传缓存
std::string DataBaseGetInvoiceTableName();
std::string DataBaseGetInvoiceTableDefine();

std::string DataBaseGetDebitRecordTableName();
std::string DataBaseGetDebitRequestTableName();

std::string DataBaseGetDebitRecordTableDefine();
std::string DataBaseGetDebitQuestTableDefine();

}

#endif // DATA_BASE_HELPER_H

/**
 * 数据库辅助处理
 * 1、查询操作，会判断表是否存在，如果不存在，同样认为没有记录
 */