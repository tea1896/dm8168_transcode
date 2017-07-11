#include <osa.h>
#include <osa_thr.h>
#include <osa_sem.h>
#include "ti_venc.h"

typedef struct {
    OSA_ThrHndl thr;
    OSA_SemHndl sem;
    Bool quit;
    Bool quit_done;

    FILE *fp[32];
    int fcount;
} BitsSaver; 
BitsSaver saver;

static void *bits_saver_thr_func(void *prm) {
    while (! saver.quit) {
        /* wait enc completed */
        //printf("Waiting encoded data ... \n");
        OSA_semWait(&saver.sem, OSA_TIMEOUT_FOREVER);

        /* get encoded data */
        VCODEC_BITSBUF_LIST_S bitsBuf;
        if (Venc_getBitstreamBuffer(&bitsBuf, TIMEOUT_NO_WAIT) != ERROR_NONE)
            continue;

        /* save */
        int i;
        for(i=0; i < bitsBuf.numBufs; i++) {      
            VCODEC_BITSBUF_S *pBuf = &bitsBuf.bitsBuf[i];
            //printf("Write encoded (%d) bytes in channel (%d) in file (%d) ... \n", pBuf->filledBufSize, pBuf->chnId, fileno(saver.fp[pBuf->chnId]));
            if(fwrite(pBuf->bufVirtAddr, 1, pBuf->filledBufSize, saver.fp[pBuf->chnId]) != pBuf->filledBufSize) 
            {
                saver.quit = TRUE;
            }
            else
            {
                fsync(fileno(saver.fp[pBuf->chnId]));
            }
        }

        Venc_releaseBitstreamBuffer(&bitsBuf);
    }

    saver.quit_done = TRUE;

    return NULL;
}

static void bits_saver_signal_new_data(Ptr prm) {
    //printf("Received encoded data!\n");
    OSA_semSignal(&saver.sem);
}

void bits_saver_create(int channels, char *f_ext) {
    /* File write handle */
    int i;
    saver.fcount = channels;
    for (i = 0; i < saver.fcount; i++) {
        char name[128];
        sprintf(name, "%s/VID_CH%02d.%s", ".", i, f_ext);
        saver.fp[i] = fopen(name, "wb");
        if( NULL == saver.fp[i])
        {
            printf("File (%s) can't open!\n", name);
        }
        else
        {
            printf("File (%s) open => (%d)!\n", name, fileno( saver.fp[i]));
        }
    }

    /* Register call back with encoder */
    VENC_CALLBACK_S callback;
    callback.newDataAvailableCb = bits_saver_signal_new_data;
    Venc_registerCallback(&callback, (Ptr)&saver);

    saver.quit = FALSE;
    saver.quit_done = FALSE;
    OSA_assert(OSA_SOK == OSA_semCreate(&saver.sem, 1, 0));
    OSA_assert(OSA_SOK == OSA_thrCreate(&saver.thr, bits_saver_thr_func, OSA_THR_PRI_DEFAULT, 0, &saver));
}

void bits_saver_delete() {
    saver.quit = TRUE;
    OSA_semSignal(&saver.sem);

    while (! saver.quit_done)
        OSA_waitMsecs(10);

    OSA_thrDelete(&saver.thr);
    OSA_semDelete(&saver.sem);

    int i;
    for (i = 0; i < saver.fcount; i++) 
        fclose(saver.fp[i]);
}
