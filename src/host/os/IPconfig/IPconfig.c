/**********************************************************************
* Copyright(c), 2017 WELLAV Technology Co.,Ltd.
* All rights reserved.
*
* FileName:     IPconfig.c
* Description:  IP manage and configure
* Author    :   ruibin.zhang
* Modified  :
* Reviewer  :
* Date      :   2017-03-04
* Record    :
*
**********************************************************************/

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/msg.h>
#include "appGlobal.h"
#include "IPconfig.h"
#include "xil_io.h"
#include "wv_log.h"

void *s_pGEM1LogicAddress;
void *s_pGEM2LogicAddress;

//定义MDIO接口读写
#define MDIO_BASE 0x11020000  
#define MDIO_DATA 0X0014  
#define MDIO_ADDR 0X0010  
#define MII_BUSY  0x00000001  
#define MII_WRITE 0x00000002  
#define MII_PHY   0x04  

#define READ(_a)     ( *((U32*)(_a)) )  
#define WRITE(_a, _v)   (*(volatile U32 *)(_a) = (_v))  

static int mdio_write(int phyaddr, int phyreg, U16 phydata)  
{  
    U16 value = 0;  
    int count = 10;

    value = (((phyaddr << 11) & (0x0000F800)) |  
            ((phyreg << 6) & (0x000007C0))) |  
            MII_WRITE;  
    value |= MII_BUSY;  

    while(count > 0)
    {
       	if(((READ(s_pGEM1LogicAddress + MDIO_ADDR)) & MII_BUSY) == 1)
		{
			usleep(200000);
		}
		else
		{
			break;
		}
		--count;
    }

    /* Set the MII address register to write */  
    WRITE(s_pGEM1LogicAddress + MDIO_DATA, phydata);
    WRITE(s_pGEM1LogicAddress + MDIO_ADDR, value);
  
    /* Wait until any existing MII operation is complete */  
    count = 10;
    while(count > 0)
    {
		if(((READ(s_pGEM1LogicAddress + MDIO_ADDR)) & MII_BUSY) == 1)
		{
			usleep(200000);
		}
		else
		{
			break;
		}
		--count;
    }

    return 0;  
}  
  
static int mdio_read(int phyaddr, int phyreg)  
{     
    int data = -1;  
    U16 value = 0;  
    int count = 10;
      
    value = (((phyaddr << 11) & (0x0000F800)) |  
            ((phyreg << 6) & (0x000007C0)));  
    value |= MII_BUSY;  

    while(count > 0)
    {
    	if(((READ(s_pGEM1LogicAddress + MDIO_ADDR)) & MII_BUSY) == 1)
    	{
    		usleep(200000);
    	}
    	else
    	{
    		break;
    	}
    	--count;
    }

    /* Set the MII address register to write */  
    WRITE(s_pGEM1LogicAddress + MDIO_ADDR, value);
    count = 10;
    while(count > 0)
    {
    	if(((READ(s_pGEM1LogicAddress + MDIO_ADDR)) & MII_BUSY) == 1)
		{
			usleep(200000);
		}
		else
		{
			break;
		}
		--count;
    }
  
    data = READ(s_pGEM1LogicAddress + MDIO_DATA);
      
    return data;  
}  


/*****************************************************************************
  Function:     Reg_Map
  Description:  map address
  Input:        none
  Output:       none
  Return:       none
  Author:       liwei
*****************************************************************************/
void PHYReg_Map(void)
{
    S32 s32Memfd = 0;

    s32Memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if(s32Memfd == -1)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Can't open /dev/mem.\n");
        return;
    }

    s_pGEM1LogicAddress = mmap(0, 0x1FFF, PROT_READ | PROT_WRITE, MAP_SHARED, s32Memfd, GEM1_NET_CONFIG_BASEADRESS);
    if(s_pGEM1LogicAddress == (void *) -1)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Can't map the memory to user space.\n");
        return;
    }
    else
    {
    	LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "PHY Reg Map Successfully!");
    	return ;
    }

    /*
    s_pGEM2LogicAddress = mmap(0, 0xfff, PROT_READ | PROT_WRITE, MAP_SHARED, s32Memfd, GEM2_NET_CONFIG_BASEADRESS);
    if(s_pGEM2LogicAddress == (void *) -1)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Can't map the memory to user space.\n");
        return;
    }
    */
}

