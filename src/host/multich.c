#include "mcfw/src_linux/mcfw_api/usecases/multich_common.h"
#include "mcfw/src_linux/mcfw_api/usecases/multich_ipcbits.h"
#include "mcfw/interfaces/link_api/system_tiler.h"
#include "../shared/interface_app_link.h"

#define     MULTICH_NUM_SWMS_MAX_BUFFERS              (7)
#define     MAX_BUFFERING_QUEUE_LEN_PER_CH           (50)
#define     BIT_BUF_LENGTH_LIMIT_FACTOR_HD            (5)

static SystemVideo_Ivahd2ChMap_Tbl systemVid_encDecIvaChMapTbl =
{
    .isPopulated = 1,
    .ivaMap[0] =
    {
        .EncNumCh  = 0,
        .EncChList = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0},

        .DecNumCh  = 16,
        .DecChList = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
    },
};

const UInt32 DSP_LINK_ID_APP                     = USER_DSP_LINK_ID_APP;
const UInt32 DSP_LINK_ID_IPC_FRAMES_IN           = SYSTEM_DSP_LINK_ID_IPC_FRAMES_IN_0;
const UInt32 DSP_LINK_ID_IPC_FRAMES_OUT          = SYSTEM_DSP_LINK_ID_IPC_FRAMES_OUT_0; 
const UInt32 VIDEO_LINK_ID_IPC_FRAME_OUT_TO_DSP  = SYSTEM_VIDEO_LINK_ID_IPC_FRAMES_OUT_0;
const UInt32 VPSS_LINK_ID_IPC_FRAME_IN           = SYSTEM_VPSS_LINK_ID_IPC_FRAMES_IN_0;

