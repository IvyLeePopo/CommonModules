#include "sqlite3_agent.h"



HMODULE sqlite3_agent::g_sqlite_module = NULL;
sqlite3_finalize_api sqlite3_agent::sqlite3_finalize_dll = NULL;
sqlite3_prepare_v2_api sqlite3_agent::sqlite3_prepare_v2_dll = NULL;
sqlite3_exec_api sqlite3_agent::sqlite3_exec_dll = NULL;
sqlite3_errcode_api sqlite3_agent::sqlite3_errcode_dll = NULL;
sqlite3_errmsg_api sqlite3_agent::sqlite3_errmsg_dll = NULL;
sqlite3_open_api sqlite3_agent::sqlite3_open_dll = NULL;
sqlite3_open_v2_api sqlite3_agent::sqlite3_open_v2_dll = NULL;
sqlite3_close_v2_api sqlite3_agent::sqlite3_close_v2_dll = NULL;
sqlite3_close_api    sqlite3_agent::sqlite3_close_dll = NULL;
sqlite3_changes_api sqlite3_agent::sqlite3_changes_dll = NULL;
sqlite3_sql_api sqlite3_agent::sqlite3_sql_dll = NULL;
sqlite3_step_api sqlite3_agent::sqlite3_step_dll = NULL;
sqlite3_column_count_api sqlite3_agent::sqlite3_column_count_dll = NULL;
sqlite3_column_name_api sqlite3_agent::sqlite3_column_name_dll = NULL;
sqlite3_column_type_api sqlite3_agent::sqlite3_column_type_dll = NULL;
sqlite3_column_int_api sqlite3_agent::sqlite3_column_int_dll = NULL;
sqlite3_column_int64_api sqlite3_agent::sqlite3_column_int64_dll = NULL;
sqlite3_memory_used_api sqlite3_agent::sqlite3_memory_used_dll = NULL;
sqlite3_column_text_api sqlite3_agent::sqlite3_column_text_dll = NULL;
sqlite3_column_double_api sqlite3_agent::sqlite3_column_double_dll = NULL;
sqlite3_free_api  sqlite3_agent::sqlite3_free_dll = NULL;
sqlite3_prepare_api sqlite3_agent::sqlite3_prepare_dll = NULL;
sqlite3_column_origin_name_api sqlite3_agent::sqlite3_column_origin_name_dll = NULL;

sqlite3_bind_blob_api sqlite3_agent::sqlite3_bind_blob_dll = NULL;
sqlite3_column_blob_api sqlite3_agent::sqlite3_column_blob_dll = NULL;
sqlite3_column_bytes_api sqlite3_agent::sqlite3_column_bytes_dll = NULL;


