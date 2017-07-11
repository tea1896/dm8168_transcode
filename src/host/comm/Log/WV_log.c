/*****************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName WV_log.c
* Description : log module
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-20
* Record    :
*
*****************************************************************************/

#include <semaphore.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
#include <sys/syslog.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "WV_log.h"

U32     g_u32LogControl = 0x0;
bool    g_isLogInited  = false;
bool    g_isSyslogOpen = false;

//TODO 根据项目需要，增加模块名字，以在打印中显示
char log_NameArrayName[][MAX_MODULE_NAME_LEN] = {
    "DEBUG",
    "ALARM",
    "ERROR",   
    "EMERGENCY",   
    "CONSOLE",   
    "LOG",   
    "SYS",   
    "INIT",
    "FPGA",
    "GPIO"
};

/*****************************************************************************
  Function:     log_getdirpath
  Description:  get dir path from filepath
  Input:        filepath    -   file path
  Output:       dirpath     -   dir path
  Return:       none
  Author:       ruibin.zhang
*****************************************************************************/
void log_getdirpath(U8 * dirpath, U8 * filepath)
{
    if((NULL == dirpath) || (NULL == filepath))
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_INIT, "Param is NULL!");
        return ;
    }

    char * ptr;

    ptr = strrchr((char *)filepath, '/');
    if(ptr != NULL)
    {
        strncpy((char *)dirpath, (char *)filepath, (ptr - (char *)filepath));
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_INIT, "log_init Logs save path isn't absolute path");
    }
}

/*****************************************************************************
  Function:     log_Init
  Description:  Initalize log control
  Input:        pu8LogSavepath  -   log file will saved path
                u32LogSavesize  -   log file size (kB)  1024
  Output:       none
  Return:       none
  Author:       dadi.zeng
  Modify:       ruibin.zhang
*****************************************************************************/
void log_Init(U8 * pu8LogSavepath, U32 u32LogSavesize)
{
    if(NULL == pu8LogSavepath)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_INIT,"Param is NULL!");
        return ;
    }

    U8  u8Command[MAX_COMMAND_LENGTH]		=	{0};
    U8  u8LogSavedir[MAX_FILENAME_LENGTH]	=	{0};
    DIR * pdir = NULL;

    log_getdirpath(u8LogSavedir, pu8LogSavepath);

    pdir = opendir((char *)u8LogSavedir);
    if(NULL == pdir)
    {
        g_isSyslogOpen = false;
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_INIT,"%s dir isn't exist!", u8LogSavedir);
    }
    else
    {
        closedir(pdir);
        snprintf((char *)u8Command, sizeof(u8Command),"syslogd -O %s -s %d", pu8LogSavepath, u32LogSavesize);
        system((char *)u8Command);

        openlog(LOG_PROJECT_NAME, LOG_PID , LOG_USER);

        g_isSyslogOpen = true;
    }

    g_u32LogControl      = LOG_LEVEL_ALL | LOG_OUTPUT_ALL | LOG_MODULE_ALL;   // open all prints
    g_isLogInited = true;
}

