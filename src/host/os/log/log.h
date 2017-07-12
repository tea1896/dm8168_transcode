#ifndef _INCLUDE_LOG
#define _INCLUDE_LOG

#include "appGlobal.h"

#define TEST_LOG    			 (0)

#define LOG_SUCCESS              (0)
#define LOG_ERROR                (-1)

#define LOG_FILE_PATH            ("/tmp/log.txt")
#define LOG_FILE_SIZE            (4 * 1024) //4 M

#define MAX_COMMAND_LEN          (256)
#define MAX_FILENAME_LEN         (256)

#define LOG_LINE_MAX_LEN		 (256)
#define LOG_LEVEL_NAME_MAX_LEN   (50)
#define LOG_MODULE_NAME_MAX_LEN  (50)


typedef enum
{
	LOG_LEVEL_ALL      =      (0xFF),
	LOG_LEVEL_DEBUG    =      (0x1 << 0),
	LOG_LEVEL_INFO     =      (0x1 << 1),
	LOG_LEVEL_NOTICE   =      (0x1 << 2),
	LOG_LEVEL_WARNING  =      (0x1 << 3),
	LOG_LEVEL_ERROR    =      (0x1 << 4),
}LogLevel_EM;

typedef enum
{
	LOG_OUTPUT_ALL       =      (0xFF),
	LOG_OUTPUT_CONSOLE   =      (0x1 << 0),
	LOG_OUTPUT_SYSLOG    =      (0x1 << 1),
	LOG_OUTPUT_MAINBOARD =      (0x1 << 2),
}LogOutput_EM;


typedef enum
{
	LOG_MODULE_ALL       =      (0xFFFFFFFF),
	LOG_MODULE_INIT      =      (0x1 << 0),
	LOG_MODULE_COMM      =      (0x1 << 1),
	LOG_MODULE_PCM       =      (0x1 << 2),
	LOG_MODULE_ENCODER   =      (0x1 << 3),
	LOG_MODULE_MUXER     =      (0x1 << 4),
	LOG_MODULE_SOCKET    =      (0x1 << 5),
	LOG_MODULE_UPDATA    =      (0x1 << 6),
	LOG_MODULE_FPGA      =      (0x1 << 7),
	LOG_MODULE_SYS       =      (0x1 << 8),
	LOG_MODULE_GPIO      =      (0x1 << 9),
	LOG_MODULE_ADV7611   =      (0x1 << 10),
	LOG_MODULE_I2C       =      (0x1 << 11),
    LOG_MODULE_UART      =      (0x1 << 12),
    LOG_MODULE_M22       =      (0x1 << 13),
	LOG_MODULE_TEMP      =      (0x1 << 14),
    LOG_MODULE_PARAMS    =      (0x1 << 15),
	LOG_MODULE_DS2432    =      (0x1 << 16),
    LOG_MODULE_SPI       =      (0x1 << 17),
	LOG_MODULE_WEB       =      (0x1 << 18),
	LOG_MODULE_USER      =      (0x1 << 19),
	LOG_MODULE_TSP       =      (0x1 << 20),
	LOG_MODULE_SIP       =      (0x1 << 21)
}LogModule_EM;


typedef enum
{
	emLogOpenMode,
	emLogCloseMode,
}LogMode_EM;

struct LogCtrl_ST
{
	LogLevel_EM emLogLevel;
	LogModule_EM emLogModule;
	LogOutput_EM emLogOutput;
};


void Log_SetLevel(LogLevel_EM emLogLevel, LogMode_EM emLogMode);
void Log_SetModule(LogModule_EM emLogModule, LogMode_EM emLogMode);
void Log_SetOutputMode(LogOutput_EM emLogOutput, LogMode_EM emLogMode);

LogLevel_EM   Log_GetLevel(void);
LogModule_EM  Log_GetModule(void);
LogOutput_EM  Log_GetOutputMode(void);

S32  Log_Init(char *ps8LogSavePath, U32 u32LogSize);
void Log_Printf(LogLevel_EM emLogLevel, LogModule_EM emLogModule, const char *format, ...);
void Log_Clear(S8 *ps8LogSavePath, U32 u32LogSize);

#define LOG_PRINTF(emLogLevel, emLogModule, format, ...) Log_Printf(emLogLevel, emLogModule, "[%s:%d] " format "\r\n", __func__, __LINE__,##__VA_ARGS__)

#endif
