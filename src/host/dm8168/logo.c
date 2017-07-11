#include "mcfw/src_linux/mcfw_api/usecases/multich_common.h"
#include "osa_file.h"

#include "demo_text.h"
#include <time.h>
#include <sys/time.h>

#include "font/unicode_cn_420sp_420_30.c"
#include "font/unicode_cn_422i_420_30.c"
#include "font/ascii_420sp_1078_28.c"
#include "font/ascii_422i_1078_28.c"

#define OSD_NUM_CH          (4)
#define OSD_NUM_WINDOWS     (3)

#define OSD_BUF_HEAP_SR_ID  (0)

#define OSD_TRANSPARENCY    (1)
#define OSD_GLOBAL_ALPHA    (0x80)
#define OSD_ENABLE_WIN      (1)

#define OSD_BUF_ALIGN       (2)

#if 0
#define OSD_LOGO_FILE       "./logo_489x200.422I"
#define OSD_WIN_WIDTH       (490)
#define OSD_WIN_HEIGHT      (200)
#define OSD_WIN0_STARTX     (50)
#define OSD_WIN0_STARTY     (100)
#else
#define OSD_LOGO_FILE       "./rgb16_48x48.raw"
#define OSD_WIN_WIDTH       (48)
#define OSD_WIN_HEIGHT      (48)
#define OSD_WIN0_STARTX     (100)
#define OSD_WIN0_STARTY     (100)
#endif

#define OSD_WIN_PITCH       (OSD_WIN_WIDTH)
#define OSD_BUF_SIZE        (OSD_WIN_PITCH * OSD_WIN_HEIGHT * 2)





Vsys_AllocBufInfo bufInfo;
Vsys_AllocBufInfo osdTextBufInfo;
Vsys_AllocBufInfo osdTimeBufInfo;

void updateTime(int signo);

/*
 * All channels share the same OSD window buffers, this is just for demo,
 * actually each CH can have different OSD buffers
 */
Void Logo_init(UInt32 numCh)
{
    Int status;
    UInt32 actualReadSize = 0;

    status = Vsys_allocBuf(OSD_BUF_HEAP_SR_ID, OSD_BUF_SIZE, OSD_BUF_ALIGN, &bufInfo);
    OSA_assert(ERROR_NONE == status);

    status = Vsys_allocBuf(OSD_BUF_HEAP_SR_ID, OSD_BUF_SIZE, OSD_BUF_ALIGN, &osdTextBufInfo);
    OSA_assert(ERROR_NONE == status);

    status = Vsys_allocBuf(OSD_BUF_HEAP_SR_ID, OSD_BUF_SIZE, OSD_BUF_ALIGN, &osdTimeBufInfo);
    OSA_assert(ERROR_NONE == status);

    OSA_fileReadFile(OSD_LOGO_FILE, bufInfo.virtAddr, OSD_BUF_SIZE, &actualReadSize);
    OSA_assert(actualReadSize == OSD_BUF_SIZE);
}

