/*****************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName WV_err.h
* Description : 返回错误string 模块
* Author    : ruibin.zhang
* Modified  :
* Reviewer  :
* Date      : 2017-02-28
* Record    :
*
*****************************************************************************/
#ifndef WV_ERR_H
#define WV_ERR_H

#define MAX_ERR_INFO_LEN    100
#define MAX_LANGUAGE_NUM    5
#define MAX_ERR_CODE_NUM    1024
#define INVALID_ERRCODE     0xFFFFFFFF

/* language menu */
typedef enum{
    EN_LANGUAGE_EN,     /* 英文 */
    EN_LANGUAGE_CN,     /* 中文 */
    EN_LANGUAGE_MAX     /* 标记 */
}wvLanguage;


/*  error code */
typedef enum{
    WV_SUCCESS                      = 0,
    WV_ERR_FAILURE                  = 0x1,
    WV_ERR_CONFIG_TIMEOUOT          = 0x2,
    WV_ERR_VER                      = 0x3,
    WV_ERR_PARAMS                   = 0x4,
    WV_ERR_CRC                      = 0x5,
    WV_ERR_FILE_NOT_EXISTING        = 0x6,      // 文件不存在
    WV_ERR_FILE_CAN_NOT_READE       = 0x7,      // 文件不可读
    WV_ERR_MALLOC_FALIURE           = 0x8,
    WV_ERR_UPGRADE_CHECK_HEADER     = 0x9,      // 升级文件头部校验失败 
    WV_ERR_UPGRADE_CHECK_NET_HEADER = 0xa,      // 校验网络同步失败
    WV_ERR_NO_DNS                   = 0xb,      // 没有DNS信息
    WV_ERR_NO_NETCARD_INFO          = 0xc,      // 没有网卡信息
    WV_ERR_PARAMS_BY_MODIFY         = 0xd,      // 参数已经被修改过
    WV_ERR_REALTIME_STREAM_BANORMAL = 0xe,      // 实时码流异常
    WV_ERR_PARAMS_MIGRATION         = 0xf,      // 数据需要迁移
    WV_ERR_HARDWAREVER              = 0x10,     // 硬件版本不匹配
    WV_ERR_TEMPERATURE              = 0x11,     // 设备温度过高
    WV_ERR_TAR_UPGRADE_FILE_ERROR   = 0x12,     // 解压升级文件失败
    WV_ERR_LIC_EXPIRED              = 0x13,     // 授权过期

    //encoder
    PE_ENCODER_NORMAL               = 0x201,
    WV_ERR_ENCODER_CONFIGTIMEOUT    = 0x202,
    WV_ERR_ENCODER_PARAMETERS_ERR   = 0x203,
    WV_ERR_ENCODER_FIRMWARE_ERR     = 0x204,
    WV_ERR_ENCODER_NOT_LOCK_SIGNAL  = 0x205,    // 编码没有锁定信号
    WV_ERR_ENCODER_STATUS_ERR       = 0x206,
     
    //web   
    WV_ERR_WEB_NOTEXIST             = 0x302,    //不存在
    WV_ERR_WEB_MALLOC               = 0x303,    //开辟内存失败
    WV_ERR_WEB_GETPE                = 0x304,    //获取PE参数失败
    WV_ERR_WEB_SETPE                = 0x305,    //设置PE参数失败
    WV_ERR_WEB_POST_PARAM           = 0x306,    //post的参数有误
    WV_ERR_WEB_BAD_PARAM            = 0x307,    //入参错误
    WV_ERR_WEB_USER_AUTH            = 0x308,    //用户认证失败
    WV_ERR_WEB_GET_PLAYER_ERR       = 0x309,    //获取播放器失败
    WV_ERR_WEB_GET_PROGRAM_ERR      = 0x30a,    //获取列表节目失败
	WV_ERR_WEB_TARFAIL				= 0x30b,	//打包失败
	WV_ERR_WEB_DIRCREATFAIL 		= 0x30c,	//创建文件夹失败

    //user
    WV_ERR_USER_LEN_ERROR           = 0x402,    //用户名密码，长度错误
    WV_ERR_USER_EXIST               = 0x403,    //用户存在
    WV_ERR_USER_NOT_EXIST           = 0x404,    //用户不存在
    WV_ERR_USER_FULL                = 0x405,    //用户满了
    WV_ERR_USER_OPEN_FILE_ERROR     = 0x406,    //打开文件错误
    WV_ERR_USER_PWD_ERR             = 0x407,    //用户密码错误

    //license


    //comm
    WV_ERR_COMM_PTHREAD_CREAT       = 0x502,    //线程创建失败
    WV_ERR_COMM_UART_RECV_MISS      = 0x503,    //串口接收数据过少
    WV_ERR_COMM_UART_RECV_HEADER    = 0x504,    //串口接收数据头错误
    WV_ERR_COMM_UART_RECV_SLOT      = 0x505,    //串口接收数据槽号错误
    WV_ERR_COMM_IP_RECV_MISS        = 0x506,    //IP接收数据过少
	WV_ERR_COMM_IP_TEMP_FULL		= 0x507,	//IP临时数据满了

	//ip comm
    WV_ERR_SOCKET_OPEN_FAIL         = 0x602,
    WV_ERR_SOCKET_CONNECT_FAIL      = 0x603,
    WV_ERR_IPCOMM_TIME_OUT          = 0x604,

     /* CAM */
     ERROR_CI_CAM_REMOVED            = 0x800,


    //TSP
    WV_ERR_TSP_INPUT_PARAM                    = 0x802,    //输入参数错误
    WV_ERR_TSP_ARRAY_INDEX_OUT_OF_RANGE       = 0x803,    //数组越界
    WV_ERR_TSP_MEMORY_NOT_ENOUGH              = 0x804,    //内存不足
    WV_ERR_TSP_RESOURCE_NOT_ENOUGH            = 0x805,    //没有可分配的TS数组资源
    WV_ERR_TSP_TS_ERROR                       = 0x806,    //TS数据异常
    WV_ERR_TSP_NOT_EXIST                      = 0x807,    //TS数据不存在
    WV_ERR_TSP_FILTER_PSI_TIMEOUT             = 0x808,    //过滤PSI超时
    WV_ERR_TSP_CRC_ERROR                      = 0x809,    //CRC错误
    WV_ERR_TSP_PROG_ALREADY_EXIST             = 0x80A,    //节目已经存在

    
    //PSI
    WV_ERR_SIP_ERROR_BAD_PARAM                = 0x902,    //输入参数错误
    WV_ERR_SIP_ERROR_OUT_OF_RANGE             = 0x903,    //数组越界

    // to add
}wvErrCode;


typedef struct
{
    wvErrCode   enErrCode;
    char        u8ErrEnglishString[MAX_ERR_INFO_LEN];
    char        u8ErrChineseString[MAX_ERR_INFO_LEN];
} ST_PE_ERR;

int err_SetLanguage(wvLanguage enLanguage);
char * err_GetErrCodeString(wvErrCode enErrCode);

#endif






