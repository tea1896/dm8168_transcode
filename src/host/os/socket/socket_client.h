/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName socket_client.h
* Description :socket client
* Author    : ruibin.zhang
* Reviewer  :
* Date      : 2017-03-01
* Record    :
*
******************************************************************************/
#ifndef __SOCKET_CLIENT_H__
#define __SOCKET_CLIENT_H__

#include "appGlobal.h"

typedef enum{
    WV_SOCKETC_SUCCESS                         =               0X0,

    WV_SOCKETC_PARAM_ERR                       =               0X1,

    WV_SOCKETC_OPEN_ERR                        =               0X2,
    WV_SOCKETC_ALREADY_OPEN_ERR                =               0X3,

    WV_SOCKETC_BIND_ERR                        =               0X4,

    WV_SOCKETC_LISTEN_ERR                      =               0X5,

}SOCKETCErrCode;

SOCKETCErrCode socketC_open(S32 * pu32socketCfd);
SOCKETCErrCode socketC_connect(S32 u32socketCfd, U32 u32IPAddr);

#endif