Void Logo_add(int ch_num)
{
    Int chId;
    struct itimerval oneSec;
    AlgLink_OsdChWinParams osdChParam[ch_num];
    UInt32 padingColor[3] = { 0x0, 0x80, 0x80 };

    for(chId = 0; chId < ch_num; chId++)
    {
        AlgLink_OsdChWinParams * chWinPrm = &osdChParam[chId];

        chWinPrm->chId = chId;
        chWinPrm->numWindows = OSD_NUM_WINDOWS;

        chWinPrm->colorKey[0] = 0xfa;         /* Y */
        chWinPrm->colorKey[1] = 0x7d;         /* U */
        chWinPrm->colorKey[2] = 0x7e;         /* V */

        chWinPrm->winPrm[0].startX             = OSD_WIN0_STARTX;
        chWinPrm->winPrm[0].startY             = OSD_WIN0_STARTY;
        chWinPrm->winPrm[0].width              = OSD_WIN_WIDTH;
        chWinPrm->winPrm[0].height             = OSD_WIN_HEIGHT;
        chWinPrm->winPrm[0].lineOffset         = OSD_WIN_PITCH;
        chWinPrm->winPrm[0].globalAlpha        = OSD_GLOBAL_ALPHA;
        chWinPrm->winPrm[0].transperencyEnable = OSD_TRANSPARENCY;
        chWinPrm->winPrm[0].enableWin          = OSD_ENABLE_WIN;
        chWinPrm->winPrm[0].format             = SYSTEM_DF_YUV422I_YUYV;
        //chWinPrm->winPrm[0].format             =  SYSTEM_DF_RGB16_565;
        chWinPrm->winPrm[0].addr[0][0]         = bufInfo.physAddr;

        // Set window parameters of osd text.
        chWinPrm->winPrm[1].startX             = OSD_WIN0_STARTX;
        chWinPrm->winPrm[1].startY             = OSD_WIN0_STARTY + 100;
        chWinPrm->winPrm[1].width              = 100;
        chWinPrm->winPrm[1].height             = 100;
        chWinPrm->winPrm[1].lineOffset         = OSD_WIN_PITCH;
        chWinPrm->winPrm[1].globalAlpha        = OSD_GLOBAL_ALPHA;
        chWinPrm->winPrm[1].transperencyEnable = OSD_TRANSPARENCY;
        chWinPrm->winPrm[1].enableWin          = OSD_ENABLE_WIN;
        chWinPrm->winPrm[1].format             = SYSTEM_DF_YUV422I_YUYV;
        chWinPrm->winPrm[1].addr[0][0]         = osdTextBufInfo.physAddr;

        char cnExample[20] = "China TV";
        unsigned int length = strlen(cnExample);

        Demo_text_draw(ascii_422i_1078_28, &chWinPrm->winPrm[1], cnExample,
                length, (char*)osdTextBufInfo.virtAddr, padingColor);

        // Set window parameters of osd time.
        chWinPrm->winPrm[2].startX             = OSD_WIN0_STARTX;
        chWinPrm->winPrm[2].startY             = OSD_WIN0_STARTY + 200;
        chWinPrm->winPrm[2].width              = 100;
        chWinPrm->winPrm[2].height             = 100;
        chWinPrm->winPrm[2].lineOffset         = OSD_WIN_PITCH;
        chWinPrm->winPrm[2].globalAlpha        = OSD_GLOBAL_ALPHA;
        chWinPrm->winPrm[2].transperencyEnable = OSD_TRANSPARENCY;
        chWinPrm->winPrm[2].enableWin          = OSD_ENABLE_WIN;
        chWinPrm->winPrm[2].format             = SYSTEM_DF_YUV422I_YUYV;
        chWinPrm->winPrm[2].addr[0][0]         = osdTimeBufInfo.physAddr;
        Demo_text_draw(ascii_422i_1078_28, &chWinPrm->winPrm[2],
                "00:00:00", 8, (char*)osdTimeBufInfo.virtAddr, padingColor);

        signal(SIGALRM, updateTime);
        oneSec.it_value.tv_sec = 1;
        oneSec.it_value.tv_usec = 0;
        oneSec.it_interval.tv_sec = 1;
        oneSec.it_interval.tv_usec = 0;
        setitimer(ITIMER_REAL, &oneSec, NULL);

        System_linkControl(SYSTEM_DSP_LINK_ID_ALG_0,
                           ALG_LINK_OSD_CMD_SET_CHANNEL_WIN_PRM,
                           chWinPrm,
                           sizeof(AlgLink_OsdChWinParams),
                           TRUE);
    }
}

Void Logo_deInit()
{
    Int status;

    struct itimerval stop;
    signal(SIGALRM, SIG_DFL);
    stop.it_value.tv_sec = 0;
    stop.it_value.tv_usec = 0;
    stop.it_interval.tv_sec = 0;
    stop.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &stop, NULL);

    status = Vsys_freeBuf(OSD_BUF_HEAP_SR_ID, bufInfo.virtAddr, OSD_BUF_SIZE);
    OSA_assert(status ==0);
    status = Vsys_freeBuf(OSD_BUF_HEAP_SR_ID, osdTextBufInfo.virtAddr, OSD_BUF_SIZE);
    OSA_assert(status ==0);
    status = Vsys_freeBuf(OSD_BUF_HEAP_SR_ID, osdTimeBufInfo.virtAddr, OSD_BUF_SIZE);
    OSA_assert(status ==0);
}

void updateTime(int signo)
{
    static struct tm tmCur;
    time_t timeval ;
    char osdStr[40];
    //the windows attrib is already set in the init function
    //so here we don't really change window attrib.
    AlgLink_OsdWindowPrm dummy;
    UInt32 padingColor[3]={0x0,0x80,0x80};

    switch(signo){
        case SIGALRM:
            time(&timeval) ;

            localtime_r(&timeval, &tmCur) ;

            sprintf(osdStr,"%02d:%02d:%02d",(int)tmCur.tm_hour,(int)tmCur.tm_min,(int)tmCur.tm_sec);
            Demo_text_draw(ascii_422i_1078_28, &dummy, osdStr, strlen(osdStr),
                    (char*)osdTimeBufInfo.virtAddr, padingColor);
            break;
    }
}
