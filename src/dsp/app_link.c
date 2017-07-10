#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/src_bios6/links_common/system/system_priv_common.h>

#include "../shared/interface_app_link.h"

/* Task Stack */
#define APP_LINK_TSK_STACK_SIZE          (SYSTEM_DEFAULT_TSK_STACK_SIZE)
#pragma DATA_ALIGN(gAppLink_tskStack, 32)
#pragma DATA_SECTION(gAppLink_tskStack, ".bss:taskStackSection")
UInt8 gAppLink_tskStack[APP_LINK_TSK_STACK_SIZE];

/* Link object */
typedef struct
{
    UInt32                  tskId;
    Utils_TskHndl           tsk;
    AppLink_CreateParams    createArgs;

    AppLink_GrayParams      grayPrms;
    System_LinkInfo         info;

    Utils_QueHandle outFrameBufQue;
    FVID2_Frame *outFrameBufQueMem[SYSTEM_IPC_FRAMES_MAX_LIST_ELEM];
} AppLink_Obj;
AppLink_Obj gAppLink_obj;

/* Construct link object */
static Int32 AppLink_drvCreate(AppLink_Obj * pObj, AppLink_CreateParams * pPrm)
{
    UInt32 status;
    System_LinkInQueParams *pInQueParams;

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));

    /* Get link info */
    pInQueParams = &pObj->createArgs.inQueParams;

    status = System_linkGetInfo(pInQueParams->prevLinkId, &pObj->info);
    UTILS_assert(status == FVID2_SOK);

    status = Utils_queCreate(&pObj->outFrameBufQue,
                             SYSTEM_IPC_FRAMES_MAX_LIST_ELEM,
                             pObj->outFrameBufQueMem,
                             UTILS_QUE_FLAG_NO_BLOCK_QUE);
    UTILS_assert(status == FVID2_SOK);

    Vps_printf(" %d: App   : Create Done !!!\n", Utils_getCurTimeInMsec());

    return FVID2_SOK;
}

/* Process frames from previous link */
static Int32 AppLink_drvProcessFrames(AppLink_Obj * pObj)
{
    System_LinkInQueParams *pInQueParams;
    FVID2_FrameList frameList;

    /* Acquire frames */
    pInQueParams = &pObj->createArgs.inQueParams;
    System_getLinksFullFrames(pInQueParams->prevLinkId, pInQueParams->prevLinkQueId, &frameList);

    static int j = 0;
    if (frameList.numFrames)
    {
        UInt32 i;
        for (i = 0; i < frameList.numFrames; i++)
        {
            UInt8 *uv;
            UInt32 uv_size;
            UInt32 status;
            FVID2_Frame *pFrame;
            System_LinkChInfo *chInfo;

            pFrame = frameList.frames[i];

            chInfo = &pObj->info.queInfo[0].chInfo[pFrame->channelNum];
            UTILS_assert(chInfo->dataFormat == SYSTEM_DF_YUV420SP_UV);

            Vps_printf(" %d: App %d : width=%d, height=%d, pitch[0]=%d, pitch[1]=%d !!!\n",
                    Utils_getCurTimeInMsec(), j++, chInfo->width, chInfo->height, chInfo->pitch[0], chInfo->pitch[1]);

            /* Do gray */
            uv = (UInt8 *)pFrame->addr[0][1];
            uv_size = chInfo->pitch[0] * chInfo->height >> 1;
            memset(uv, 0x80, uv_size);
            Cache_wb(uv, uv_size, Cache_Type_ALLD, FALSE);

            status = Utils_quePut(&pObj->outFrameBufQue, pFrame, BIOS_NO_WAIT);
            UTILS_assert(status == FVID2_SOK);
        }

        System_sendLinkCmd(pObj->createArgs.outQueParams.nextLink, SYSTEM_CMD_NEW_DATA);
    }

    return FVID2_SOK;
}

