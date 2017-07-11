#ifndef WV_TSINPUT_H_
#define WV_TSINPUT_H_

#include "appGlobal.h"
#include "linux/types.h" 


#include "libavformat/avformat.h"
#include "libavutil/opt.h"
#include "libavutil/time.h"
#include "libavutil/mathematics.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/audio_fifo.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"

#define INPUT_TS_MAX_PROGRAM_NUM        128
#define INPUT_TS_MAX_FRAME_BUFFER_NUM   60

/* 输入的一帧音视频数据 */
typedef struct {
    /* 一帧数据 */
    AVPacket *  pktData;
} INPUT_TS_FRAME_S;

/* 一个输入流缓存 */
typedef struct {    
    /* 输入的视频上下文 */
    AVFormatContext *   ifmt_ctx;

    /* 缓存的帧数 */
    u_int32_t           numFrames;
   
    /* 缓存队列 */
    INPUT_TS_FRAME_S    frameBuf[INPUT_TS_MAX_FRAME_BUFFER_NUM];
    
    /* 输入数据通知事件 */
    int32_t             inputEventFd; 
} INPUT_STREAM_FRAME_LIST_S;


/* 一个输入节目缓存 */
typedef struct {
    INPUT_STREAM_FRAME_LIST_S  videoStreamBuffer;
    INPUT_STREAM_FRAME_LIST_S  audioStreamBuffer[MAX_SUPPORT_AUDIO_NUM_PROGRAM];       
} INPUT_TS_PROGRAM_LIST_S;


/* 一个输入通道  */
typedef struct
{
    /* 输入通道ID */
    u_int32_t           channelId;   
    
    /* 输入通道的URL地址 */
    int8_t              inputURL[OS_MAX_LINE_LEN]; 

    /* 是否读取数据 */
    int8_t              inputReadFlag; 

    /* 节目个数 */
    u_int32_t           programNum;
    
    /* 节目buffer */
   	INPUT_TS_PROGRAM_LIST_S programBuffer[INPUT_TS_MAX_PROGRAM_NUM];
}TSIP_INPUT_CHANNEL_S;

/* TS流输入 */
typedef struct
{
    TSIP_INPUT_CHANNEL_S  inputChannel[MAX_SUPPORT_TRANS_NUM];
}TSIP_INPUT_S;


TSIP_INPUT_S *  tsInput_GetHandler(void);
int32_t         tsInput_Init(void);
int32_t         tsInput_StartChannel(const int32_t inpuChannelNum);
int32_t         tsInput_StopChannel(const int32_t inpuChannelNum);
int32_t         tsInput_WriteChannelVideoPkt(const int32_t inpuChannelNum, AVPacket *  pstPkt);
int32_t         tsInput_ReadChannelVideoPkt(const int32_t inpuChannelNum, AVPacket *  pstPkt);
int32_t         tsInput_WriteChannelAudioPkt(const int32_t inpuChannelNum, const int32_t audioChannelNum, AVPacket *  pstPkt);
int32_t         tsInput_ReadChannelAudioPkt(const int32_t inpuChannelNum, const int32_t audioChannelNum, AVPacket *  pstPkt);
int32_t         tsInput_GetChannelVideoPktNum(const int32_t inpuChannelNum);
int32_t         tsInput_GetChannelAudioPktNum(const int32_t inpuChannelNum, const int32_t audioChannelNum);

#endif


