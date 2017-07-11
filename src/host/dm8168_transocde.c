#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>

#include "ti_vsys.h"
#include "ti_vdec.h"
#include "ti_vdis.h"
#include "ti_vcap.h"
#include "ti_venc.h"


#define CH_NUM  1
#define I_TO_P_FRAME_INTERVAL_FOR_PRIMARY_STREAMS 3 


Void Logo_init();
Void Logo_deInit();
Void Logo_add(int ch_num);

void bits_saver_create(int channels, char *f_ext);
void bits_saver_delete();

void chain_dec_dis_create(int ch_num);
void chain_dec_dis_delete();
void chain_dec_dis_1080p_3x3_layout(VDIS_MOSAIC_S * prm_mosaic);

void file_feeder_create(char *path_data, char *path_header, uint32_t buf_size, uint32_t ch_num);
void file_feeder_delete();
void file_feeder_start();
void file_feeder_stop();

/* signal handle for ctrl + c */
volatile bool g_quit = false;
static void sig_handle(int signo) {
    printf("recived quit signal\n");
    g_quit = true;
}

int dm8168_transcode(int argc, char **argv) {
    /* cmd line */
    if (argc != 6) {
        printf("usage: ./app <<h264_data_file>> <<h264_header_file>> <<width>> <<height>> <<ch_num>>\n");
        return -1;
    }
    char *path_data = argv[1];
    char *path_header = argv[2];
    const int width  = atoi(argv[3]);
    const int height = atoi(argv[4]);
    const int ch_num = atoi(argv[5]); 

    /* register signal hander */
    signal(SIGINT, sig_handle);

    /* init system */
    VSYS_PARAMS_S vsysParams;
    Vsys_params_init(&vsysParams);
    Vsys_init(&vsysParams);

    /* init decode */
    VDEC_PARAMS_S vdecParams;
    Vdec_params_init(&vdecParams);
    vdecParams.numChn                   = ch_num;
    vdecParams.forceUseDecChannelParams = TRUE;
    int i;
    for (i = 0; i < vdecParams.numChn; i++) {
        vdecParams.decChannelParams[i].dynamicParam.frameRate     = 30; 
        vdecParams.decChannelParams[i].dynamicParam.targetBitRate = 2 * 1000 * 1000; 
        vdecParams.decChannelParams[i].maxVideoWidth              = width;
        vdecParams.decChannelParams[i].maxVideoHeight             = height;
        vdecParams.decChannelParams[i].isCodec                    = VDEC_CHN_H264;
        vdecParams.decChannelParams[i].displayDelay               = 10;
        vdecParams.decChannelParams[i].numBufPerCh                = 16; 
        vdecParams.decChannelParams[i].tilerEnable                = FALSE; 
        vdecParams.decChannelParams[i].fieldPicture               = FALSE;
    }
    Vdec_init(&vdecParams);

    
    /* Encode init */
    VENC_PARAMS_S vencParams;
    Venc_params_init(&vencParams);
    /* Enable B frame for primary channels - set parameters before venc_init */
    int ch;
    for (ch=0; ch < ch_num; ch++) 
        vencParams.encChannelParams[ch].dynamicParam.interFrameInterval = I_TO_P_FRAME_INTERVAL_FOR_PRIMARY_STREAMS;
    Venc_init(&vencParams);

    /* init display */
    VDIS_PARAMS_S vdisParams;
    Vdis_params_init(&vdisParams);
    vdisParams.deviceParams[VDIS_DEV_HDMI].resolution = VSYS_STD_1080P_60;
    vdisParams.mosaicLayoutResolution[VDIS_DEV_HDMI]  = vdisParams.deviceParams[VDIS_DEV_HDMI].resolution;
    vdisParams.mosaicParams[VDIS_DEV_HDMI].userSetDefaultSWMLayout = TRUE;
    vdisParams.enableConfigExtVideoEncoder = FALSE;
    chain_dec_dis_1080p_3x3_layout(&vdisParams.mosaicParams[VDIS_DEV_HDMI]); 
    Vdis_init(&vdisParams);
    Vsys_configureDisplay();
    

    /* Init the application specific module which will handle bitstream exchange */
    bits_saver_create(ch_num, "h264");

    /* init log buffer */
    Logo_init();
    
    /* create link instances and connects compoent blocks */
    chain_dec_dis_create(ch_num);

    /* start components in reverse order */
    Vdis_start();
    Venc_start();
    Vcap_start();
    Vdec_start();

    /* load logo file */
    Logo_add(ch_num);

    /* h264 file feeder */
    file_feeder_create(path_data, path_header, width * height, ch_num);
    file_feeder_start();
   
    /* wait ctrl + c */
    while (! g_quit) { usleep(100); }
        printf("### Exit transcode!\n");

    /* stop components */
    Logo_deInit();
    file_feeder_stop();
    usleep(500000);
    Vdec_stop();
    usleep(100000);
    Vcap_stop();
    usleep(100000);
    Vdis_stop();
    usleep(100000);
    Venc_stop();
    usleep(100000);
    
    

    /* de-initialize components */
    chain_dec_dis_delete();
    Vsys_deConfigureDisplay();
    Vdec_exit();
    Venc_exit();
    Vdis_exit();
    Vsys_exit();
    file_feeder_delete();

    return 0;
}