/*****************************************************************************
  Function:     log_printf
  Description:  Printf information according to setting
  Input:        u32Loglevel     -   print level
                u32Modulemask   -   module mask
                format          -   the content will print
  Output:       none
  Return:       none
  Author:       dadi.zeng
  Modify:       ruibin.zhang
*****************************************************************************/
void log_printf(Loglevel u32Loglevel, LogMoudles u32Modulemask, const char *format,...)
{
    va_list     Argument;
    static U8   u8StringBuff[LOG_MAX_LENGTH];
    U8          u8PrintBuff[LOG_MAX_LENGTH];

    memset(u8StringBuff,0,sizeof(u8StringBuff));

    // 获取待打印的内容
    va_start(Argument, format);
    vsprintf((char *)u8StringBuff, format, Argument);
    va_end(Argument);

    snprintf((char *)u8PrintBuff, sizeof(u8PrintBuff),
            "[%s][%s]%s",
            (char *)log_NameArrayName[(U32)((log(u32Loglevel) / log(2)))],
            (char *)log_NameArrayName[(U32)((log(u32Modulemask) / log(2)))],
            (char *)u8StringBuff);

    // 应对初始化打印，此时LOG控制还未生效
    if((LOG_MODULE_INIT == u32Modulemask) || (false == g_isLogInited))
    {
        printf("%s\n log module isn't initialized \n\n",u8PrintBuff);
        return ;
    }

    // 校验打印控制
    if((u32Loglevel & g_u32LogControl) && (u32Modulemask & g_u32LogControl))
    {
        // 控制台是否打开
        if(LOG_OUTPUT_CONSOLE & g_u32LogControl)
        {
            printf("%s", u8PrintBuff);
        }

        // 是否输入到日志
        if(LOG_OUTPUT_SYSLOG & g_u32LogControl)
        {
            if(true == g_isSyslogOpen)
            {
                syslog(LOG_DEBUG,"%s",u8PrintBuff);
            }
            else
            {
                printf("[%s:%d]Syslogd isn't open!\n", __FUNCTION__, __LINE__);
            }
        }
    }
}

/*****************************************************************************
  Function:     log_TimeMark
  Description:  Record time mark
  Input:        format          -   the content will print
  Output:       none
  Return:       none
  Author:       dadi.zeng
  Modify:       ruibin.zhang
*****************************************************************************/
void log_TimeMark(const char *format,...)
{
    va_list     Argument;
    static U8   u8StringBuff[LOG_MAX_LENGTH];
    U8          u8PrintBuff[LOG_MAX_LENGTH];
    FILE * fp  = NULL;
    char time[MAX_LINE_LENGTH] = {0};

    memset(u8StringBuff,0,sizeof(u8StringBuff));

    // 获取时间
    fp = popen("date", "r");
    fgets(time, MAX_LINE_LENGTH, fp);
    fclose(fp);

    // 获取待打印的内容
    va_start(Argument, format);
    vsprintf((char *)u8StringBuff, format, Argument);
    va_end(Argument);

    // 输出内容
    snprintf((char *)u8PrintBuff, sizeof(u8PrintBuff), "\n%s%s\r\n", time,  (char *)u8StringBuff);

    // 控制台
    printf("%s",u8PrintBuff);

    // 日志
    syslog(LOG_INFO,"%s",u8PrintBuff);
}

/*****************************************************************************
  Function:     log_Control
  Description:  Initalize log control
  Input:        none
  Output:       none
  Return:       none
  Author:       dadi.zeng
  Modify:       ruibin.zhang
*****************************************************************************/
void log_Control(U32 u32Log)
{
    // bit31 = 0 表明为打开 否则为关闭
    if(0 == (LOG_SWITCH & u32Log))
    {
        g_u32LogControl |=  u32Log;
    }
    else
    {
        g_u32LogControl &=  u32Log;
    }
}

/*****************************************************************************
  Function:     log_ClearLog
  Description:  Clear log information
  Input:        pu8LogSavepath  -   log file will saved path
                u32LogSavesize  -   log file size (kB)  1024
  Output:       none
  Return:       none
  Author:       dadi.zeng
  Modify:       ruibin.zhang
*****************************************************************************/
void log_ClearLog(U8 * pu8LogSavepath, U32 u32LogSavesize)
{
    if(NULL == pu8LogSavepath)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS,"Param is NULL!");
        return ;
    }

    FILE * fp = NULL;
    U8   u8Command[MAX_COMMAND_LENGTH];

    bzero((void *)u8Command, sizeof(u8Command));

    // 停止log
    system("syslogd -s 0");

    // 清空
    fp = fopen((char *)pu8LogSavepath, "w+");
    if(NULL == fp)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR,  LOG_MODULE_SYS, "log file can't been empty!\n");
        return ;
    }
    fclose(fp);

    // 重启
    snprintf((char *)u8Command, sizeof(u8Command),"syslogd -O %s -s %d", pu8LogSavepath, u32LogSavesize);
    system((char *)u8Command);

}