void sqlite3_agent::LoadSqlite(const char* path)
{
    g_sqlite_module = LoadLibrary(path);

    sqlite3_finalize_dll =  (sqlite3_finalize_api)::GetProcAddress(g_sqlite_module, "sqlite3_finalize");
    sqlite3_prepare_v2_dll =  (sqlite3_prepare_v2_api)::GetProcAddress(g_sqlite_module, "sqlite3_prepare_v2");
    sqlite3_exec_dll =  (sqlite3_exec_api)::GetProcAddress(g_sqlite_module, "sqlite3_exec");
    sqlite3_errcode_dll =  (sqlite3_errcode_api)::GetProcAddress(g_sqlite_module, "sqlite3_errcode");
    sqlite3_errmsg_dll =  (sqlite3_errmsg_api)::GetProcAddress(g_sqlite_module, "sqlite3_errmsg");
    sqlite3_open_dll =  (sqlite3_open_api)::GetProcAddress(g_sqlite_module, "sqlite3_open");
    sqlite3_open_v2_dll =  (sqlite3_open_v2_api)::GetProcAddress(g_sqlite_module, "sqlite3_open_v2");
    sqlite3_close_v2_dll =  (sqlite3_close_v2_api)::GetProcAddress(g_sqlite_module, "sqlite3_close_v2");
    sqlite3_close_dll =  (sqlite3_close_api)::GetProcAddress(g_sqlite_module, "sqlite3_close");
    sqlite3_changes_dll =  (sqlite3_changes_api)::GetProcAddress(g_sqlite_module, "sqlite3_changes");
    sqlite3_sql_dll =  (sqlite3_sql_api)::GetProcAddress(g_sqlite_module, "sqlite3_sql");
    sqlite3_step_dll =  (sqlite3_step_api)::GetProcAddress(g_sqlite_module, "sqlite3_step");
    sqlite3_column_count_dll =  (sqlite3_column_count_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_count");
    sqlite3_column_name_dll =  (sqlite3_column_name_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_name");
    sqlite3_column_type_dll =  (sqlite3_column_type_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_type");
    sqlite3_column_int_dll =  (sqlite3_column_int_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_int");
    sqlite3_column_int64_dll =  (sqlite3_column_int64_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_int64");
    sqlite3_memory_used_dll =  (sqlite3_memory_used_api)::GetProcAddress(g_sqlite_module, "sqlite3_memory_used");
    sqlite3_column_text_dll =  (sqlite3_column_text_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_text");
    sqlite3_column_double_dll =  (sqlite3_column_double_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_double");
    sqlite3_free_dll =  (sqlite3_free_api)::GetProcAddress(g_sqlite_module, "sqlite3_free");
    sqlite3_prepare_dll =  (sqlite3_prepare_api)::GetProcAddress(g_sqlite_module, "sqlite3_prepare");
    sqlite3_column_origin_name_dll =  (sqlite3_column_origin_name_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_origin_name");

    sqlite3_bind_blob_dll = (sqlite3_bind_blob_api)::GetProcAddress(g_sqlite_module, "sqlite3_bind_blob");
    sqlite3_column_blob_dll = (sqlite3_column_blob_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_blob");
    sqlite3_column_bytes_dll = (sqlite3_column_bytes_api)::GetProcAddress(g_sqlite_module, "sqlite3_column_bytes");
}
void sqlite3_agent::UnloadSqlite()
{
    if (g_sqlite_module != NULL)
    {
        FreeLibrary(g_sqlite_module);
    }
    g_sqlite_module = NULL;
    sqlite3_finalize_dll = NULL;
    sqlite3_prepare_v2_dll = NULL;
    sqlite3_exec_dll = NULL;
    sqlite3_errcode_dll = NULL;
    sqlite3_errmsg_dll = NULL;
    sqlite3_open_dll = NULL;
    sqlite3_open_v2_dll = NULL;
    sqlite3_close_v2_dll = NULL;
    sqlite3_close_dll = NULL;
    sqlite3_changes_dll = NULL;
    sqlite3_sql_dll = NULL;
    sqlite3_step_dll = NULL;
    sqlite3_column_count_dll = NULL;
    sqlite3_column_name_dll = NULL;
    sqlite3_column_type_dll = NULL;
    sqlite3_column_int_dll = NULL;
    sqlite3_column_int64_dll = NULL;
    sqlite3_memory_used_dll = NULL;
    sqlite3_column_text_dll = NULL;
    sqlite3_column_double_dll = NULL;
    sqlite3_free_dll = NULL;
    sqlite3_prepare_dll = NULL;
    sqlite3_column_origin_name_dll = NULL;
}


SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_finalize_fun(sqlite3_stmt *pStmt)
{
    if (sqlite3_finalize_dll == NULL)
    {
        return 0;
    }
    return sqlite3_finalize_dll(pStmt);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_prepare_v2_fun(sqlite3 *db,  const char *zSql,  int nByte, sqlite3_stmt **ppStmt, const char **pzTail)
{
    if (sqlite3_prepare_v2_dll == NULL)
    {
        return 0;
    }
    return sqlite3_prepare_v2_dll(db, zSql,  nByte, ppStmt, pzTail);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_exec_fun(sqlite3*para1,  const char *sql,  int (*callback)(void*, int, char**, char**), void *para4, char **errmsg   )
{
    if (sqlite3_exec_dll == NULL)
    {
        return 0;
    }
    return sqlite3_exec_dll(para1,  sql,  callback, para4, errmsg   );
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_errcode_fun(sqlite3 *db)
{
    if (sqlite3_errcode_dll == NULL)
    {
        return 0;
    }
    return sqlite3_errcode_dll(db);
}
SQLITE_API const char *SQLITE_STDCALL sqlite3_agent::sqlite3_errmsg_fun(sqlite3*para1)
{
    if (sqlite3_errmsg_dll == NULL)
    {
        return NULL;
    }
    return sqlite3_errmsg_dll(para1);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_open_fun(const char *filename, sqlite3 **ppDb)
{
    if (sqlite3_open_dll == NULL)
    {
        return 0;
    }
    return sqlite3_open_dll(filename, ppDb);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_open_v2_fun(const char *filename,   sqlite3 **ppDb,  int flags,   const char *zVfs    )
{
    if (sqlite3_open_v2_dll == NULL)
    {
        return 0;
    }
    return sqlite3_open_v2_dll(filename, ppDb,   flags,   zVfs    );
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_close_fun(sqlite3*para1)
{
    if (sqlite3_close_dll == NULL)
    {
        return 0;
    }
    return sqlite3_close_dll(para1);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_close_v2_fun(sqlite3*para1)
{
    if (sqlite3_close_v2_dll == NULL)
    {
        return 0;
    }
    return sqlite3_close_v2_dll(para1);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_changes_fun(sqlite3*para1)
{
    if (sqlite3_changes_dll == NULL)
    {
        return 0;
    }
    return sqlite3_changes_dll(para1);
}
SQLITE_API const char *SQLITE_STDCALL sqlite3_agent::sqlite3_sql_fun(sqlite3_stmt *pStmt)
{
    if (sqlite3_sql_dll == NULL)
    {
        return NULL;
    }
    return sqlite3_sql_dll(pStmt);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_step_fun(sqlite3_stmt*para1)
{
    if (sqlite3_step_dll == NULL)
    {
        return 0;
    }
    return sqlite3_step_dll(para1);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_column_count_fun(sqlite3_stmt *pStmt)
{
    if (sqlite3_column_count_dll == NULL)
    {
        return 0;
    }
    return sqlite3_column_count_dll(pStmt);
}
SQLITE_API const char *SQLITE_STDCALL sqlite3_agent::sqlite3_column_name_fun(sqlite3_stmt*para1, int N)
{
    if (sqlite3_column_name_dll == NULL)
    {
        return NULL;
    }
    return sqlite3_column_name_dll(para1,  N);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_column_type_fun(sqlite3_stmt*para1, int iCol)
{
    if (sqlite3_column_type_dll == NULL)
    {
        return 0;
    }
    return sqlite3_column_type_dll(para1, iCol);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_column_int_fun(sqlite3_stmt*para1, int iCol)
{
    if (sqlite3_column_int_dll == NULL)
    {
        return 0;
    }
    return sqlite3_column_int_dll(para1,  iCol);
}
SQLITE_API sqlite3_int64 SQLITE_STDCALL sqlite3_agent::sqlite3_column_int64_fun(sqlite3_stmt*para1, int iCol)
{
    if (sqlite3_column_int64_dll == NULL)
    {
        return 0;
    }
    return sqlite3_column_int64_dll(para1,  iCol);
}
SQLITE_API sqlite3_int64 SQLITE_STDCALL sqlite3_agent::sqlite3_memory_used_fun(void)
{
    if (sqlite3_memory_used_dll == NULL)
    {
        return 0;
    }
    return sqlite3_memory_used_dll();
}
SQLITE_API const unsigned char *SQLITE_STDCALL sqlite3_agent::sqlite3_column_text_fun(sqlite3_stmt*para1, int iCol)
{
    if (sqlite3_column_text_dll == NULL)
    {
        return NULL;
    }
    return sqlite3_column_text_dll(para1,  iCol);
}
SQLITE_API double SQLITE_STDCALL sqlite3_agent::sqlite3_column_double_fun(sqlite3_stmt*para1, int iCol)
{
    if (sqlite3_column_double_dll == NULL)
    {
        return 0.0;
    }
    return sqlite3_column_double_dll(para1,  iCol);
}
SQLITE_API void SQLITE_STDCALL sqlite3_agent::sqlite3_free_fun(void*para1)
{
    if (sqlite3_free_dll == NULL)
    {
        return ;
    }
    return sqlite3_free_dll(para1);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_prepare_fun(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail)
{
    if (sqlite3_prepare_dll == NULL)
    {
        return 0;
    }
    return sqlite3_prepare_dll(db, zSql, nByte, ppStmt, pzTail);
}
SQLITE_API const char *SQLITE_STDCALL sqlite3_agent::sqlite3_column_origin_name_fun(sqlite3_stmt*para1, int para2)
{
    if (sqlite3_column_origin_name_dll == NULL)
    {
        return NULL;
    }
    return sqlite3_column_origin_name_dll(para1, para2);
}

SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_bind_blob_fun(sqlite3_stmt*para_1, int para_2, const void* para_3, int para_4, void(*para_5)(void*))
{
     if (sqlite3_bind_blob_dll == NULL)
    {
        return -1;
    }
    return sqlite3_bind_blob_dll(para_1, para_2,para_3,para_4,para_5);
}
SQLITE_API const void *SQLITE_STDCALL sqlite3_agent::sqlite3_column_blob_fun(sqlite3_stmt* stmt, int iCol)
{
    if(sqlite3_column_blob_dll ==NULL)
    {
        return NULL;
    }
    return sqlite3_column_blob_dll(stmt,iCol);
}
SQLITE_API int SQLITE_STDCALL sqlite3_agent::sqlite3_column_bytes_fun(sqlite3_stmt*stmt, int iCol)
{
    if(sqlite3_column_bytes_dll ==NULL)
    {
        return NULL;
    }
    return sqlite3_column_bytes_dll(stmt,iCol);
}