/*****************************************************************************
  Function:     SetEth
  Description:  设置网口
  Input:        type     --- 网口eth0或者eth1
  Output:       none
  Return:       none
  Author:       jie.zhan
*****************************************************************************/
BOOL SetEth(EthType type)
{
    U16 PhyReg0   = 0;
    U16 PhyReg8   = 0;
    U32 Status    = 0;

    U16 PhyReg2   = 0;
    U16 PhyReg3   = 0;
    U32 u32Id     = 0;

    PhyReg2 = mdio_read(0, 2);
    PhyReg3 = mdio_read(0, 3);

    u32Id = PhyReg2;
    u32Id = u32Id << 6;
    u32Id = u32Id | (PhyReg3 >> 10);

    printf("*chip id 0x%x\r\n", u32Id);

    //控制Rgmi数据时钟对其方式,要重启PHY才能生效
    PhyReg0  = mdio_read(0, 20);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "**** read PhyReg20 0x%x\r\n", PhyReg0);

    //PhyReg0 = 0;
    PhyReg0 |= 0xfa;
    mdio_write(0, 20, PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "**** write PhyReg20 0x%x\r\n", PhyReg0);

    //PhyReg0 = 0x0;
    PhyReg0  = mdio_read(0, 20);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "**** read  PhyReg20 0x%x \r\n", PhyReg0);

    //设置PHY为自动协商模式，要重启PHY才能生效
    //PhyReg0 = 0x0;
    PhyReg0 = mdio_read(0, 0);
    PhyReg0 |= 0x1000;
    
    mdio_write(0, 0, PhyReg0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "**** write  PhyReg0 0x%x \r\n", PhyReg0);

    //PhyReg0 = 0;
    PhyReg0 = mdio_read(0, 0);
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "**** read  PhyReg0 0x%x \r\n", PhyReg0);
    
    //PHY0最高位为1，则重启PHY
    //PhyReg0 = 0x0;
    PhyReg0 |= 0x8000;
    mdio_write(0, 0, PhyReg0);

    /*读取当前PHY的速率模式
    bit 15:14
    11 = Reserved
    10 = 1000Mbps
    01 = 100Mbps
    00 = 10Mbps
    */
    //PhyReg0 = 0x0;
    PhyReg0 = mdio_read(0, 17);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "**** read  PhyReg17 0x%x \r\n", PhyReg0);
    
    PhyReg8 = mdio_read(8, 16);
    if((PhyReg0 & 0xC000) == 0x8000)
    {
        //1000M
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "***** 1000M *****\r\n");
        PhyReg8 |= 0x40;
        PhyReg8 &= ~0x2000;
    }
    else if((PhyReg0 & 0xC000) == 0x4000)
    {
        //100M
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "***** 100M *****\r\n");
        PhyReg8 |= 0x2000;
        PhyReg8 &= ~0x40;
    }
    else
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "***** speed mode error *****\r\n");
        return FALSE;
    }

    //把速率模式写到PHY8
    mdio_write(8, 16, PhyReg8);

    PhyReg0  = mdio_read(0, 0);
    //PHY0最高位为1，则重启PHY
    PhyReg0 |= 0x8000;
    mdio_write(0, 0, PhyReg0);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "*****Status = %u *****\r\n", Status );

    PhyReg2 = mdio_read(0, 2);
    PhyReg3 = mdio_read(0, 3);

    u32Id = PhyReg2;
    u32Id = u32Id << 6;
    u32Id = u32Id | (PhyReg3 >> 10);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "***** chip id 0x%x*****\r\n", u32Id );

    return TRUE;
}

/*****************************************************************************
  Function:     PHY_Init
  Description:  PHY芯片初始化
  Input:        none
  Output:       none
  Return:       none
  Author:       jie.zhan
*****************************************************************************/
void PHY_Init(void)
{
    //映射PHY MAP地址
    PHYReg_Map();
    
    //设置网卡eth0和eth1
    SetEth(ETH0);
}

/*****************************************************************************
  Function:     IPconfig_SetIP
  Description:  设置网卡0的IP地址
  Input:        none
  Output:       none
  Return:       none
  Author:       jie.zhan
*****************************************************************************/
void IPconfig_SetIP(U32 u32IP)
{

    U8 u8IPBuf[60] = {0};
    U8 *u8data     = (U8*)&u32IP;
    sprintf((char *)&u8IPBuf[0],"ifconfig eth0 %d.%d.%d.%d netmask 255.255.255.0", u8data[3], u8data[2], u8data[1], u8data[0]);
    
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_SYS, "***** Debug = %s *****\r\n", u8IPBuf);

    //配置eth0
    system("ifconfig eth0 down");
    system((char *)u8IPBuf);
    system("ifconfig eth0 up");
}

//TODO 自动获取IP，还需要进行优化
void IPconfig_GetFreeIP(U32 * pu32Ip)
{
    if(NULL == pu32Ip)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Input Parameter is NULL. \n");
        return ;
    }
    
    //192.168.3.15
    *pu32Ip = 0xc0a8030f;
}

void PHY_TEST()
{
    U16 PhyReg0   = 0;
    U16 PhyReg8   = 0;
    U32 Status    = 0;
    EthType type  = 0;
    
    //设置PHY为自动协商模式，要重启PHY才能生效
    Status  = XEmacPs_PhyRead(type, 0, 20, &PhyReg0);

    printf("[%s:%d] PhyReg0 = 0x%x\r\n", __func__, __LINE__,PhyReg0);
    
    PhyReg0 |= 0xfa;
    Status = XEmacPs_PhyWrite(type, 0, 20, PhyReg0);

    Status  = XEmacPs_PhyRead(type, 0, 20, &PhyReg0);
    printf("[%s:%d] PhyReg0 = 0x%x, status = %u\r\n",__func__, __LINE__, PhyReg0, Status);
    
}
      

  
        

