#include "stdafx.h"
#include "DataBaseHelper.h"
#include <sstream>
#include "md5.h"
#include "SystemDiary.h"

char* MultiByteToUTF8(const char *file)
{
    std::string str;
    WCHAR   *pwchar = 0;
    CHAR    *pchar = 0;
    int len = 0;
    int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
    len = MultiByteToWideChar(codepage, 0, file, -1, NULL, 0);
    pwchar = new WCHAR[len];
    if (pwchar != 0)
    {
        len = MultiByteToWideChar(codepage, 0, file, -1, pwchar, len);
        if ( len != 0 )
        {
            len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, 0, 0, 0, 0);
            pchar = new CHAR[len];
            if (pchar != 0)
            {
                len = WideCharToMultiByte(CP_UTF8, 0, pwchar, -1, pchar, len, 0, 0);
                if (len != 0)
                {
                    str = pchar;
                }
                delete pchar;
            }
            delete pwchar;
        }
    }
    int iLen = static_cast<int>(str.size());
    char* pszData = NULL;
    if (iLen > 0)
    {
        pszData = new char[iLen + 1];
        if (NULL != pszData)
        {
            memset(pszData, 0, iLen + 1);
            strcpy(pszData, str.c_str());
        }
    }
    return pszData;
}

namespace trawe
{

sqlite3* DataBaseOpen(const std::string& file_path)
{
    sqlite3* sql_handle = 0;
    char* pszString = MultiByteToUTF8(file_path.c_str());// = UnicodeToAscii(m_strDatabaseFilePath);
    if (NULL == pszString)
    {
        TW_TRACE_DIARY("数据库打开失败,路径转换错误:" + file_path);
        return 0;
    }
    //执行创建或打开SQLite3数据库指令
    int iDBStatus = sqlite3_agent::sqlite3_open_v2_fun(pszString, &sql_handle , SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
    delete []pszString;
    pszString = NULL;
    if (SQLITE_OK != iDBStatus)
    {
        return 0;
    }
    return sql_handle;
}

int sqlite3_callback_func( void* pHandle, int iRet, char** szSrc, char** szDst )
{
    //...
    if ( 1 == iRet )
    {
        int iTableExist = atoi(*(szSrc));  //此处返回值为查询到同名表的个数，没有则为0，否则大于0
        if (pHandle != 0)
        {
            int* pRes = (int*)pHandle;
            *pRes = iTableExist;
        }
        // szDst 指向的内容为"count(*)"
    }

    return 0; //返回值一定要写，否则下次调用 sqlite3_exec(...) 时会返回 SQLITE_ABORT
}
//参考:https://blog.csdn.net/autumoonchina/article/details/78839239
bool DataBaseIsTableExist(sqlite3* sql_handle, const std::string& strTableName)
{
    std::string strFindTable = "SELECT COUNT(*) FROM sqlite_master where type ='table' and name ='" + strTableName + "'";


    int nTableNums = 0;
    if (sqlite3_agent::sqlite3_exec_fun(sql_handle, strFindTable.c_str(), &sqlite3_callback_func, &nTableNums, NULL) != SQLITE_OK)
    {
        return false;
    }

    return nTableNums > 0;
}

int DataBaseCreateTable(sqlite3* sql_handle, const std::string& table_name, const std::string& create_sql)
{
    if (sql_handle == 0)
    {
        TW_TRACE_DIARY("数据库句柄非法");
        return -1;
    }
    //检查表是否存在
    if (DataBaseIsTableExist(sql_handle, table_name))
    {
        return 0;
    }
    int iDBStatus = sqlite3_agent::sqlite3_exec_fun(sql_handle, create_sql.c_str(), NULL, NULL, NULL);
    if (iDBStatus != SQLITE_OK)
    {
        TW_TRACE_DIARY("创建数据表失败:" + table_name);
        return -1;
    }
    return 0;
}

int DataBaseReleaseDataSet(sqlite3_stmt* sql_stmt)
{
    if (sql_stmt == 0)
    {
        return 0;
    }
    int iDBStatus = sqlite3_agent::sqlite3_finalize_fun(sql_stmt);
    if (SQLITE_OK != iDBStatus)
    {
        TW_TRACE_DIARY("关闭数据集失败");
        return -1;
    }
    return 0;

}
int DataBaseClose(sqlite3* sql_handle)
{
    if (sql_handle == 0)
    {
        return 0;
    }
    int iDBStatus = sqlite3_agent::sqlite3_close_v2_fun(sql_handle);
    if (SQLITE_OK != iDBStatus)
    {
        TW_TRACE_DIARY("关闭数据库失败");
        return -1;
    }
    return 0;
}
std::string DataBaseGetLastError(sqlite3*sql_handle)
{
    if (sql_handle == 0)
    {
        return "数据句柄非法，无法获取错误";
    }
    char szThrowDBError[500] = {0};
    DWORD m_dwLastErrorCode = sqlite3_agent::sqlite3_errcode_fun(sql_handle);
    strcpy_s(szThrowDBError, _countof(szThrowDBError), sqlite3_agent::sqlite3_errmsg_fun(sql_handle));
    return std::string(szThrowDBError);
}


int DataBaseAddDebitQuest(sqlite3*sql_handle , const ETCDebitRecord& record)
{
    CFunctionCallTrace fun_trace("支付数据缓存增加");
    if (sql_handle == 0)
    {
        return -1;
    }
    std::stringstream ss;
    ss << "insert into " << DataBaseGetDebitRequestTableName();
    ss << " values(" ;
    ss <<"'"<< record.PayIdentifier<<"'";
    ss << " ," << 1;
    ss << " ,?);";
    std::string add_sql = ss.str();
    const  char* bin_data = (const char*)&record;
    sqlite3_stmt *stmt = 0;
    if (SQLITE_OK != sqlite3_agent::sqlite3_prepare_v2_fun(sql_handle, add_sql.c_str(), add_sql.size(), &stmt, 0))
    {
        return -1;
    }
    {
        if (SQLITE_OK != sqlite3_agent::sqlite3_bind_blob_fun(stmt, 1, bin_data, sizeof(record), NULL)) //1代表第一个？
        {
            sqlite3_agent::sqlite3_finalize_fun(stmt);
            return -1;
        }
        if (SQLITE_DONE != sqlite3_agent::sqlite3_step_fun(stmt))                                     //将数据写入数据库中
        {
            sqlite3_agent::sqlite3_finalize_fun(stmt);
            return -1;
        }
    }
    sqlite3_agent::sqlite3_finalize_fun(stmt);
    return 0;
}
int DataBaseQueryDebitQuest(sqlite3* sql_handle, ETCDebitRecord& record)
{
    CFunctionCallTrace fun_trace("支付数据缓存查询");
    if (sql_handle == 0)
    {
        return -1;
    }
    sqlite3_stmt *stmt = 0;
    std::string query_sql = "select * from " + DataBaseGetDebitRequestTableName() + ";";
    sqlite3_agent::sqlite3_prepare_v2_fun(sql_handle, query_sql.c_str(), -1, &stmt, 0);
    int result = sqlite3_agent::sqlite3_step_fun(stmt);
    if (result == SQLITE_ROW)                                     //查询成功返回的是SQLITE_ROW
    {
        const void * bin_data = sqlite3_agent::sqlite3_column_blob_fun(stmt, 2); //读取数据，返回一个指针
        int len = sqlite3_agent::sqlite3_column_bytes_fun(stmt, 2);                      //返回数据大小
        memcpy(&record, bin_data, len);                       //把数据拷贝出来
		sqlite3_agent::sqlite3_finalize_fun(stmt);
    }
	else
	{
		return -1;
	}
    
    return 0;
}

int DataBaseDeleteDebitQuest(sqlite3*sql_handle, const ETCDebitRecord&record)
{
    if (sql_handle == 0)
    {
        return -1;
    }
    std::stringstream ss;
    ss << "delete  from " << DataBaseGetDebitRequestTableName();
    ss <<  " where PayIdentifier = '"  << record.PayIdentifier << "';";
    std::string delete_sql = ss.str();
    int nRes =  sqlite3_agent::sqlite3_exec_fun(sql_handle, delete_sql.c_str(), 0, 0, 0);
    if (nRes != SQLITE_OK)
    {
        return -1;
    }
    return 0;
}

int DataBaseAddRecord(sqlite3*sql_handle, const ETCDebitRecord&record)
{
    CFunctionCallTrace fun_trace("归档记录增加");
    if (sql_handle == 0)
    {
        return -1;
    }
    std::stringstream ss;
    ss.imbue(std::locale(std::locale::classic(),"",std::locale::ctype));
    ss << "insert into " << DataBaseGetDebitRecordTableName();
    ss << " values(" ;
    ss << "'"<< record.PayIdentifier << "'";
    ss << " ," << record.LocalResponse.ExecuteCode;
    ss << " ," << record.ServerResponse.ServerStatus;
    ss << " ,'" << record.LocalResponse.TradeSecurityCode << "'";
    ss << " ," << record.LocalResponse.PayPlatformType;
    ss << " ,'" << record.ServerResponse.DebitOrder << "'";
    ss << " ,'" << Datetime2Str(record.LocalResponse.DebitTime) << "'";
    ss << " ,'" << record.LocalResponse.PaySerialNo << "'";
    ss << " ," << record.Money;
    ss << " ,'" << record.OBUID << "'";
    ss << " ,'" << StringConvertByCodePage(record.OBULicense, CP_ACP, CP_UTF8) << "'";
    ss << " ," << record.OBULicenseColor;
    ss << " ,'" << record.CardID << "'";
    ss << " ," << 1;
    ss << " );";
    std::string add_sql = ss.str();
    if (SQLITE_OK != sqlite3_agent::sqlite3_exec_fun(sql_handle, add_sql.c_str(), 0, 0, 0))
    {
        return -1;
    }
    return 0;
}
int DataBaseQueryRecord(sqlite3*sql_handle, ETCDebitRecord& record)
{
    CFunctionCallTrace fun_trace("归档记录查询");
    if (sql_handle == 0)
    {
        return -1;
    }
    sqlite3_stmt *sql_stmt = 0;
    std::stringstream ss;
    ss << "select * from " << DataBaseGetDebitRecordTableName() ;
    ss << " where PayIdentifier='" << record.PayIdentifier << "';";
    std::string query_sql = ss.str();
    if(sqlite3_agent::sqlite3_prepare_v2_fun(sql_handle, query_sql.c_str(), -1, &sql_stmt, 0)!=SQLITE_OK)
    {
        return -1;
    }
    int result = sqlite3_agent::sqlite3_step_fun(sql_stmt);
    char tmp_chs[100]={0};
    if (result == SQLITE_ROW)                                     //查询成功返回的是SQLITE_ROW
    {
        DataBaseRecordReadVarchar(sql_stmt, 0, record.PayIdentifier, sizeof(record.PayIdentifier));
        DataBaseRecordReadInt(sql_stmt, 1, record.LocalResponse.ExecuteCode);
        DataBaseRecordReadInt(sql_stmt, 2, record.ServerResponse.ServerStatus);
		DataBaseRecordReadVarchar(sql_stmt, 3, record.LocalResponse.TradeSecurityCode, sizeof(record.LocalResponse.TradeSecurityCode));
        DataBaseRecordReadInt(sql_stmt, 4, record.LocalResponse.PayPlatformType);
        DataBaseRecordReadVarchar(sql_stmt, 5, record.ServerResponse.DebitOrder, sizeof(record.ServerResponse.DebitOrder));
        DataBaseRecordReadVarchar(sql_stmt, 6, tmp_chs, sizeof(tmp_chs));
        record.LocalResponse.DebitTime = Str2Datetime(tmp_chs);
        DataBaseRecordReadVarchar(sql_stmt, 7, record.LocalResponse.PaySerialNo, sizeof(record.LocalResponse.PaySerialNo));
        DataBaseRecordReadInt(sql_stmt, 8, record.Money);
        DataBaseRecordReadVarchar(sql_stmt, 9, record.OBUID, sizeof(record.OBUID));
		DataBaseRecordReadVarchar(sql_stmt, 10, record.OBULicense, sizeof(record.OBULicense));
		DataBaseRecordReadInt(sql_stmt, 11, record.OBULicenseColor);
        DataBaseRecordReadVarchar(sql_stmt, 12, record.CardID, sizeof(record.CardID));
		sqlite3_agent::sqlite3_finalize_fun(sql_stmt);
        strcpy(record.LocalResponse.PayIdentifier,record.PayIdentifier);


		record.LocalResponse.ExecuteCode = 255;
		switch(record.ServerResponse.ServerStatus)
		{
		case 1:
			record.LocalResponse.ExecuteCode = 0;
			break;
		case 30:
			record.LocalResponse.ExecuteCode = 3;
			break;
		case 31:
		case 32:
		case 33:
			record.LocalResponse.ExecuteCode = 1;
			break;
		case 34:
			record.LocalResponse.ExecuteCode = 2;
			break;
		}
    }
	else
	{
        sqlite3_agent::sqlite3_finalize_fun(sql_stmt);
		return -1;
	}
    
    return 0;
}
int DataBaseDeleteRecord(sqlite3*sql_handle, const ETCDebitRecord&record)
{
    if (sql_handle == 0)
    {
        return -1;
    }
    // 目前的归档记录，不支持删除
    return -1;
}

int DataBaseAdd(sqlite3*sql_handle, const DebitCancleParameter&record)
{
    if (sql_handle == 0)
    {
        return -1;
    }
    std::string create_time = Datetime2Str(ChinaTime());
    std::string verify_code;
    {   //计算校验码
        std::stringstream calc_para;
        calc_para.imbue(std::locale(std::locale::classic(), "", std::locale::ctype));
        calc_para <<record.PayIdentifier;
        calc_para <<record.ProvinceID;
        calc_para << create_time;
        verify_code = md5(calc_para.str());
    }
    std::stringstream ss;
    ss.imbue(std::locale(std::locale::classic(), "", std::locale::ctype));
    ss << "insert into " << DataBaseGetDebitCancleTableName();
    ss << " values(" ;
    ss << "'" << record.PayIdentifier << "'";
    ss << " ," << record.ProvinceID;
    ss << " ,'" << verify_code << "'";
    ss << " ,'" << create_time << "'";
    ss << " );";
    std::string add_sql = ss.str();
    if (SQLITE_OK != sqlite3_agent::sqlite3_exec_fun(sql_handle, add_sql.c_str(), 0, 0, 0))
    {
        return -1;
    }
    return 0;
}


int DataBaseAdd(sqlite3* sql_handle, const InvoiceCancelParameter&record)
{
	if (sql_handle == 0)
    {
        return -1;
    }

    std::stringstream ss;
    ss.imbue(std::locale(std::locale::classic(), "", std::locale::ctype));
    ss << "insert into " << DataBaseGetInvoiceCancleTableName();
    ss << " values(" ;
    ss << "'" << record.ElectronicInvoiceNo << "'";
    ss << " ,'" << record.PayProvinceId << "'";
    ss << " ,'" << record.ExitTime << "'";
    ss << " );";
    std::string add_sql = ss.str();
	TW_TRACE_DIARY("发票撤销数据插入数据库sql:" + add_sql);
    if (SQLITE_OK != sqlite3_agent::sqlite3_exec_fun(sql_handle, add_sql.c_str(), 0, 0, 0))
    {
        return -1;
    }
    return 0;
}

int DataBaseAdd(sqlite3* sql_handle, const InvoiceParameter&record)
{
	if (sql_handle == 0)
    {
        return -1;
    }

    std::stringstream ss;
    ss.imbue(std::locale(std::locale::classic(), "", std::locale::ctype));
    ss << "insert into " << DataBaseGetInvoiceTableName();
    ss << " values(" ;
    ss << "'" << record.ElectronicInvoiceNo << "'";
    ss << " ,'" << record.DebitOrderNumber << "'";
    ss << " ," << record.Money;
    ss << " ," << record.PayMode;
    ss << " ,'" << record.VehicleLicense<< "'";
    ss << " ," << record.VehicleColor;
    ss << " ," << record.VehicleType;
    ss << " ," << record.VehicleClass;
    ss << " ," << record.AxisCount;
    ss << " ," << record.TotalWeight;
    ss << " ," << record.LimitWeight;
    ss << " ,'" << record.PayAreaID << "'";
    ss << " ,'" << record.PayProvinceId << "'";
    ss << " ,'" << record.PayRoadID << "'";
    ss << " ,'" << record.PayStationID << "'";
    ss << " ,'" << record.PayStationName << "'";
    ss << " ,'" << record.PayLane << "'";
    ss << " ,'" << record.ExitTime << "'";
    ss << " ," << record.nExitTime;
    ss << " ," << record.PassCertificateType;
    ss << " ,'" << record.PassCertificateCode << "'";
    ss << " ,'" << record.TollDate << "'";
    ss << " ," << record.ShiftID;
    ss << " ,'" << record.OperatorID << "'";
    ss << " ,'" << record.OperatorName << "'";
    ss << " ," << record.Distance;
    ss << " ,'" << record.EntryAreaID << "'";
    ss << " ,'" << record.EntryRoadID << "'";
    ss << " ,'" << record.EntryStationID << "'";
    ss << " ,'" << record.EntryStationName << "'";
    ss << " ,'" << record.EntryLaneID << "'";
    ss << " ,'" << record.EntryTime << "'";
    ss << " ," << record.nEntryTime;
    ss << " ,'" << record.EntryVehicleLicense << "'";
    ss << " ," << record.EntryVLColor;
    ss << " ," << record.EntryVehicleClass;
    ss << " ," << record.EntryVehicleType;
    ss << " );";
    std::string add_sql = ss.str();
	TW_TRACE_DIARY("发票上传数据插入数据库sql:" + add_sql);
    if (SQLITE_OK != sqlite3_agent::sqlite3_exec_fun(sql_handle, add_sql.c_str(), 0, 0, 0))
    {
        return -1;
    }
    return 0;
}


int DataBaseQuery(sqlite3* sql_handle, DebitCancleParameter&record )
{
    if (sql_handle == 0)
    {
        return -1;
    }
    sqlite3_stmt *sql_stmt = 0;
    std::stringstream ss;
    ss << "select * from " << DataBaseGetDebitCancleTableName() << " LIMIT 1;";
    std::string query_sql = ss.str();
    if (sqlite3_agent::sqlite3_prepare_v2_fun(sql_handle, query_sql.c_str(), -1, &sql_stmt, 0) != SQLITE_OK)
    {
        return -1;
    }
    int result = sqlite3_agent::sqlite3_step_fun(sql_stmt);
    char tmp_chs[100] = {0};
    if (result == SQLITE_ROW)                                     //查询成功返回的是SQLITE_ROW
    {
        DataBaseRecordReadVarchar(sql_stmt, 0, record.PayIdentifier, sizeof(record.PayIdentifier));
        DataBaseRecordReadInt(sql_stmt, 1, record.ProvinceID);
        DataBaseRecordReadVarchar(sql_stmt, 2, tmp_chs, sizeof(tmp_chs));
        DataBaseRecordReadTimet(sql_stmt, 3, record.GenerateTime);
        sqlite3_agent::sqlite3_finalize_fun(sql_stmt);
    }
    else
    {
        sqlite3_agent::sqlite3_finalize_fun(sql_stmt);
        return -1;
    }

    return 0;
}

int DataBaseQuery(sqlite3* sql_handle, InvoiceCancelParameter&record)
{
	if (sql_handle == 0)
    {
        return -1;
    }
    sqlite3_stmt *sql_stmt = 0;
    std::stringstream ss;
    ss << "select * from " << DataBaseGetInvoiceCancleTableName() << " LIMIT 1;";
    std::string query_sql = ss.str();
    if (sqlite3_agent::sqlite3_prepare_v2_fun(sql_handle, query_sql.c_str(), -1, &sql_stmt, 0) != SQLITE_OK)
    {
        return -1;
    }
    int result = sqlite3_agent::sqlite3_step_fun(sql_stmt);
    if (result == SQLITE_ROW)                                     //查询成功返回的是SQLITE_ROW
    {
        DataBaseRecordReadVarchar(sql_stmt, 0, record.ElectronicInvoiceNo, sizeof(record.ElectronicInvoiceNo));
        DataBaseRecordReadVarchar(sql_stmt, 1, record.PayProvinceId, sizeof(record.PayProvinceId));
        DataBaseRecordReadVarchar(sql_stmt, 2, record.ExitTime, sizeof(record.ExitTime));
		record.IsDataBaseRecord = 1;
        sqlite3_agent::sqlite3_finalize_fun(sql_stmt);
    }
    else
    {
        sqlite3_agent::sqlite3_finalize_fun(sql_stmt);
        return -1;
    }

    return 0;
}

int DataBaseQuery(sqlite3* sql_handle, InvoiceParameter&record)
{
	if (sql_handle == 0)
    {
        return -1;
    }
    sqlite3_stmt *sql_stmt = 0;
    std::stringstream ss;
    ss << "select * from " << DataBaseGetInvoiceTableName() << " LIMIT 1;";
    std::string query_sql = ss.str();
    if (sqlite3_agent::sqlite3_prepare_v2_fun(sql_handle, query_sql.c_str(), -1, &sql_stmt, 0) != SQLITE_OK)
    {
        return -1;
    }
    int result = sqlite3_agent::sqlite3_step_fun(sql_stmt);
    if (result == SQLITE_ROW)                                     //查询成功返回的是SQLITE_ROW
    {
        DataBaseRecordReadVarchar(sql_stmt, 0, record.ElectronicInvoiceNo, sizeof(record.ElectronicInvoiceNo));
        DataBaseRecordReadVarchar(sql_stmt, 1, record.DebitOrderNumber, sizeof(record.DebitOrderNumber));
        DataBaseRecordReadInt(sql_stmt, 2, record.Money);
		DataBaseRecordReadInt(sql_stmt, 3, record.PayMode);
        DataBaseRecordReadVarchar(sql_stmt, 4, record.VehicleLicense, sizeof(record.VehicleLicense));
        DataBaseRecordReadInt(sql_stmt, 5, record.VehicleColor);
		DataBaseRecordReadInt(sql_stmt, 6, record.VehicleType);
        DataBaseRecordReadInt(sql_stmt, 7, record.VehicleClass);
        DataBaseRecordReadInt(sql_stmt, 8, record.AxisCount);
		DataBaseRecordReadInt(sql_stmt, 9, record.TotalWeight);
        DataBaseRecordReadInt(sql_stmt, 10, record.LimitWeight);
        DataBaseRecordReadVarchar(sql_stmt, 11, record.PayAreaID, sizeof(record.PayAreaID));
        DataBaseRecordReadVarchar(sql_stmt, 12, record.PayProvinceId, sizeof(record.PayProvinceId));
		DataBaseRecordReadVarchar(sql_stmt, 13, record.PayRoadID, sizeof(record.PayRoadID));
        DataBaseRecordReadVarchar(sql_stmt, 14, record.PayStationID, sizeof(record.PayStationID));
        DataBaseRecordReadVarchar(sql_stmt, 15, record.PayStationName, sizeof(record.PayStationName));
		DataBaseRecordReadVarchar(sql_stmt, 16, record.PayLane, sizeof(record.PayLane));
        DataBaseRecordReadVarchar(sql_stmt, 17, record.ExitTime, sizeof(record.ExitTime));
        DataBaseRecordReadTimet(sql_stmt, 18, record.nExitTime);
		DataBaseRecordReadInt(sql_stmt, 19, record.PassCertificateType);
        DataBaseRecordReadVarchar(sql_stmt, 20, record.PassCertificateCode, sizeof(record.PassCertificateCode));
        DataBaseRecordReadVarchar(sql_stmt, 21, record.TollDate, sizeof(record.TollDate));
		DataBaseRecordReadInt(sql_stmt, 22, record.ShiftID);
		DataBaseRecordReadVarchar(sql_stmt, 23, record.OperatorID, sizeof(record.OperatorID));
        DataBaseRecordReadVarchar(sql_stmt, 24, record.OperatorName, sizeof(record.OperatorName));
        DataBaseRecordReadInt(sql_stmt, 25, record.Distance);
		DataBaseRecordReadVarchar(sql_stmt, 26, record.EntryAreaID, sizeof(record.EntryAreaID));
        DataBaseRecordReadVarchar(sql_stmt, 27, record.EntryRoadID, sizeof(record.EntryRoadID));
        DataBaseRecordReadVarchar(sql_stmt, 28, record.EntryStationID, sizeof(record.EntryStationID));
		DataBaseRecordReadVarchar(sql_stmt, 29, record.EntryStationName, sizeof(record.EntryStationName));
        DataBaseRecordReadVarchar(sql_stmt, 30, record.EntryLaneID, sizeof(record.EntryLaneID));
        DataBaseRecordReadVarchar(sql_stmt, 31, record.EntryTime, sizeof(record.EntryTime));
        DataBaseRecordReadTimet(sql_stmt, 32, record.nEntryTime);
		DataBaseRecordReadVarchar(sql_stmt, 33, record.EntryVehicleLicense, sizeof(record.EntryVehicleLicense));
        DataBaseRecordReadInt(sql_stmt, 34, record.EntryVLColor);
        DataBaseRecordReadInt(sql_stmt, 35, record.EntryVehicleClass);
		DataBaseRecordReadInt(sql_stmt, 36, record.EntryVehicleType);
		record.IsDataBaseRecord = 1;
        sqlite3_agent::sqlite3_finalize_fun(sql_stmt);
    }
    else
    {
        sqlite3_agent::sqlite3_finalize_fun(sql_stmt);
        return -1;
    }

    return 0;
}



int DataBaseDelete(sqlite3*sql_handle, const DebitCancleParameter&record)
{
    if (sql_handle == 0)
    {
        return -1;
    }
    std::stringstream ss;
    ss << "delete  from " << DataBaseGetDebitCancleTableName();
    ss <<  " where PayIdentifier = '"  << record.PayIdentifier << "';";
    std::string delete_sql = ss.str();
    int nRes =  sqlite3_agent::sqlite3_exec_fun(sql_handle, delete_sql.c_str(), 0, 0, 0);
    if (nRes != SQLITE_OK)
    {
        return -1;
    }
    return 0;
}


int DataBaseDelete(sqlite3* sql_handle, const InvoiceCancelParameter&record)
{
    if (sql_handle == 0)
    {
        return -1;
    }
    std::stringstream ss;
    ss << "delete  from " << DataBaseGetInvoiceCancleTableName();
    ss <<  " where InvoiceNo = '"  << record.ElectronicInvoiceNo << "';";
    std::string delete_sql = ss.str();
    int nRes =  sqlite3_agent::sqlite3_exec_fun(sql_handle, delete_sql.c_str(), 0, 0, 0);
    if (nRes != SQLITE_OK)
    {
        return -1;
    }
    return 0;
}

int DataBaseDelete(sqlite3* sql_handle, const InvoiceParameter&record)
{
	if (sql_handle == 0)
    {
        return -1;
    }
    std::stringstream ss;
    ss << "delete  from " << DataBaseGetInvoiceTableName();
    ss <<  " where ElectronicInvoiceNo = '"  << record.ElectronicInvoiceNo << "';";
    std::string delete_sql = ss.str();
    int nRes =  sqlite3_agent::sqlite3_exec_fun(sql_handle, delete_sql.c_str(), 0, 0, 0);
    if (nRes != SQLITE_OK)
    {
        return -1;
    }
    return 0;
}


std::string DataBaseGetDebitRecordTableDefine()
{
    std::string table_sql = "create table Archive_DebitRecord(";
    table_sql += "PayIdentifier varchar(64),";
    table_sql += "ExecuteCode integer,";
    table_sql += "ServerStatus integer,";
    table_sql += "TradeSecurityCode varchar(64),";
    table_sql += "PayPlatformType integer,";
    table_sql += "DebitOrder varchar(64),";
    table_sql += "DebitTime varchar(64),";
    table_sql += "PaySerialNo varchar(64),";
    table_sql += "Money integer,";
    table_sql += "OBUID varchar(64),";
    table_sql += "OBUPlate varchar(30),";
    table_sql += "OBUPlateColor integer,";
    table_sql += "CardID varchar(64),";
    table_sql += "Version integer,";
    table_sql += "PRIMARY KEY(PayIdentifier)";
    table_sql += ")";
    return table_sql;
}

std::string DataBaseGetDebitQuestTableDefine()
{
    return "create table Cache_DebitRequest (PayIdentifier varchar(64),Version integer , DebitPara blob,PRIMARY KEY(PayIdentifier));";
}
std::string DataBaseGetDebitRecordTableName()
{
    return "Archive_DebitRecord";
}
std::string DataBaseGetDebitRequestTableName()
{
    return "Cache_DebitRequest";
}


std::string DataBaseGetDebitCancleTableName()
{
    return "Cache_DebitCancle";
}


std::string DataBaseGetInvoiceCancleTableName()
{
    return "Cache_InvoiceCancel";
}

std::string DataBaseGetInvoiceTableName()
{
    return "Cache_Invoice";
}


std::string DataBaseGetDebitCancleTableDefine()
{
    std::string table_sql = "create table Cache_DebitCancle(";
    table_sql += "PayIdentifier varchar(64),";
    table_sql += "ProvinceID integer,";
    table_sql += "VerifyCode varchar(64),";
    table_sql += "CreateTime varchar(64),";
    table_sql += "PRIMARY KEY(PayIdentifier)";
    table_sql += ")";
    return table_sql;
}

//发票撤销
std::string DataBaseGetInvoiceCancleTableDefine()
{
	std::string table_sql = "create table Cache_InvoiceCancel(";
    table_sql += "InvoiceNo varchar(64),";
    table_sql += "PayProvinceId varchar(8),";
    table_sql += "ExitTime varchar(64),";
    table_sql += "PRIMARY KEY(InvoiceNo)";
    table_sql += ")";
    return table_sql;
}

//发票上传
std::string DataBaseGetInvoiceTableDefine()
{
	std::string table_sql = "create table Cache_Invoice(";
    table_sql += "ElectronicInvoiceNo varchar(64),";
    table_sql += "DebitOrderNumber varchar(48),";
    table_sql += "Money integer,";
	table_sql += "PayMode integer,";
	table_sql += "VehicleLicense varchar(16),";
	table_sql += "VehicleColor integer,";
	table_sql += "VehicleType integer,";
	table_sql += "VehicleClass integer,";
	table_sql += "AxisCount integer,";
	table_sql += "TotalWeight integer,";
	table_sql += "LimitWeight integer,";
	table_sql += "PayAreaID varchar(64),";
	table_sql += "PayProvinceId varchar(16),";
	table_sql += "PayRoadID varchar(32),";
	table_sql += "PayStationID varchar(32),";
	table_sql += "PayStationName varchar(64),";
	table_sql += "PayLane varchar(32),";
	table_sql += "ExitTime varchar(64),";
	table_sql += "nExitTime integer,";
	table_sql += "PassCertificateType integer,";
	table_sql += "PassCertificateCode varchar(64),";
	table_sql += "TollDate varchar(64),";
	table_sql += "ShiftID integer,";
	table_sql += "OperatorID varchar(32),";
	table_sql += "OperatorName varchar(32),";
	table_sql += "Distance integer,";
	table_sql += "EntryAreaID varchar(64),";
	table_sql += "EntryRoadID varchar(32),";
	table_sql += "EntryStationID varchar(32),";
	table_sql += "EntryStationName varchar(32),";
	table_sql += "EntryLaneID varchar(16),";
	table_sql += "EntryTime varchar(64),";
	table_sql += "nEntryTime integer,";
	table_sql += "EntryVehicleLicense varchar(16),";
	table_sql += "EntryVLColor integer,";
	table_sql += "EntryVehicleClass integer,";
	table_sql += "EntryVehicleType integer,";
    table_sql += "PRIMARY KEY(ElectronicInvoiceNo)";
    table_sql += ")";
    return table_sql;
}

int DataBaseRecordReadInt(sqlite3_stmt* sql_stmt, int pos, int&val)
{
    if (sql_stmt == 0)
    {
        return -1;
    }
    val = sqlite3_agent::sqlite3_column_int_fun(sql_stmt, pos);
    return 0;
}
int DataBaseRecordReadVarchar(sqlite3_stmt* sql_stmt, int pos, char* dest, int dest_size)
{
    if (sql_stmt == 0 || dest_size < 2)
    {
        return -1;
    }
    const unsigned char* var_char = sqlite3_agent::sqlite3_column_text_fun(sql_stmt, pos);
    strncpy(dest, (const char*)var_char, dest_size - 1);
    return 0;
}

int DataBaseRecordReadTimet(sqlite3_stmt* sql_stmt, int pos, time_t& val)
{
    if (sql_stmt == 0)
    {
        return -1;
    }
    char tmp_time_str[30] = {0};
    const unsigned char* var_char = sqlite3_agent::sqlite3_column_text_fun(sql_stmt, pos);
    strncpy(tmp_time_str, (const char*)var_char, 20);
    val = Str2Datetime(tmp_time_str);
    return 0;
}

}
