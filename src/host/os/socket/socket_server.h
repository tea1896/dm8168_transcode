/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName socket_server.h
* Description :socket server
* Author    : ruibin.zhang
* Reviewer  :
* Date      : 2017-03-01
* Record    :
*
******************************************************************************/
#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#define WV_SERVER_DEFAULT_PORT      8000
#define WV_SERVER_MAX_CONNECT_NUM   16

typedef enum{
    WV_SOCKETS_SUCCESS                         =               0X0,

    WV_SOCKETS_PARAM_ERR                       =               0X1,

    WV_SOCKETS_OPEN_ERR                        =               0X2,
    WV_SOCKETS_ALREADY_OPEN_ERR                =               0X3,

    WV_SOCKETS_BIND_ERR                        =               0X4,

    WV_SOCKETS_LISTEN_ERR                      =               0X5,

}SOCKETSErrCode;

SOCKETSErrCode socketS_open(void);
SOCKETSErrCode socketS_bind(void);
SOCKETSErrCode socketS_listen(void);

#endif
