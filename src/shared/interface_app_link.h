/* Include this file after mcfw interface */

#ifndef _INTERFACE_APP_LINK_H_
#define _INTERFACE_APP_LINK_H_

/* Link's identifier in framework */
#define USER_DSP_LINK_ID_APP        (SYSTEM_LINK_ID_ALG_END + 1) /* We improve the Alg link family */

/* App Link's command */
#define USER_DSP_LINK_CMD_APP_GRAY  (0x06)   /* should be greater than 0x05 */

typedef struct 
{
    System_LinkQueInfo      inputInfo;            /* Input queue information */
    UInt32                  numInQue;
    System_LinkInQueParams  inQueParams;
    System_LinkOutQueParams outQueParams;         /* Output queue information */ 
} AppLink_CreateParams;

typedef struct
{
    UInt32  channelNum;
} AppLink_GrayParams;


#endif
