/*****************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName WV_log.h
* Description : log module
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-20
* Record    :
*
*****************************************************************************/

#ifndef WV_LOG_H
#define WV_LOG_H

#include "AppGlobal.h"

#define LOG_PROJECT_NAME "[4U_BB]"

//TODO 根据项目要求，更改下面宏定义，或者用参数实现
#define LOG_SAVE_FILE           WELLAV_DATA"log/wellav.log"
#define LOG_SAVE_SIZE           1024

#define LOG_MAX_LENGTH          256
#define LOG_SWITCH              (0x1 << 31)
#define MAX_MODULE_NAME_LEN     30

typedef enum{
    LOG_LEVEL_ALL           = 0xF,
    LOG_LEVEL_DEBUG         = (0x1 << 0),
    LOG_LEVEL_ALARM         = (0x1 << 1),
    LOG_LEVEL_ERROR         = (0x1 << 2),
    LOG_LEVEL_EMERGENCY     = (0x1 << 3)
}Loglevel;

typedef enum{
    LOG_OUTPUT_ALL          = (0x30),
    LOG_OUTPUT_CONSOLE      = (0x1 << 4),
    LOG_OUTPUT_SYSLOG       = (0x1 << 5)
}LogChoice;

//TODO 根据项目需要，增加模块名字，增加了模块后，必须在c文件中增加相应的打印名字
//TODO 模块不能超过27个
typedef enum{
    LOG_MODULE_ALL          = (0x7FFFFFC0),
    LOG_MODULE_SYS          = (0x1 << 6),
    LOG_MODULE_INIT         = (0x1 << 7),
    LOG_MODULE_FPGA         = (0x1 << 8),
    LOG_MODULE_GPIO         = (0x1 << 9),
    //取反，方便控制位关掉该寄存器
    LOG_MODULE_SYS_F          = ~(0x1 << 6),
    LOG_MODULE_INIT_F         = ~(0x1 << 7),
    LOG_MODULE_FPGA_F         = ~(0x1 << 8),
    LOG_MODULE_GPIO_F         = ~(0x1 << 9),
}LogMoudles;

void log_Init(U8 * pu8LogSavepath, U32 u32LogSavesize);
void log_printf(Loglevel u32Loglevel, LogMoudles u32Modulemask, const char *format,...);
void log_TimeMark(const char *format,...);
void log_Control(U32 u32Log);
void log_ClearLog(U8 * pu8LogSavepath, U32 u32LogSavesize);

#define LOG_PRINTF(u32Loglevel, u32Modulemask, format,...) log_printf(u32Loglevel, u32Modulemask, "[%s:%d] " format "\r\n", __FUNCTION__, __LINE__, ## __VA_ARGS__)

#endif
