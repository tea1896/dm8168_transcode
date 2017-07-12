#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include "tsInput.h"
#include "wv_log.h"


TSIP_INPUT_S g_stTsInput;

TSIP_INPUT_S *  tsInput_GetHandler(void)
{
    return &g_stTsInput;
}

TSIP_INPUT_CHANNEL_S *  tsInput_GetChannelHandler(const int32_t channelIndex)
{
    return &(g_stTsInput.inputChannel[channelIndex]);
}

int32_t tsInput_Init(void)
{
    int32_t channelIndex = 0;
    int32_t programIndex = 0;
    TSIP_INPUT_S * pstTSInputHandler = NULL;

    pstTSInputHandler =  tsInput_GetHandler();    

    // ffmpeg init
    av_register_all();  
    avformat_network_init(); 

    // input init
    for( channelIndex = 0; channelIndex < MAX_SUPPORT_TRANS_NUM; channelIndex++ )
    {
        pstTSInputHandler->inputChannel[channelIndex].channelId = channelIndex;
        memset(pstTSInputHandler->inputChannel[channelIndex].inputURL, 0, OS_MAX_LINE_LEN);
        pstTSInputHandler->inputChannel[channelIndex].ifmt_ctx = NULL;
        pstTSInputHandler->inputChannel[channelIndex].inputReadFlag = 0;
        pstTSInputHandler->inputChannel[channelIndex].programNum = 0;
        for( programIndex = 0; programIndex < INPUT_TS_MAX_PROGRAM_NUM; programIndex++ )
        {
            pstTSInputHandler->inputChannel[channelIndex].programInfo[programIndex].u32ProgramId = 0;
            memset(pstTSInputHandler->inputChannel[channelIndex].programInfo[programIndex].u8ServiceName, 0, MAX_PROGTRAM_NAME_LEN);
            memset(pstTSInputHandler->inputChannel[channelIndex].programInfo[programIndex].u8ServiceProder, 0, MAX_PROGTRAM_NAME_LEN);
            pstTSInputHandler->inputChannel[channelIndex].programInfo[programIndex].u32StreamNum = 0;
        }
    }

    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "TSIN initialize Finished!");
    return 0;

except_exit:
    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "TSIN  except exit!");
    return -1;
}

int32_t tsInput_StartChannel(const int32_t inpuChannelNum, const int8_t * url)
{
    int32_t  programIndex = 0;
    int32_t  streamIndex  = 0;
    int32_t  frameIndex = 0; 
    int32_t  eventFd = -1;
    AVFormatContext * pFormatCtx = NULL;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);
    pFormatCtx = avformat_alloc_context();  

    /* Open  url */ 
    if(0 != avformat_open_input(&pFormatCtx, url, NULL, NULL))
    {  
        printf("Couldn't open input stream.\n");  
        return -1;  
    }  

    /*  Retrieve stream information  */ 
    if(avformat_find_stream_info(pFormatCtx , NULL) < 0)
    {  
        printf("Couldn't find stream information.\n");  
        return -1;  
    }  

    /* Dump valid information onto standard error  */
    av_dump_format(pFormatCtx, 0, url, false);  

    /* Fill stream info */
    pstChannelInfo->ifmt_ctx = pFormatCtx;
    pstChannelInfo->programNum =  pFormatCtx->nb_programs;
    LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_TSIN, "Channe-(%d) contain (%d) programs!", inpuChannelNum, pstChannelInfo->programNum);

    /* FIXME: Now, only support SPTS (not support MPTS) */
    if( pstChannelInfo->programNum > 0 )
    {
        for( programIndex = 0; programIndex< pstChannelInfo->programNum; programIndex++)    
        {       
            pstChannelInfo->programInfo[programIndex].u32ProgramId = pFormatCtx->programs[programIndex]->program_num;
            pstChannelInfo->programInfo[programIndex].u32StreamNum = pFormatCtx->nb_streams;
            LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_TSIN, "Channe-(%d) program (%d )contain (%d) stream!", inpuChannelNum, programIndex, pFormatCtx->nb_streams);

            for( streamIndex = 0; streamIndex < pFormatCtx->nb_streams; streamIndex++)
            {
                pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].u32StreamId = pFormatCtx->streams[streamIndex]->id;
                pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].u32StreamIndex = pFormatCtx->streams[streamIndex]->index;
                pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].eStreamType = pFormatCtx->streams[streamIndex]->codec->codec_type;
                pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].numFrames = 0;
                pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].inputEventFd = eventfd ( 0, EFD_NONBLOCK | EFD_SEMAPHORE );
                assert( -1 != pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].inputEventFd);
                LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_TSIN, "u32StreamId-(%d) u32StreamIndex (%d ) StreamType (%s) eventfd (%d)!", 
                           pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].u32StreamId, 
                           pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].u32StreamIndex, 
                           (AVMEDIA_TYPE_VIDEO == pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].eStreamType) ? "video" : "no-video",
                           pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].inputEventFd);
                
            }
        }
    }

    /*  */
   

    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "TSIN  channel-%d open  %s finished!", inpuChannelNum, url);

    return 0;
}

