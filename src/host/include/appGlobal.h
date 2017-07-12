#ifndef APPGLOBAL_H_
#define APPGLOBAL_H_


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/eventfd.h>

#define  SW_VER         0x1


typedef unsigned char           U8;
typedef unsigned short          U16;
typedef unsigned int            U32;
typedef unsigned long long      U64;
typedef signed char             S8;
typedef signed short            S16;
typedef signed int              S32;
typedef long long               S64;

typedef unsigned char volatile          VU8;
typedef unsigned short volatile         VU16;
typedef unsigned int volatile           VU32;
typedef unsigned long volatile          VU64;

typedef unsigned char                   BYTE;
typedef short                           SHORT;
typedef unsigned short                  WORD;
typedef int                             LONG;
typedef unsigned int                    DWORD;
typedef char                            BOOL;

#define FALSE                           0
#define TRUE                            1



#define MAX_SUPPORT_TRANS_NUM           12  // 能够支持的最大转码路数
#define MAX_SUPPORT_TRANS_SD_NUM        12  // 能够支持的最大标清路数 - 不建议使用
#define MAX_SUPPORT_TRANS_HD_NUM        4   // 能够支持的最大高清路数
#define MAX_SUPPORT_STREAM_NUM_PROGRAM   3   // 每个节目能支持的最大音频数量

#define OS_MAX_LINE_LEN              1024    // 系统支持的最大一行字符串长度
#define OS_MAX_FILE_NAME_LEN         1024    // 系统支持的最大文件名长度
#define OS_INVALID_FD                  (-1)  // 无效的文件描述符

#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define MIX(x,y) (((x) < (y)) ? (x) : (y))
#define ARRAY_SIZE(arr) (sizeof((arr))/sizeof((arr[0])))




#endif



