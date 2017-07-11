#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <osa.h>
#include <osa_thr.h>
#include <osa_sem.h>
#include "ti_vdec.h"

#define CH_NUM_MAX 16

typedef struct {
    FILE         *fp_header[CH_NUM_MAX];
    int           fd_data[CH_NUM_MAX];
    int           fcount;
    int           ch_num;

    uint32_t      buf_size;
    OSA_ThrHndl   thread;
    OSA_SemHndl   sem;

    volatile bool quit;
    volatile bool quit_done;
} FileFeeder;
FileFeeder feeder;

static void file_feeder_read_h264_by_frame(VCODEC_BITSBUF_S *buf) {
    int ch = buf->chnId;

    /* get frame size */
    fscanf(feeder.fp_header[ch], "%d", &(buf->filledBufSize));

    /* read h264 */
    int ret = read(feeder.fd_data[ch], buf->bufVirtAddr, buf->filledBufSize);

    /* end-of-file, read again from start */
    if( feof(feeder.fp_header[ch]) || ret != buf->filledBufSize) {
        clearerr(feeder.fp_header[ch]);
        
        rewind(feeder.fp_header[ch]);
        lseek(feeder.fd_data[ch], 0, SEEK_SET);
        fscanf(feeder.fp_header[ch],"%d",&(buf->filledBufSize));
        
        read(feeder.fd_data[ch], buf->bufVirtAddr, buf->filledBufSize);
    }
}

static void file_feeder_fill_h264(VCODEC_BITSBUF_LIST_S  *buf_list) {
    Int i;
    for (i = 0; i < buf_list->numBufs; i++) {
        VCODEC_BITSBUF_S *buf = &buf_list->bitsBuf[i];
        file_feeder_read_h264_by_frame(buf);
    }
}

static void *file_feeder_fill_thr(Void * prm) {
    OSA_semWait(&feeder.sem, OSA_TIMEOUT_FOREVER);

    while (FALSE == feeder.quit) {
        Int32 i;
        for (i = 0; i < feeder.ch_num; i++) {
            /* request empty buffer */
            VCODEC_BITSBUF_LIST_S buf_list = { .numBufs = 0 };
            VDEC_BUF_REQUEST_S req = { .chNum = i, . bufSize = feeder.buf_size };
            Vdec_requestBitstreamBuffer(&req, &buf_list, 0);

            /* fill and send to decode */
            if (buf_list.numBufs) {
                file_feeder_fill_h264(&buf_list);
                Vdec_putBitstreamBuffer(&buf_list);
            }
        }

        OSA_waitMsecs(8);
    }

    feeder.quit_done = TRUE;

    return NULL;
}


void file_feeder_create(char *path_data, char *path_header, uint32_t buf_size, uint32_t ch_num) {
    feeder.quit      = FALSE;
    feeder.quit_done = FALSE;
    feeder.buf_size  = buf_size;
    feeder.ch_num = ch_num;
    feeder.fcount = feeder.ch_num;

    int i;
    for (i = 0; i < feeder.fcount; i++) {
        feeder.fp_header[i] = fopen(path_header, "r");
        feeder.fd_data[i] = open(path_data, O_RDONLY);
    }

    OSA_semCreate(&feeder.sem, 1, 0);
    OSA_thrCreate(&feeder.thread, file_feeder_fill_thr, 2, 0, NULL);
}

void file_feeder_start() {
    OSA_semSignal(&feeder.sem);
}

void file_feeder_stop() {
    feeder.quit = TRUE;
    while(! feeder.quit_done)
        OSA_waitMsecs(10);
}

void file_feeder_delete() {
    OSA_thrDelete(&feeder.thread);
    OSA_semDelete(&feeder.sem);

    int i;
    for (i = 0; i < feeder.fcount; i++) {
        fclose(feeder.fp_header[i]);
        close(feeder.fd_data[i]);
    }
}
