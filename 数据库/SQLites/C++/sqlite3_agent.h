#ifndef SQLITE3_AGENT_H
#define SQLITE3_AGENT_H
#include "sqlite3.h"
#include <windows.h>

typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_finalize_api)(sqlite3_stmt *pStmt);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_prepare_v2_api)(sqlite3 *db,  const char *zSql,  int nByte, sqlite3_stmt **ppStmt, const char **pzTail);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_exec_api)(sqlite3*,  const char *sql,  int (*callback)(void*,int,char**,char**), void *, char **errmsg   );
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_errcode_api)(sqlite3 *db);
typedef SQLITE_API const char *SQLITE_STDCALL (*sqlite3_errmsg_api)(sqlite3*);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_open_api)(const char *filename, sqlite3 **ppDb);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_open_v2_api)(const char *filename,   sqlite3 **ppDb,  int flags,   const char *zVfs    );
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_close_api)(sqlite3*);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_close_v2_api)(sqlite3*);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_changes_api)(sqlite3*);
typedef SQLITE_API const char *SQLITE_STDCALL (*sqlite3_sql_api)(sqlite3_stmt *pStmt);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_step_api)(sqlite3_stmt*);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_column_count_api)(sqlite3_stmt *pStmt);
typedef SQLITE_API const char *SQLITE_STDCALL (*sqlite3_column_name_api)(sqlite3_stmt*, int N);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_column_type_api)(sqlite3_stmt*, int iCol);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_column_int_api)(sqlite3_stmt*, int iCol);
typedef SQLITE_API sqlite3_int64 SQLITE_STDCALL (*sqlite3_column_int64_api)(sqlite3_stmt*, int iCol);
typedef SQLITE_API sqlite3_int64 SQLITE_STDCALL (*sqlite3_memory_used_api)(void);
typedef SQLITE_API const unsigned char *SQLITE_STDCALL (*sqlite3_column_text_api)(sqlite3_stmt*, int iCol);
typedef SQLITE_API double SQLITE_STDCALL (*sqlite3_column_double_api)(sqlite3_stmt*, int iCol);
typedef SQLITE_API void SQLITE_STDCALL (*sqlite3_free_api)(void*);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_prepare_api)(sqlite3 *db, const char *zSql,int nByte,sqlite3_stmt **ppStmt, const char **pzTail);
typedef SQLITE_API const char *SQLITE_STDCALL (*sqlite3_column_origin_name_api)(sqlite3_stmt*,int);

typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_bind_blob_api)(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
typedef SQLITE_API const void *SQLITE_STDCALL (*sqlite3_column_blob_api)(sqlite3_stmt*, int iCol);
typedef SQLITE_API int SQLITE_STDCALL (*sqlite3_column_bytes_api)(sqlite3_stmt*, int iCol);

/**
 * 在合适的地方调用LoadSqlite，加载sqlite动态库
 * 在不再使用sqlite的时候，调用UnLoadSqlite
 */

class sqlite3_agent
{
public:
    sqlite3_agent();
    ~sqlite3_agent();
public:
    static void LoadSqlite(const char* path);
    static void UnloadSqlite();
    static SQLITE_API int SQLITE_STDCALL sqlite3_finalize_fun(sqlite3_stmt *pStmt);
    static SQLITE_API int SQLITE_STDCALL sqlite3_prepare_v2_fun(sqlite3 *db,  const char *zSql,  int nByte, sqlite3_stmt **ppStmt, const char **pzTail);
    static SQLITE_API int SQLITE_STDCALL sqlite3_exec_fun(sqlite3*,  const char *sql,  int (*callback)(void*,int,char**,char**), void *, char **errmsg   );
    static SQLITE_API int SQLITE_STDCALL sqlite3_errcode_fun(sqlite3 *db);
    static SQLITE_API const char *SQLITE_STDCALL sqlite3_errmsg_fun(sqlite3*);
    static SQLITE_API int SQLITE_STDCALL sqlite3_open_fun(const char *filename, sqlite3 **ppDb);
    static SQLITE_API int SQLITE_STDCALL sqlite3_open_v2_fun(const char *filename,   sqlite3 **ppDb,  int flags,   const char *zVfs    );
    static SQLITE_API int SQLITE_STDCALL sqlite3_close_fun(sqlite3*);
    static SQLITE_API int SQLITE_STDCALL sqlite3_close_v2_fun(sqlite3*);
    static SQLITE_API int SQLITE_STDCALL sqlite3_changes_fun(sqlite3*);
    static SQLITE_API const char *SQLITE_STDCALL sqlite3_sql_fun(sqlite3_stmt *pStmt);
    static SQLITE_API int SQLITE_STDCALL sqlite3_step_fun(sqlite3_stmt*);
    static SQLITE_API int SQLITE_STDCALL sqlite3_column_count_fun(sqlite3_stmt *pStmt);
    static SQLITE_API const char *SQLITE_STDCALL sqlite3_column_name_fun(sqlite3_stmt*, int N);
    static SQLITE_API int SQLITE_STDCALL sqlite3_column_type_fun(sqlite3_stmt*, int iCol);
    static SQLITE_API int SQLITE_STDCALL sqlite3_column_int_fun(sqlite3_stmt*, int iCol);
    static SQLITE_API sqlite3_int64 SQLITE_STDCALL sqlite3_column_int64_fun(sqlite3_stmt*, int iCol);
    static SQLITE_API sqlite3_int64 SQLITE_STDCALL sqlite3_memory_used_fun(void);
    static SQLITE_API const unsigned char *SQLITE_STDCALL sqlite3_column_text_fun(sqlite3_stmt*, int iCol);
    static SQLITE_API double SQLITE_STDCALL sqlite3_column_double_fun(sqlite3_stmt*, int iCol);
    static SQLITE_API void SQLITE_STDCALL sqlite3_free_fun(void*);
    static SQLITE_API int SQLITE_STDCALL sqlite3_prepare_fun(sqlite3 *db, const char *zSql,int nByte,sqlite3_stmt **ppStmt, const char **pzTail);
    static SQLITE_API const char *SQLITE_STDCALL sqlite3_column_origin_name_fun(sqlite3_stmt*,int);

    static SQLITE_API int SQLITE_STDCALL sqlite3_bind_blob_fun(sqlite3_stmt*, int, const void*, int n, void(*)(void*));
    static SQLITE_API const void *SQLITE_STDCALL sqlite3_column_blob_fun(sqlite3_stmt*, int iCol);
    static SQLITE_API int SQLITE_STDCALL sqlite3_column_bytes_fun(sqlite3_stmt*, int iCol);
private:
    static HMODULE g_sqlite_module;
    static sqlite3_finalize_api sqlite3_finalize_dll;
    static sqlite3_prepare_v2_api sqlite3_prepare_v2_dll;
    static sqlite3_exec_api sqlite3_exec_dll;
    static sqlite3_errcode_api sqlite3_errcode_dll;
    static sqlite3_errmsg_api sqlite3_errmsg_dll;
    static sqlite3_open_api sqlite3_open_dll;
    static sqlite3_open_v2_api sqlite3_open_v2_dll;
    static sqlite3_close_v2_api sqlite3_close_v2_dll;
    static sqlite3_close_api    sqlite3_close_dll;
    static sqlite3_changes_api sqlite3_changes_dll;
    static sqlite3_sql_api sqlite3_sql_dll;
    static sqlite3_step_api sqlite3_step_dll;
    static sqlite3_column_count_api sqlite3_column_count_dll;
    static sqlite3_column_name_api sqlite3_column_name_dll;
    static sqlite3_column_type_api sqlite3_column_type_dll;
    static sqlite3_column_int_api sqlite3_column_int_dll;
    static sqlite3_column_int64_api sqlite3_column_int64_dll;
    static sqlite3_memory_used_api sqlite3_memory_used_dll;
    static sqlite3_column_text_api sqlite3_column_text_dll;
    static sqlite3_column_double_api sqlite3_column_double_dll;
    static sqlite3_free_api  sqlite3_free_dll;
    static sqlite3_prepare_api sqlite3_prepare_dll;
    static sqlite3_column_origin_name_api sqlite3_column_origin_name_dll;
    static sqlite3_bind_blob_api sqlite3_bind_blob_dll;
    static sqlite3_column_blob_api sqlite3_column_blob_dll;
    static sqlite3_column_bytes_api sqlite3_column_bytes_dll;
private:
    sqlite3_agent(const sqlite3_agent&);
    sqlite3_agent& operator = (const sqlite3_agent&);

};

#endif // SQLITE3_AGENT_H
