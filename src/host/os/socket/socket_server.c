/******************************************************************************
* Copyright (c) 2017, WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName socket_server.c
* Description :socket server
* Author    : ruibin.zhang
* Reviewer  :
* Date      : 2017-03-01
* Record    :
*
******************************************************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#include "socket_server.h"
#include "log/log.h"

volatile int socketSfd  = -1;

/*****************************************************************************
  Function:     socketS_open
  Description:  socket server init
  Input:        none
  Output:       none
  Return:       SOCKETSErrCode
  Author:       ruibin.zhang
*****************************************************************************/
SOCKETSErrCode socketS_open(void)
{
    if(socketSfd > 0)
    {
       LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SOCKET, "socketSfd already open!");
       return WV_SOCKETS_SUCCESS;
    }

    socketSfd = socket(AF_INET, SOCK_STREAM , IPPROTO_IP);// IPv4 | 有保证的socket类型 | 比较开放，校验与验证协议自己写
    if(socketSfd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SOCKET, "socketSfd open error: %s(errno: %d)!",strerror(errno),errno);
        return WV_SOCKETS_OPEN_ERR;
    }

    return WV_SOCKETS_SUCCESS;
}

/*****************************************************************************
  Function:     socketS_bind
  Description:  bind socket server
  Input:        none
  Output:       none
  Return:       SOCKETSErrCode
  Author:       ruibin.zhang
*****************************************************************************/
SOCKETSErrCode socketS_bind(void)
{
    if(socketSfd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SOCKET, "socketSfd open error!");
        return WV_SOCKETS_OPEN_ERR;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);       //IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons(WV_SERVER_DEFAULT_PORT);  //设置的端口为DEFAULT_PORT

    //将本地地址绑定到所创建的套接字上
    if( -1 == bind(socketSfd, (struct sockaddr*)&servaddr, sizeof(servaddr)))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SOCKET, "bind socket error: %s(errno: %d)",strerror(errno),errno);
        return  WV_SOCKETS_BIND_ERR;
    }

    return WV_SOCKETS_SUCCESS;
}

/*****************************************************************************
  Function:     socketS_listen
  Description:  listen socket server
  Input:        none
  Output:       none
  Return:       SOCKETSErrCode
  Author:       ruibin.zhang
*****************************************************************************/
SOCKETSErrCode socketS_listen(void)
{
    if(socketSfd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SOCKET, "socketSfd open error!");
        return WV_SOCKETS_OPEN_ERR;
    }

    if(-1 == listen(socketSfd, WV_SERVER_MAX_CONNECT_NUM))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SOCKET, "listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        return WV_SOCKETS_LISTEN_ERR;
    }

    return WV_SOCKETS_SUCCESS;
}