/* Link message loop */
static Void AppLink_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{
    /* Wait CREATE command */
    UInt32 cmd = Utils_msgGetCmd(pMsg);
    if (cmd != SYSTEM_CMD_CREATE)
    {
        Utils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* Create App link object */
    AppLink_Obj *pObj = (AppLink_Obj *) pTsk->appData;
    Int32 status = AppLink_drvCreate(pObj, Utils_msgGetPrm(pMsg));
    Utils_tskAckOrFreeMsg(pMsg, status);
    if (status != FVID2_SOK)
        return;

    Bool done = FALSE;
    Bool ackMsg = FALSE;

    /* Message loop */
    while (!done)
    {
        /* Wait message */
        status = Utils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = Utils_msgGetCmd(pMsg);
        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            case SYSTEM_CMD_NEW_DATA:
                Utils_tskAckOrFreeMsg(pMsg, status);
                AppLink_drvProcessFrames(pObj);
                break;

            /* Dynamic gray params */
            case USER_DSP_LINK_CMD_APP_GRAY:
                memcpy(&pObj->grayPrms, Utils_msgGetPrm(pMsg), sizeof(AppLink_GrayParams));
                Vps_printf(" %d: App   : do gray on channel %d !!!\n",
                    Utils_getCurTimeInMsec(), pObj->grayPrms.channelNum);
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;

            default:
                Utils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    Vps_printf(" %d: App   : Delete Done !!!\n", Utils_getCurTimeInMsec());

    if (ackMsg && pMsg != NULL)
        Utils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Int32 AppLink_getInfo(Utils_TskHndl * pTsk, System_LinkInfo * info)
{
    AppLink_Obj *pObj = (AppLink_Obj *) pTsk->appData;

    memcpy(info, &pObj->info, sizeof(*info));

    return FVID2_SOK;
}

Int32 AppLink_getFullFrames(Utils_TskHndl * pTsk, UInt16 queId,
                                FVID2_FrameList * pFrameList)
{
    AppLink_Obj *pObj = (AppLink_Obj *) pTsk->appData;
    UInt32 idx;
    Int32 status;

    for (idx = 0; idx < FVID2_MAX_FVID_FRAME_PTR; idx++)
    {
        status =
        Utils_queGet(&pObj->outFrameBufQue, (Ptr *) & pFrameList->frames[idx],
                1, BIOS_NO_WAIT);
        if (status != FVID2_SOK)
        break;
    }

    pFrameList->numFrames = idx;

    return FVID2_SOK;
}

Int32 AppLink_putEmptyFrames(Utils_TskHndl * pTsk, UInt16 queId,
                                 FVID2_FrameList * pFrameList)
{
    AppLink_Obj *pObj = (AppLink_Obj *) pTsk->appData;
    (void)pObj;
    System_LinkInQueParams *pInQueParams;

    if (pFrameList->numFrames)
    {
        pInQueParams = &pObj->createArgs.inQueParams;
        System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                   pInQueParams->prevLinkQueId,
                                   pFrameList);
    }

    return FVID2_SOK;
}

/* Init and register link to framework */
Int32 AppLink_init()
{
    AppLink_Obj *pObj = &gAppLink_obj;
    memset(pObj, 0, sizeof(*pObj));

    pObj->tskId = USER_DSP_LINK_ID_APP;          /* Link's identifier in framework */

    System_LinkObj linkObj;
    memset(&linkObj, 0, sizeof(linkObj));
    linkObj.pTsk = &pObj->tsk;
    linkObj.linkGetFullFrames = AppLink_getFullFrames;
    linkObj.linkPutEmptyFrames = AppLink_putEmptyFrames;
    linkObj.getLinkInfo = AppLink_getInfo;

    /* Register App Link to framework */
    System_registerLink(pObj->tskId, &linkObj);

    /* Create working task */
    Int32 status = Utils_tskCreate(&pObj->tsk,
                                   AppLink_tskMain,
                                   2, /* Task Priority */
                                   gAppLink_tskStack,
                                   APP_LINK_TSK_STACK_SIZE, pObj, "App0");
    UTILS_assert(status == FVID2_SOK);

    return status;
}

/* DeInit link */
Int32 AppLink_deInit()
{
    AppLink_Obj *pObj = &gAppLink_obj;
    Utils_queDelete(&pObj->outFrameBufQue);

    Utils_tskDelete(&gAppLink_obj.tsk);
    return FVID2_SOK;
}
