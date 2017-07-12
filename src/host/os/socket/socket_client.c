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
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "socket_client.h"
#include "socket_server.h"
#include "wv_log.h"
#include "wv_err.h"

/*****************************************************************************
  Function:     socketC_open
  Description:  socket Clinent open
  Input:        none
  Output:       S32 * pu32socketCfd -  文件描述符
  Return:       SOCKETSErrCode
  Author:       ruibin.zhang
*****************************************************************************/
SOCKETCErrCode socketC_open(S32 * pu32socketCfd)
{
    if(NULL == pu32socketCfd)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_COMM, "Param NULL");
        return WV_ERR_PARAMS;
    }

    * pu32socketCfd = socket(AF_INET, SOCK_STREAM , IPPROTO_IP);// IPv4 |
    if(* pu32socketCfd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SOCKET, "socketCfd open error: %s(errno: %d)!",strerror(errno),errno);
        return WV_SOCKETC_OPEN_ERR;
    }

    return WV_SOCKETC_SUCCESS;
}

/*****************************************************************************
  Function:     socketC_connect
  Description:  socket Clinent open
  Input:        S32 u32socketCfd    -       文件描述符
                U8 * pu8IPAddr      -       IP 地址
  Output:
  Return:       SOCKETSErrCode
  Author:       ruibin.zhang
*****************************************************************************/
SOCKETCErrCode socketC_connect(S32 u32socketCfd, U32 u32IPAddr)
{

    U8 U8IPaddr[20];  
    U8 *u8data = (U8*)&u32IPAddr;

    /* genera ipAddr string*/
    sprintf((char *)&U8IPaddr[0],"%d.%d.%d.%d", u8data[3], u8data[2], u8data[1], u8data[0]);

    if(u32socketCfd < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SOCKET, "socketCfd open error!");
        return WV_SOCKETC_OPEN_ERR;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr((char *)U8IPaddr);        //想通信的ip
    servaddr.sin_port = htons(WV_SERVER_DEFAULT_PORT);              //设置的端口为DEFAULT_PORT

    //将本地地址绑定到所创建的套接字上
    if( -1 == connect(u32socketCfd, (struct sockaddr*)&servaddr, sizeof(servaddr)))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SOCKET, "connect server error: %s(errno: %d)",strerror(errno),errno);
        return  WV_SOCKETC_BIND_ERR;
    }

    return WV_SOCKETC_SUCCESS;
}