void Multich_init() {
    IpcBitsOutLinkHLOS_CreateParams   ipcBitsOutHostPrm;
    IpcBitsInLinkRTOS_CreateParams    ipcBitsInVideoPrm;
    DecLink_CreateParams        decPrm;
    IpcLink_CreateParams        ipcOutVideoPrm;
    IpcLink_CreateParams        ipcInVpssPrm;
    SwMsLink_CreateParams       swMsPrm[VDIS_DEV_MAX];
    DisplayLink_CreateParams    displayPrm[VDIS_DEV_MAX];

    UInt32 i;

    MULTICH_INIT_STRUCT(IpcLink_CreateParams,ipcInVpssPrm);
    MULTICH_INIT_STRUCT(IpcLink_CreateParams,ipcOutVideoPrm);
    MULTICH_INIT_STRUCT(IpcBitsOutLinkHLOS_CreateParams,ipcBitsOutHostPrm);
    MULTICH_INIT_STRUCT(IpcBitsInLinkRTOS_CreateParams,ipcBitsInVideoPrm);
    MULTICH_INIT_STRUCT(DecLink_CreateParams, decPrm);
    for (i = 0; i < VDIS_DEV_MAX;i++) {
        MULTICH_INIT_STRUCT(DisplayLink_CreateParams,displayPrm[i]);
        MULTICH_INIT_STRUCT(SwMsLink_CreateParams ,swMsPrm[i]);
    }

    MultiCh_detectBoard();

    /* reset for display */
    System_linkControl( SYSTEM_LINK_ID_M3VPSS, SYSTEM_M3VPSS_CMD_RESET_VIDEO_DEVICES, NULL, 0, TRUE);

    /* set decode channel map */
    System_linkControl( SYSTEM_LINK_ID_M3VIDEO, SYSTEM_COMMON_CMD_SET_CH2IVAHD_MAP_TBL, &systemVid_encDecIvaChMapTbl, sizeof(SystemVideo_Ivahd2ChMap_Tbl), TRUE);

    SystemTiler_disableAllocator();

    /* link used */
    gVdecModuleContext.ipcBitsOutHLOSId = SYSTEM_HOST_LINK_ID_IPC_BITS_OUT_0;
    gVdecModuleContext.ipcBitsInRTOSId  = SYSTEM_VIDEO_LINK_ID_IPC_BITS_IN_0;
    gVdecModuleContext.decId            = SYSTEM_LINK_ID_VDEC_0;
    gVdisModuleContext.swMsId[0]        = SYSTEM_LINK_ID_SW_MS_MULTI_INST_0;
    gVdisModuleContext.displayId[0]     = SYSTEM_LINK_ID_DISPLAY_0; // ON AND OFF CHIP HDMI


    /* ipc bits out host link */
    ipcBitsOutHostPrm.baseCreateParams.outQueParams[0].nextLink= gVdecModuleContext.ipcBitsInRTOSId;
    ipcBitsOutHostPrm.baseCreateParams.notifyNextLink       = FALSE;
    ipcBitsOutHostPrm.baseCreateParams.notifyPrevLink       = FALSE;
    ipcBitsOutHostPrm.baseCreateParams.noNotifyMode         = TRUE;
    ipcBitsOutHostPrm.baseCreateParams.numOutQue            = 1;
    ipcBitsOutHostPrm.inQueInfo.numCh                       = gVdecModuleContext.vdecConfig.numChn;

    for (i=0; i<ipcBitsOutHostPrm.inQueInfo.numCh; i++)
    {
        ipcBitsOutHostPrm.inQueInfo.chInfo[i].width = gVdecModuleContext.vdecConfig.decChannelParams[i].maxVideoWidth;

        ipcBitsOutHostPrm.inQueInfo.chInfo[i].height = gVdecModuleContext.vdecConfig.decChannelParams[i].maxVideoHeight;

        ipcBitsOutHostPrm.inQueInfo.chInfo[i].scanFormat = SYSTEM_SF_PROGRESSIVE;

        ipcBitsOutHostPrm.inQueInfo.chInfo[i].bufType        = 0; // NOT USED
        ipcBitsOutHostPrm.inQueInfo.chInfo[i].codingformat   = 0; // NOT USED
        ipcBitsOutHostPrm.inQueInfo.chInfo[i].dataFormat     = 0; // NOT USED
        ipcBitsOutHostPrm.inQueInfo.chInfo[i].memType        = 0; // NOT USED
        ipcBitsOutHostPrm.inQueInfo.chInfo[i].startX         = 0; // NOT USED
        ipcBitsOutHostPrm.inQueInfo.chInfo[i].startY         = 0; // NOT USED
        ipcBitsOutHostPrm.inQueInfo.chInfo[i].pitch[0]       = 0; // NOT USED
        ipcBitsOutHostPrm.inQueInfo.chInfo[i].pitch[1]       = 0; // NOT USED
        ipcBitsOutHostPrm.inQueInfo.chInfo[i].pitch[2]       = 0; // NOT USED

        ipcBitsOutHostPrm.maxQueueDepth[i] = MAX_BUFFERING_QUEUE_LEN_PER_CH;
        ipcBitsOutHostPrm.chMaxReqBufSize[i] = (ipcBitsOutHostPrm.inQueInfo.chInfo[i].width * ipcBitsOutHostPrm.inQueInfo.chInfo[i].height); 
        ipcBitsOutHostPrm.totalBitStreamBufferSize [i] = (ipcBitsOutHostPrm.chMaxReqBufSize[i] * BIT_BUF_LENGTH_LIMIT_FACTOR_HD);

    }

    /* ipc bits in video link */
    ipcBitsInVideoPrm.baseCreateParams.inQueParams.prevLinkId    = gVdecModuleContext.ipcBitsOutHLOSId;
    ipcBitsInVideoPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcBitsInVideoPrm.baseCreateParams.outQueParams[0].nextLink  = gVdecModuleContext.decId;
    ipcBitsInVideoPrm.baseCreateParams.noNotifyMode              = TRUE;
    ipcBitsInVideoPrm.baseCreateParams.notifyNextLink            = TRUE;
    ipcBitsInVideoPrm.baseCreateParams.notifyPrevLink            = FALSE;
    ipcBitsInVideoPrm.baseCreateParams.numOutQue                 = 1;

    /* decode link */
    for (i=0; i<ipcBitsOutHostPrm.inQueInfo.numCh; i++) {
        if(gVdecModuleContext.vdecConfig.decChannelParams[i].isCodec == VDEC_CHN_H264)
            decPrm.chCreateParams[i].format                 = IVIDEO_H264HP;
        else if(gVdecModuleContext.vdecConfig.decChannelParams[i].isCodec == VDEC_CHN_MPEG4)
            decPrm.chCreateParams[i].format                 = IVIDEO_MPEG4ASP;
        else if(gVdecModuleContext.vdecConfig.decChannelParams[i].isCodec == VDEC_CHN_MJPEG)
            decPrm.chCreateParams[i].format                 = IVIDEO_MJPEG;
        else if(gVdecModuleContext.vdecConfig.decChannelParams[i].isCodec == VDEC_CHN_MPEG2)
            decPrm.chCreateParams[i].format                 = IVIDEO_MPEG2HP;

        decPrm.chCreateParams[i].numBufPerCh = gVdecModuleContext.vdecConfig.decChannelParams[i].numBufPerCh;
        decPrm.chCreateParams[i].profile                = IH264VDEC_PROFILE_ANY;
        decPrm.chCreateParams[i].displayDelay = gVdecModuleContext.vdecConfig.decChannelParams[i].displayDelay;
        decPrm.chCreateParams[i].dpbBufSizeInFrames = IH264VDEC_DPB_NUMFRAMES_AUTO;
        if (gVdecModuleContext.vdecConfig.decChannelParams[i].fieldPicture) {
            OSA_printf("MULTICH_VDEC_VDIS:INFO ChId[%d] configured for field picture\n",i);
            decPrm.chCreateParams[i].processCallLevel   = VDEC_FIELDLEVELPROCESSCALL;
        }
        else {
            decPrm.chCreateParams[i].processCallLevel   = VDEC_FRAMELEVELPROCESSCALL;
        }
        decPrm.chCreateParams[i].targetMaxWidth  = ipcBitsOutHostPrm.inQueInfo.chInfo[i].width;
        decPrm.chCreateParams[i].targetMaxHeight = ipcBitsOutHostPrm.inQueInfo.chInfo[i].height;
        decPrm.chCreateParams[i].defaultDynamicParams.targetFrameRate = gVdecModuleContext.vdecConfig.decChannelParams[i].dynamicParam.frameRate;
        decPrm.chCreateParams[i].defaultDynamicParams.targetBitRate = gVdecModuleContext.vdecConfig.decChannelParams[i].dynamicParam.targetBitRate;
        decPrm.chCreateParams[i].tilerEnable = FALSE;
        decPrm.chCreateParams[i].enableWaterMarking = gVdecModuleContext.vdecConfig.decChannelParams[i].enableWaterMarking;
    }

    decPrm.inQueParams.prevLinkId       = gVdecModuleContext.ipcBitsInRTOSId;
    decPrm.inQueParams.prevLinkQueId    = 0;
    decPrm.outQueParams.nextLink        = VIDEO_LINK_ID_IPC_FRAME_OUT_TO_DSP;


    IpcFramesOutLinkRTOS_CreateParams ipcFramesOutVideoToDSPPrms;
    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams,ipcFramesOutVideoToDSPPrms);
    ipcFramesOutVideoToDSPPrms.baseCreateParams.noNotifyMode   = FALSE;
    ipcFramesOutVideoToDSPPrms.baseCreateParams.notifyPrevLink = FALSE;
    ipcFramesOutVideoToDSPPrms.baseCreateParams.notifyNextLink = TRUE;
    ipcFramesOutVideoToDSPPrms.baseCreateParams.inQueParams.prevLinkId = gVdecModuleContext.decId;
    ipcFramesOutVideoToDSPPrms.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesOutVideoToDSPPrms.baseCreateParams.numOutQue = 1;
    ipcFramesOutVideoToDSPPrms.baseCreateParams.outQueParams[0].nextLink = DSP_LINK_ID_IPC_FRAMES_IN;

    IpcFramesInLinkRTOS_CreateParams  ipcFramesInDspPrms;
    MULTICH_INIT_STRUCT(IpcFramesInLinkRTOS_CreateParams,ipcFramesInDspPrms);
    ipcFramesInDspPrms.baseCreateParams.noNotifyMode   = FALSE;
    ipcFramesInDspPrms.baseCreateParams.notifyPrevLink = TRUE;
    ipcFramesInDspPrms.baseCreateParams.notifyNextLink = TRUE;
    ipcFramesInDspPrms.baseCreateParams.inQueParams.prevLinkId = VIDEO_LINK_ID_IPC_FRAME_OUT_TO_DSP;
    ipcFramesInDspPrms.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesInDspPrms.baseCreateParams.numOutQue   = 1;
    ipcFramesInDspPrms.baseCreateParams.outQueParams[0].nextLink = DSP_LINK_ID_APP;

    /* DSP App link */
    AppLink_CreateParams              appPrms;
    appPrms.numInQue                  = 1;
    appPrms.inQueParams.prevLinkId    = DSP_LINK_ID_IPC_FRAMES_IN;
    appPrms.inQueParams.prevLinkQueId = 0;
    appPrms.outQueParams.nextLink     = DSP_LINK_ID_IPC_FRAMES_OUT;

    IpcFramesOutLinkRTOS_CreateParams dspOutPrm;
    MULTICH_INIT_STRUCT(IpcFramesOutLinkRTOS_CreateParams, dspOutPrm);
    dspOutPrm.baseCreateParams.noNotifyMode              = FALSE;
    dspOutPrm.baseCreateParams.notifyPrevLink            = FALSE; 
    dspOutPrm.baseCreateParams.notifyNextLink            = TRUE;
    dspOutPrm.baseCreateParams.inQueParams.prevLinkId    = DSP_LINK_ID_APP;
    dspOutPrm.baseCreateParams.inQueParams.prevLinkQueId = 0;
    dspOutPrm.baseCreateParams.numOutQue  = 1;
    dspOutPrm.baseCreateParams.outQueParams[0].nextLink  = VPSS_LINK_ID_IPC_FRAME_IN;

    IpcFramesInLinkRTOS_CreateParams  ipcFramesInVpssPrms;
    MULTICH_INIT_STRUCT(IpcFramesInLinkRTOS_CreateParams,ipcFramesInVpssPrms);
    ipcFramesInVpssPrms.baseCreateParams.noNotifyMode              = FALSE;
    ipcFramesInVpssPrms.baseCreateParams.notifyPrevLink            = TRUE;
    ipcFramesInVpssPrms.baseCreateParams.notifyNextLink            = FALSE;
    ipcFramesInVpssPrms.baseCreateParams.inQueParams.prevLinkId    = DSP_LINK_ID_IPC_FRAMES_OUT;
    ipcFramesInVpssPrms.baseCreateParams.inQueParams.prevLinkQueId = 0;
    ipcFramesInVpssPrms.baseCreateParams.numOutQue                 = 1;
    ipcFramesInVpssPrms.baseCreateParams.outQueParams[0].nextLink  = gVdisModuleContext.swMsId[0];

    /* sw mosaic link */
    swMsPrm[0].numSwMsInst = 1;
    swMsPrm[0].swMsInstId[0]        = SYSTEM_SW_MS_SC_INST_DEIHQ_SC_NO_DEI;
    swMsPrm[0].swMsInstStartWin[0]  = 0;
    swMsPrm[0].swMsInstStartWin[1]  = 10;
    swMsPrm[0].enableProcessTieWithDisplay = TRUE;
    swMsPrm[0].includeVipScInDrvPath = FALSE;

    VDIS_DEV vdDevId = VDIS_DEV_HDMI;

    swMsPrm[0].inQueParams.prevLinkId     = VPSS_LINK_ID_IPC_FRAME_IN;
    swMsPrm[0].inQueParams.prevLinkQueId  = 0;
    swMsPrm[0].outQueParams.nextLink      = gVdisModuleContext.displayId[0];
    swMsPrm[0].numOutBuf                  = MULTICH_NUM_SWMS_MAX_BUFFERS;
    swMsPrm[0].maxInputQueLen             = SYSTEM_SW_MS_INVALID_INPUT_QUE_LEN;
    swMsPrm[0].maxOutRes                  = VSYS_STD_1080P_60;
    swMsPrm[0].initOutRes                 = gVdisModuleContext.vdisConfig.deviceParams[VDIS_DEV_HDMI].resolution;
    swMsPrm[0].lineSkipMode               = FALSE;
    swMsPrm[0].enableLayoutGridDraw       = gVdisModuleContext.vdisConfig.enableLayoutGridDraw;

    MultiCh_swMsGetDefaultLayoutPrm(vdDevId, &swMsPrm[0], FALSE);    /* both from 0-16 chnl */

    /* display link */
    displayPrm[0].inQueParams[0].prevLinkId    = gVdisModuleContext.swMsId[0];
    displayPrm[0].inQueParams[0].prevLinkQueId = 0;
    displayPrm[0].displayRes                   = VSYS_STD_1080P_60;
    displayPrm[0].numInputQueues               = 1;

    /* create link */
    System_linkCreate(gVdecModuleContext.ipcBitsOutHLOSId,&ipcBitsOutHostPrm,sizeof(ipcBitsOutHostPrm));
    System_linkCreate(gVdecModuleContext.ipcBitsInRTOSId,&ipcBitsInVideoPrm,sizeof(ipcBitsInVideoPrm));
    System_linkCreate(gVdecModuleContext.decId, &decPrm, sizeof(decPrm));

    System_linkCreate(VIDEO_LINK_ID_IPC_FRAME_OUT_TO_DSP, &ipcFramesOutVideoToDSPPrms, sizeof(ipcFramesOutVideoToDSPPrms));
    System_linkCreate(DSP_LINK_ID_IPC_FRAMES_IN, &ipcFramesInDspPrms, sizeof(ipcFramesInDspPrms));
    System_linkCreate(DSP_LINK_ID_APP, &appPrms, sizeof(appPrms));
    System_linkCreate(DSP_LINK_ID_IPC_FRAMES_OUT, &dspOutPrm, sizeof(dspOutPrm));

    System_linkCreate(VPSS_LINK_ID_IPC_FRAME_IN, &ipcFramesInVpssPrms, sizeof(ipcFramesInVpssPrms));
    System_linkCreate(gVdisModuleContext.swMsId[0]  , &swMsPrm[0], sizeof(swMsPrm[0]));
    System_linkCreate(gVdisModuleContext.displayId[0], &displayPrm[0], sizeof(displayPrm[0]));
}

void Multich_deInit() {
    Vdec_delete();
    Vdis_delete();

    System_linkDelete(VIDEO_LINK_ID_IPC_FRAME_OUT_TO_DSP);
    System_linkDelete(DSP_LINK_ID_APP);
    System_linkDelete(VPSS_LINK_ID_IPC_FRAME_IN);
    System_linkDelete(DSP_LINK_ID_IPC_FRAMES_IN);
    System_linkDelete(DSP_LINK_ID_IPC_FRAMES_OUT);

    MultiCh_prfLoadCalcEnable(FALSE, TRUE, FALSE);

    SystemTiler_enableAllocator();
}
