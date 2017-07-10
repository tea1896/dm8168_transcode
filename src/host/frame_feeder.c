
#include "rtsp_receiver/buffer.h"
#include "rtsp_receiver/RtspReceiver.h"

#include "ti_vdec.h"
#include <osa.h>
#include <osa_thr.h>
#include <osa_sem.h>

#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>

#define BLK_CNT 20 
#define BLK_LEN 1280 * 720   /* it's can be changed but not be too small */

static void frame_feeder_get_one_frame(VCODEC_BITSBUF_S *buf);
static void *frame_feeder_feed_func(Void * prm);

typedef struct {
    uint32_t      buf_size;
    OSA_ThrHndl   thread;
    OSA_ThrHndl   rtsp_recv_thread;
    OSA_SemHndl   sem;

    UInt8 *memPool;
    BufHndl buffer;

    volatile bool quit;
    volatile bool quit_done;
} FileFeeder;
FileFeeder feeder;

static void frame_feeder_initBuffer() 
{
    UInt32 i;
    int status;

    feeder.memPool = (UInt8 *)malloc(BLK_CNT * BLK_LEN);

    /* place all circle buffer blocks in a continuous memory */
    BufCreate desc;
    desc.numBuf = BLK_CNT;
    for (i = 0; i < desc.numBuf; i++)
        desc.bufVirtAddr[i] = (void *)(feeder.memPool + BLK_LEN * i);

    status = Buf_create(&feeder.buffer, &desc);
    OSA_assert(status == BUF_OK);
}

void frame_feeder_init(int buf_size ) {
    int status;
    feeder.quit      = FALSE;
    feeder.quit_done = FALSE;
    feeder.buf_size  = buf_size;

    status = OSA_semCreate(&feeder.sem, 1, 0);
    OSA_assert(status == OSA_SOK);

    // Create thread that feed data to MCFW chain.
    status = OSA_thrCreate(&feeder.thread, frame_feeder_feed_func, 2, 0, NULL);
    OSA_assert(status == OSA_SOK);

    /* init circle buffer which used to get data from live555. */
    frame_feeder_initBuffer();

    /* create thread which get data from buffer. */
    status = OSA_thrCreate(&feeder.rtsp_recv_thread, RtspReceiver_main,
                           OSA_THR_PRI_DEFAULT, 0, &feeder.buffer);
    OSA_assert(status == OSA_SOK);
}

static void *frame_feeder_feed_func(void *arg) {
    /* wait start */
    OSA_semWait(&feeder.sem, OSA_TIMEOUT_FOREVER);

    while (FALSE == feeder.quit) {
        /* request empty buffer */
        VCODEC_BITSBUF_LIST_S buf_list = { .numBufs = 0 };
        VDEC_BUF_REQUEST_S req = { .chNum = 0, . bufSize = feeder.buf_size };
        Vdec_requestBitstreamBuffer(&req, &buf_list, 0);

        if (buf_list.numBufs) {
            /* fill h264 */
            int i;
            for (i = 0; i < buf_list.numBufs; i++) {
                VCODEC_BITSBUF_S *buf = &buf_list.bitsBuf[i];
                frame_feeder_get_one_frame(buf);
            }

            /* send to decode and display */
            Vdec_putBitstreamBuffer(&buf_list);
        }

        OSA_waitMsecs(8);
    }

    feeder.quit_done = TRUE;

    return NULL;
}

static void frame_feeder_get_one_frame(VCODEC_BITSBUF_S *buf) {
    int status;
    int bufId;
    int frame_size;
    BufInfo *buffer_info;

    status = Buf_getFull(&feeder.buffer, &bufId, BUF_WAIT_FOREVER);
    assert(status == BUF_OK);

    buffer_info = Buf_getBufInfo(&feeder.buffer, bufId);
    frame_size = buffer_info->size;

    memcpy(buf->bufVirtAddr, buffer_info->virtAddr, frame_size);
    buf->filledBufSize = frame_size;

    status = Buf_putEmpty(&feeder.buffer, bufId);
    assert(status == BUF_OK);
}

void frame_feeder_start() {
    OSA_semSignal(&feeder.sem);
}

void frame_feeder_stop() {
    feeder.quit = TRUE;
    while(! feeder.quit_done)
        OSA_waitMsecs(10);
}

void frame_feeder_deinit() {
    OSA_thrDelete(&feeder.thread);
    OSA_semDelete(&feeder.sem);

    OSA_thrDelete(&feeder.rtsp_recv_thread);

    Buf_delete(&feeder.buffer);
    free(feeder.memPool);
}
