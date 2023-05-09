/*****************************************************************
 * 版权信息:  
 * 文件名称: SysDefines.h
 * 文件作者:  
 * 完成日期: 
 * 当前版本:
 * 主要功能: 
 * 版本历史:
 *****************************************************************/
#ifndef __SYSDEFINES_H__
#define __SYSDEFINES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#if (defined(_WIN32)) || (defined(_WIN64))
#define IS_WINDOWS 1
#else
#define IS_WINDOWS 0
#endif

#if IS_WINDOWS
#include <Windows.h>
#include <io.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <dlfcn.h>
#include <dirent.h>
#endif

#include <algorithm> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <thread>
#include <mutex>
#include <list>
#include <map>
#include <atomic>
#include <memory>
#include <thread>             // std::thread
#include <chrono>             // std::chrono::seconds
#include <condition_variable> // std::condition_variable, std::cv_status

#if !IS_WINDOWS//linux

typedef unsigned char BYTE;
typedef   signed char CHAR;
typedef unsigned short WORD;
typedef unsigned int DWORD; //note: 64bits linux OS: sizeof(long) = 8
typedef unsigned int UINT;
typedef unsigned long long DWORD64;
typedef signed long long SDWORD64;

typedef int BOOL;
typedef int HANDLE;
typedef int errno_t;
typedef int SOCKET;
typedef unsigned long u_long;

#endif

typedef unsigned char u8;
typedef   signed char s8;
typedef unsigned short u16;
typedef   signed short s16;
typedef unsigned int u32; //note: KEIL C51: sizeof(int) = 2
typedef   signed int s32;
typedef unsigned long long u64;
typedef   signed long long s64;

//
#define COUNTOF(x)  (sizeof(x) / (sizeof(x[0])))
//
#define OFFSET_OF(t, m) ((size_t)&((t *)0)->m)
//
#define SIZEOF_MB(t, m) (sizeof(((t *)0)->m))
//
#define SIZEOF_EXCEPT(t, m) (sizeof(t) - SIZEOF_MB(t, m))
//
#define ALIGN4(x)  ((x + 3u) & ~3u)
#define ALIGN8(x)  ((x + 7u) & ~7u)
#define ALIGN16(x) ((x + 15u) & ~15u)
//
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) > (y) ? (y) : (x))
//
#define MAKE16LE(x)  (x[0] + (x[1] << 8))
#define MAKE16BE(x)  (x[1] + (x[0] << 8))
//
#define MAKE32LE(x)  (x[0] + (x[1] << 8) + (x[2] << 16) + (x[3] << 24))
#define MAKE32BE(x)  (x[3] + (x[2] << 8) + (x[1] << 16) + (x[0] << 24))
//
#define MAKE32P4(a, b, c, d)  ((a << 24) + (b << 16) + (c << 8) + d)

//
#define BUF_END(b)   (b + COUNTOF(b))

//
#define RET_SUCCESS  (0)
#define RET_ERROR    (-1)
//
#define BIT(b)  (1U << (b))


#if !IS_WINDOWS//linux
//
#define MAX_PATH  (260)
#define INVALID_HANDLE_VALUE ((HANDLE)(-1)) 
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)

//
#define USES_CONVERSION
#define A2T(pStr) (pStr)

//
#define HANDLE_IS_VALID(h) ((h) > 0)

#ifndef strncpy_s
#define strncpy_s(pDst, pSrc, szSize) do{\
        size_t szLen = (size_t)(szSize); \
        if(szLen > 0){\
			char *pcDst = (char *)(pDst); \
			const char *pccSrc = (const char *)(pSrc); \
            strncpy(pcDst, pccSrc, szLen - 1);\
            pcDst[szLen - 1] = 0;\
        }\
    }while(0)
    
//
#define sscanf_s(pBuf, szBufSz, ...) sscanf(pBuf, szBufSz, __VA_ARGS__)
//
#define fopen_s(ppFile, pPath, pMode) \
        (errno_t)(0); *(ppFile) = fopen(pPath, pMode);\
//        
#define wcscpy_s(pDst, szSize, pSrc) strncpy_s(pDst, pSrc, szSize)
//
#define wcslen(pStr) strlen(pStr)
#endif

#define EXPORT_DECL_SPEC  __attribute__((visibility("default")))
#define EXPORT_CALL_TYPE  
#else

//
typedef long clockid_t;
//
#define EXPORT_DECL_SPEC  __declspec(dllexport)
#define EXPORT_CALL_TYPE  __stdcall

//
#define HANDLE_IS_VALID(h) (((h) != NULL) && (h != INVALID_HANDLE_VALUE))

#endif

#endif