int32_t tsInput_ClearProgramBuffer(const int32_t inpuChannelNum, const int32_t programIndex)
{
    int32_t streamIndex = 0, pktIndex = 0;
    AVPacket * pstPkt = NULL;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    for( streamIndex = 0; streamIndex < pstChannelInfo->programInfo[programIndex].u32StreamNum; streamIndex++)
    {
        for( pktIndex = 0; pktIndex < pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].numFrames; pktIndex++)
        {
            pstPkt = pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].frameBuf[pktIndex].pktData;
            av_packet_free_side_data(pstPkt);
            pstPkt = pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].frameBuf[pktIndex].pktData = NULL;
        }
        pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].numFrames = 0;
    }      
}

int32_t tsInput_ClearChannelBuffer(const int32_t inpuChannelNum)
{
    int32_t programIndex = 0;  
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL; 

    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);
    for( programIndex = 0; programIndex < pstChannelInfo->programNum; programIndex++)
    {
        tsInput_ClearProgramBuffer(inpuChannelNum, programIndex);
    }
}



int32_t tsInput_StopChannel(const int32_t inpuChannelNum)
{
    int32_t programIndex = 0, streamIndex = 0;; 
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    pstChannelInfo->inputReadFlag = 0;

    if( NULL!= pstChannelInfo->ifmt_ctx)
    {
        avformat_close_input(&(pstChannelInfo->ifmt_ctx));  
        pstChannelInfo->ifmt_ctx = NULL;
    }

    for( programIndex = 0; programIndex < pstChannelInfo->programNum; programIndex++)
    {
        /* Clear all streams buffer */
        tsInput_ClearProgramBuffer(inpuChannelNum, programIndex);

        /* Close eventfd */
        for( streamIndex = 0; streamIndex < pstChannelInfo->programInfo[programIndex].u32StreamNum; streamIndex++)
        {
            close(pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].inputEventFd);        
        }

        /* Clear all stream */
        pstChannelInfo->programInfo[programIndex].u32StreamNum = 0;
    }

    /* Clear all program */
    pstChannelInfo->programNum = 0;

    return 0;
}

int32_t tsInput_WriteChannelVideoPkt(const int32_t inpuChannelNum, AVPacket *  pstPkt)
{
    return 0;
}

int32_t tsInput_ReadChannelVideoPkt(const int32_t inpuChannelNum, AVPacket *  pstPkt)
{
    return 0;
}

int32_t tsInput_WriteChannelAudioPkt(const int32_t inpuChannelNum,const int32_t audioChannelNum, AVPacket *  pstPkt)
{
    return 0;
}

int32_t tsInput_ReadChannelAudioPkt(const int32_t inpuChannelNum, const int32_t audioChannelNum, AVPacket *  pstPkt)
{
    return 0;
}


int32_t tsInput_GetChannelVideoPktNum(const int32_t inpuChannelNum)
{
    return 0;
}

int32_t tsInput_GetChannelAudioPktNum(const int32_t inpuChannelNum, const int32_t audioChannelNum)
{
    return 0;
}

