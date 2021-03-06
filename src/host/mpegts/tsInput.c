#include "appGlobal.h"
#include "tsInput.h"
#include "wv_log.h"
#include "thread.h"
#include "event.h"

TSIP_INPUT_S g_stTsInput;

TSIP_INPUT_S *  tsInput_GetHandler(void)
{
    return &g_stTsInput;
}

TSIP_INPUT_CHANNEL_S *  tsInput_GetChannelHandler(const S32 channelIndex)
{
    return &(g_stTsInput.inputChannel[channelIndex]);
}

S32 tsInput_Init(void)
{
    S32 channelIndex = 0;
    S32 programIndex = 0;
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
        pstTSInputHandler->inputChannel[channelIndex].localSave = 0;
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


static S32 tsInput_FindProgramIndex(AVFormatContext * pstIfmt_ctx, S32 s32StreamIndex)
{
    // FIMXE: Now, Only support SPTS (contain one program)
    return 0;
}

inline S32 tsInput_NotifyChannelEvent(const S32 inpuChannelNum)
{
    S32 i = 0;
    S32 j = 0;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    assert( inpuChannelNum < MAX_SUPPORT_TRANS_NUM );

    for( i=0; i<pstChannelInfo->programNum; i++)
    {
        for( j=0; j<pstChannelInfo->programInfo[i].u32StreamNum; j++)  
        {
            if(pstChannelInfo->programInfo[i].stStreams[j].numFrames > 0)
            {
                //LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "channel (%d) program (%d) stream (%d) frame(%d)!", inpuChannelNum, i, j, pstChannelInfo->programInfo[i].stStreams[j].numFrames);
                Event_Write(pstChannelInfo->programInfo[i].stStreams[j].inputEventFd);
            }
        }
    }

    return 0;
}


inline S32 tsInput_NotifyChannelFlushEvent(const S32 inpuChannelNum)
{
    S32 i = 0;
    S32 j = 0;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    assert( inpuChannelNum < MAX_SUPPORT_TRANS_NUM );

    for( i=0; i<pstChannelInfo->programNum; i++)
    {
        for( j=0; j<pstChannelInfo->programInfo[i].u32StreamNum; j++)  
        {
            //if(pstChannelInfo->programInfo[i].stStreams[j].numFrames > 0)
            {
                Event_Write(pstChannelInfo->programInfo[i].stStreams[j].inputEventFd);
            }
        }
    }

    return 0;
}


void * tsInput_ReadPktTask(void * arg)
{
    S32  inpuChannelNum = 0;
    S32  programIndex = 0;
    S32  streamIndex  = 0;
    S32  programStreamIndex = 0;
    S32  frameIndex = 0;
    S32  audioStreamIndex = 0;
    S32  eventFd = -1;
    S32  s32Ret = 0;
    AVFormatContext * pFormatCtx = NULL;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;
    AVDictionaryEntry *m = NULL;
    AVPacket * pkt = NULL;
    enum AVMediaType pktType = AVMEDIA_TYPE_UNKNOWN;
    S8 s8VideoLocalFileName[OS_MAX_FILE_NAME_LEN] = {0};
    int s8VideoFd = 0;

    pstChannelInfo = (TSIP_INPUT_CHANNEL_S *)arg;;
    pFormatCtx = avformat_alloc_context();     

    /* vars */
    inpuChannelNum = pstChannelInfo->channelId;

    /* priority */
    #if 0
    struct sched_param param = {0};
    param.sched_priority = 99;
    pthread_setschedparam( pthread_self(), SCHED_FIFO, &param );
    #endif

    /* local file */
    if( 0x1 == pstChannelInfo->localSave )
    {
        snprintf(s8VideoLocalFileName, OS_MAX_FILE_NAME_LEN, "channel_%d_Program_%d.h264", inpuChannelNum, 0);     
        s8VideoFd = open(s8VideoLocalFileName, O_RDWR | O_CREAT);
        assert(s8VideoFd > 0);
    }

    /* open  url */ 
    if(0 != avformat_open_input(&pFormatCtx, pstChannelInfo->inputURL, NULL, NULL))
    {  
        printf("Couldn't open input stream.\n");  
        return (void *)(-1);  
    }  

    /*  retrieve stream information  */ 
    pFormatCtx->max_analyze_duration = AV_TIME_BASE;
    if(avformat_find_stream_info(pFormatCtx , NULL) < 0)
    {  
        printf("Couldn't find stream information.\n");  
        return (void *)(-2);  
    }  

    /* dump valid information onto standard error  */
    av_dump_format(pFormatCtx, 0, pstChannelInfo->inputURL, false);  

    /* fill stream info */
    pstChannelInfo->ifmt_ctx = pFormatCtx;
    pstChannelInfo->programNum =  pFormatCtx->nb_programs;
    LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_TSIN, "Channe-(%d) contain (%d) programs!", inpuChannelNum, pstChannelInfo->programNum);

    /* FIXME: Now, only support SPTS (not support MPTS) */
    if( pstChannelInfo->programNum > 0 )
    {
        /* fill program information */
        for( programIndex = 0; programIndex< pstChannelInfo->programNum; programIndex++)    
        {       
            /* program id */
            pstChannelInfo->programInfo[programIndex].u32ProgramId = pFormatCtx->programs[programIndex]->program_num;
            
            /* service name  */
            m = av_dict_get(pFormatCtx->programs[programIndex]->metadata,"service_name", m, 0); 
            snprintf( pstChannelInfo->programInfo[programIndex].u8ServiceName, MAX_PROGTRAM_NAME_LEN, "%s",  m->value);
            LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_TSIN, "Channe-(%d) program (%d) service_name(%s)!", inpuChannelNum, programIndex, pstChannelInfo->programInfo[programIndex].u8ServiceName);

            /* provider name  */
            m = av_dict_get(pFormatCtx->programs[programIndex]->metadata,"service_provider", m, 0); 
            snprintf( pstChannelInfo->programInfo[programIndex].u8ServiceProder, MAX_PROGTRAM_NAME_LEN, "%s",  m->value);

            /* stream */
            pstChannelInfo->programInfo[programIndex].u32StreamNum = 0;          
            pstChannelInfo->programInfo[programIndex].u32AudioStreamNum = 0;

            LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_TSIN, "Channe-(%d) program (%d) service_provider(%s)!", inpuChannelNum, programIndex, pstChannelInfo->programInfo[programIndex].u8ServiceProder);
        }

        /* fill stream information */        
        for( streamIndex = 0; streamIndex < pFormatCtx->nb_streams; streamIndex++)
        {
            /* find the program which contain this stream */
            programIndex = tsInput_FindProgramIndex(pFormatCtx, streamIndex);
            programStreamIndex = pstChannelInfo->programInfo[programIndex].u32StreamNum;
            audioStreamIndex = pstChannelInfo->programInfo[programIndex].u32AudioStreamNum;

            /* fill stream information */
            pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].u32ProgramIndex = programIndex;
            pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].u32StreamId = pFormatCtx->streams[streamIndex]->id;
            pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].u32StreamIndex = pFormatCtx->streams[streamIndex]->index;
            pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].eStreamType = pFormatCtx->streams[streamIndex]->codec->codec_type;
            if( AVMEDIA_TYPE_VIDEO == pFormatCtx->streams[streamIndex]->codec->codec_type)
            {
                pstChannelInfo->programInfo[programIndex].u32VideoInputIndex = streamIndex;      
                pstChannelInfo->programInfo[programIndex].u32VideoBufferIndex = programStreamIndex;      
            }
            else if( AVMEDIA_TYPE_AUDIO == pFormatCtx->streams[streamIndex]->codec->codec_type)
            {
                pstChannelInfo->programInfo[programIndex].u32AudioInputIndex[audioStreamIndex] = streamIndex;    
                pstChannelInfo->programInfo[programIndex].u32AudioBufferIndex[audioStreamIndex] = programStreamIndex; 

                /* refresh index */
                pstChannelInfo->programInfo[programIndex].u32AudioStreamNum++;
            }                
            pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].numFrames = 0;
            //pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].inputEventFd = eventfd ( 0, EFD_NONBLOCK | EFD_SEMAPHORE );
            pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].inputEventFd =  Event_CreateFd();
            assert( -1 != pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].inputEventFd);
            assert(0x0 == pthread_mutex_init(&(pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].bufferLock), NULL));    

            pstChannelInfo->programInfo[programIndex].u32StreamNum++;
           
            LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_TSIN, "StreamIndex (%d)  StreamId-(%d) program index (%d) StreamType (%s) eventfd (%d)!", 
                        pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].u32StreamIndex,
                        pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].u32StreamId,                        
                        programIndex,
                        (AVMEDIA_TYPE_VIDEO == pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].eStreamType) ? "video" : "no-video",
                        pstChannelInfo->programInfo[programIndex].stStreams[programStreamIndex].inputEventFd);
        }
    }

    /* debug information */
     LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_TSIN, "channel (%d) program num (%d) audio num (%d) audio input (%d %d %d %d) audio buffer (%d %d %d %d)!",
                pstChannelInfo->channelId, pstChannelInfo->programNum, pstChannelInfo->programInfo[0].u32AudioStreamNum,
                pstChannelInfo->programInfo[0].u32AudioInputIndex[0], pstChannelInfo->programInfo[0].u32AudioInputIndex[1],pstChannelInfo->programInfo[0].u32AudioInputIndex[2],pstChannelInfo->programInfo[0].u32AudioInputIndex[3],
                pstChannelInfo->programInfo[0].u32AudioBufferIndex[0], pstChannelInfo->programInfo[0].u32AudioBufferIndex[1],pstChannelInfo->programInfo[0].u32AudioBufferIndex[2],pstChannelInfo->programInfo[0].u32AudioBufferIndex[3]); 


    /* thread flag */
    pstChannelInfo->exitFlag = 0;
    pstChannelInfo->inputReadFlag = 0x1;

    /* read AV data */
    while(0x1 == pstChannelInfo->inputReadFlag)
    {        
        /* read input frame */
        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "Read channel (%d)!", pstChannelInfo->channelId);
        pkt = av_packet_alloc();
        av_init_packet(pkt);
        
        s32Ret =  av_read_frame(pFormatCtx, pkt);
        if( s32Ret >= 0 )
        {
            pktType =pFormatCtx->streams[pkt->stream_index]->codec->codec_type;  
            switch(pktType)
            {
                case AVMEDIA_TYPE_VIDEO:                   
                    if( pkt->size > 0)
                    {
                        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "Channel (%d) read video (%d) bytes!", inpuChannelNum, pkt->size);
                        tsInput_WriteChannelVideoPkt(inpuChannelNum, pkt);
                        if( 0x1 == pstChannelInfo->localSave )
                        {
                            if( write(s8VideoFd, pkt->data, pkt->size) < pkt->size)
                            {
                                LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "Channel (%d) read video (%d) bytes failed!", inpuChannelNum, pkt->size);
                            }
                        }
                    }
                    else
                    {
                        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "Channel (%d) read video invalid (%d) bytes!", inpuChannelNum, pkt->size);
                        av_packet_free(&pkt);
                    }
                    break;
                case AVMEDIA_TYPE_AUDIO:
                    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "Channel (%d) read audio  (%d) bytes!", inpuChannelNum, pkt->size);                    
                    if( pkt->size > 0)
                    {
                        tsInput_WriteChannelAudioPkt(inpuChannelNum, pkt);
                    }
                    else
                    {
                        av_packet_free(&pkt);
                    }
                                
                    break;
                default:
                    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "Channel (%d) read unknown (%d) bytes!", inpuChannelNum, pkt->size);
                    break;
            }
        }
        else
        {
            /* idle */
            //usleep(50000);
        }

        /* notify next task to receive frame */
        tsInput_NotifyChannelEvent(inpuChannelNum);      
    }


    /* recycle resource */
    if( s8VideoFd > 0)
    {
        close(s8VideoFd);
    }
    
    pstChannelInfo->exitFlag = 1;


    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "Exit channel (%d) !", pstChannelInfo->channelId);

    return NULL;  
}



S32 tsInput_StartChannel(const S32 inpuChannelNum, const int8_t * url)
{
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    /* fill channel information */
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);
    snprintf(pstChannelInfo->inputURL, OS_MAX_LINE_LEN, "%s", url);
    pstChannelInfo->channelId = inpuChannelNum;
    
    /* creat task to read AV data */   
    THREAD_NEW_DETACH(tsInput_ReadPktTask, pstChannelInfo,"Read task");

    /* wait start */
    if( 0 != tsInput_WaitChannelStart(inpuChannelNum, 1000))
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "wait channel  (%d) start timeout !", inpuChannelNum);       
    }
   
    return 0;
}

S32 tsInput_ClearProgramBuffer(const S32 inpuChannelNum, const S32 programIndex)
{
    S32 streamIndex = 0, pktIndex = 0;
    AVPacket * pstPkt = NULL;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    for( streamIndex = 0; streamIndex < pstChannelInfo->programInfo[programIndex].u32StreamNum; streamIndex++)
    {
        /* lock */
        pthread_mutex_lock(&(pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].bufferLock));
    
        for( pktIndex = 0; pktIndex < pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].numFrames; pktIndex++)
        {
            pstPkt = pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].frameBuf[pktIndex].pktData;
            av_packet_free(&pstPkt);
            pstPkt = pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].frameBuf[pktIndex].pktData = NULL;
        }

        /* unlock */
        pthread_mutex_unlock(&(pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].bufferLock));
        
        pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].numFrames = 0;
    }      
}

S32 tsInput_ClearChannelBuffer(const S32 inpuChannelNum)
{
    S32 programIndex = 0;  
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL; 

    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);
    for( programIndex = 0; programIndex < pstChannelInfo->programNum; programIndex++)
    {
        tsInput_ClearProgramBuffer(inpuChannelNum, programIndex);
    }
}




S32 tsInput_StopChannel(const S32 inpuChannelNum)
{
    S32 programIndex = 0, streamIndex = 0;; 
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;
    S32 s32Timeout = 100;

    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    /* exit read task */
    pstChannelInfo->inputReadFlag = 0;
    while( (0x0 == pstChannelInfo->exitFlag) &&  (s32Timeout > 0))
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "Wating channel (%d) url (%s) exit ...!", inpuChannelNum, pstChannelInfo->inputURL);
        usleep(100000);
        s32Timeout--;
    }
    if(s32Timeout <= 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "Wating channel (%d) url (%s) exit timeout...!", inpuChannelNum, pstChannelInfo->inputURL);
        goto except_exit;
    }

    /* free read handler of input */
    if( NULL!= pstChannelInfo->ifmt_ctx)
    {
        avformat_close_input(&(pstChannelInfo->ifmt_ctx));  
        pstChannelInfo->ifmt_ctx = NULL;
    }

    for( programIndex = 0; programIndex < pstChannelInfo->programNum; programIndex++)
    {
        /* clear all streams buffer */
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "clear buffer of channel (%d) program (%d)!", inpuChannelNum, programIndex);
        tsInput_ClearProgramBuffer(inpuChannelNum, programIndex);

        /* close eventfd & destroy buffer lock */
        for( streamIndex = 0; streamIndex < pstChannelInfo->programInfo[programIndex].u32StreamNum; streamIndex++)
        {
            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "destroy  channel (%d) program (%d) stream (%d) lock!", inpuChannelNum, programIndex, streamIndex);
            pthread_mutex_destroy(&(pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].bufferLock));

            LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "close  channel (%d) program (%d) stream (%d) eventfd!", inpuChannelNum, programIndex, streamIndex);
            close(pstChannelInfo->programInfo[programIndex].stStreams[streamIndex].inputEventFd);        
        }

        /* clear all stream */
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "clear buffer of channel (%d)  program (%d) !", inpuChannelNum, programIndex);
        pstChannelInfo->programInfo[programIndex].u32StreamNum = 0;
    }

    /* clear all program */
    pstChannelInfo->programNum = 0;

    return 0;

except_exit:
    return -1;  
}

inline S32 tsInput_WriteChannelVideoPkt(const S32 inpuChannelNum, AVPacket *  pstPkt)
{
    S32 s32Ret = 0;
    S32 programIndex = 0;
    S32 streamIndex = 0;
    S32 videoIndex = 0;
    S32 currentFrameNum = 0;
    S32 timeout = INPUT_TS_MAX_FRAME_BUFFER_NUM;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;
  
    /* verify input parameters */
    if( (NULL == pstPkt) 
     || (inpuChannelNum >= MAX_SUPPORT_TRANS_NUM)) 
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "wrong input parameters!"); 
        s32Ret =  -1;
        goto except_exit;   
    }  

    /* find channel & program index */
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    /* if have start */
    if(  0x1 != pstChannelInfo->inputReadFlag  )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "channel (%d) input is not start (%d)!", inpuChannelNum, pstChannelInfo->inputReadFlag); 
        s32Ret =  -2;
        goto except_exit;   
    }   

    /* find program index */   
    programIndex =  tsInput_FindProgramIndex(pstChannelInfo->ifmt_ctx, pstPkt->stream_index);
    if( programIndex < 0)
    {
        s32Ret =  -3;
        goto except_exit;
    }
    videoIndex = pstChannelInfo->programInfo[programIndex].u32VideoBufferIndex;

    /* if buffer is full, clear buffer */
    if( true == tsInput_VideoBufferIsFull( inpuChannelNum, programIndex))
    {
        /* clear buffer */
        do
        {
            currentFrameNum = pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].numFrames;
            if( currentFrameNum > 0 )
            {
                Event_Write(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].inputEventFd);
                usleep(1000);
                timeout--;
            }        
        }while( ( currentFrameNum > 0 ) && (timeout > 0) );    

        if((timeout <= 0) && ( true == tsInput_VideoBufferIsFull( inpuChannelNum, programIndex)))
        {
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "### ch (%d) pro (%d) v (%d) full: (%d)!", inpuChannelNum, programIndex, videoIndex, currentFrameNum); 
            s32Ret =  -4;
            goto except_exit;
        }
    }
          
    /* get lock */
    pthread_mutex_lock(&(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].bufferLock));  
    currentFrameNum = pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].numFrames; 
    if( currentFrameNum < INPUT_TS_MAX_FRAME_BUFFER_NUM )
    {
        pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].frameBuf[currentFrameNum].pktData = pstPkt;
    }
    pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].numFrames++;

    /* debug information */
    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "write to channel (%d) program (%d) video (%d) pkt size (%d) frame (%d)!", inpuChannelNum, programIndex, videoIndex, pstPkt->size, currentFrameNum + 1);
    
    /* free lock */
    pthread_mutex_unlock(&(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].bufferLock));

    /* notify next task */
    assert(EVENT_SUCCESS == Event_Write(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].inputEventFd));

    return 0;

except_exit:
    return s32Ret;
}

inline S32 tsInput_ReadChannelVideoPkt(const S32 inpuChannelNum, const S32 programIndex, AVPacket ** pstPkt)
{
    S32 s32Ret = 0;
    S32 streamIndex = 0;
    S32 videoIndex = 0;
    S32 currentFrameNum = 0;
    AVPacket * srcPkt = NULL;
    AVPacket * dstPkt = NULL;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    /* fill channel information */
    assert(inpuChannelNum < MAX_SUPPORT_TRANS_NUM);
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);
    if( programIndex < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "wrong input parameters!"); 
        s32Ret =  -1;
        goto except_exit;
    }

    /* verify input parameters */
    if( ( programIndex >= pstChannelInfo->programNum) 
     || ( NULL == pstPkt)) 
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "wrong input parameters!"); 
        s32Ret =  -2;
        goto except_exit;   
    }    

    /* if have started input */
    if(  0x1 != pstChannelInfo->inputReadFlag  )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "channel (%d) input is not start (%d)!", inpuChannelNum, pstChannelInfo->inputReadFlag); 
        s32Ret =  -3;
        goto except_exit;   
    }

    /* find video index in program */
    videoIndex = pstChannelInfo->programInfo[programIndex].u32VideoBufferIndex;

    /* wait data */
    LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "Wait video event (%d)!", pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].inputEventFd);
    s32Ret = Event_Read(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].inputEventFd);
    if( EVENT_SUCCESS != s32Ret)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "channel (%d) program (%d) vidoe stream (%d) wait error: (%d)!", inpuChannelNum, programIndex, videoIndex, s32Ret); 
        s32Ret =  -4;
        goto except_exit;   
    }

    /* if is empty */
    currentFrameNum = pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].numFrames;
    if( currentFrameNum <= 0 )
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) program (%d) vidoe stream (%d) is empty: (%d)!", inpuChannelNum, programIndex, videoIndex, currentFrameNum); 
        s32Ret =  -5;
        goto except_exit;
    }   
        
    /* get lock */
    pthread_mutex_lock(&(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].bufferLock));  

    /* read frame */
    srcPkt = pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].frameBuf[0].pktData;
    dstPkt = av_packet_clone(srcPkt);
    assert( NULL !=  dstPkt);
    *pstPkt = dstPkt;
    av_packet_free(&srcPkt);

    /* modify buffer */
    currentFrameNum = pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].numFrames;
    if( currentFrameNum >= 2)
    {
        memmove(&(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].frameBuf[0]),
               &(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].frameBuf[1]),
               (currentFrameNum  - 1) * (sizeof(INPUT_TS_FRAME_S)));
    }
    pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].numFrames--;   
    
    /* free lock */
    pthread_mutex_unlock(&(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].bufferLock));

    /* debug information */
    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "read  channel (%d) program (%d) video (%d) frame (%d) pkt size (%d) !", inpuChannelNum, programIndex, videoIndex, currentFrameNum, (*pstPkt)->size);
    
    return 0;

except_exit:
    return s32Ret;
}


inline S32 tsInput_WriteChannelAudioPkt(const S32 inpuChannelNum, AVPacket * pstPkt)
{
    S32 i = 0;
    S32 s32Ret = 0;
    S32 programIndex = 0;
    S32 streamIndex = 0;
    S32 audioIndex = 0;
    S32 audioBufferIndex = 0;
    S32 currentFrameNum = 0;
    S32 timeout = INPUT_TS_MAX_FRAME_BUFFER_NUM * 200;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    /* verify input parameters */
    if( (inpuChannelNum >= MAX_SUPPORT_TRANS_NUM) 
     || ( NULL == pstPkt)) 
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "wrong input parameters!"); 
        s32Ret =  -1;
        goto except_exit;   
    }    

    /* find channel & program index */
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    /* find program index in program */
    programIndex =  tsInput_FindProgramIndex(pstChannelInfo->ifmt_ctx, pstPkt->stream_index);
    if( programIndex < 0)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "channel (%d) stream is invalid: (%d)!", inpuChannelNum, pstPkt->stream_index); 
        s32Ret =  -2;
        goto except_exit;
    }

    /* if have start */
    if(  0x1 != pstChannelInfo->inputReadFlag  )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "channel (%d) input is not start (%d)!", inpuChannelNum, pstChannelInfo->inputReadFlag); 
        s32Ret =  -3;
        goto except_exit;   
    }

    /* find audio index */
    for( i = 0; i < pstChannelInfo->programInfo[programIndex].u32AudioStreamNum; i++ )
    {
        if(pstPkt->stream_index == pstChannelInfo->programInfo[programIndex].u32AudioInputIndex[i] )
        {
            audioIndex = i;
            audioBufferIndex = pstChannelInfo->programInfo[programIndex].u32AudioBufferIndex[audioIndex];        
        }
    }

    if( i > pstChannelInfo->programInfo[programIndex].u32AudioStreamNum )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "channel (%d) program (%d) stream is invalid: (%d)!", inpuChannelNum, programIndex, pstPkt->stream_index); 
        s32Ret =  -4;
        goto except_exit;
    }
    
    /* if buffer is full, clear buffer */
    if( true == tsInput_AudioBufferIsFull( inpuChannelNum, programIndex, audioIndex))
    {
        /* clear buffer */
        do
        {
            currentFrameNum = pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].numFrames;
            if( currentFrameNum > 0 )
            {
                Event_Write(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].inputEventFd);
                usleep(1000);
                timeout--;
            }        
        }while( ( currentFrameNum > 0 ) && (timeout > 0) );  

        if((timeout <= 0) && ( true == tsInput_AudioBufferIsFull( inpuChannelNum, programIndex, audioIndex)))
        {
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "### channel (%d) program (%d) audio (%d) is full: (%d)!", inpuChannelNum, programIndex, audioIndex, currentFrameNum); 
            s32Ret =  -5;
            goto except_exit;
        }
    }
        
    /* get lock */
    pthread_mutex_lock(&(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].bufferLock));  
    
    currentFrameNum = pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].numFrames;  
    pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].frameBuf[currentFrameNum].pktData = pstPkt;
    pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].numFrames++;

    /* debug information */
    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "write to channel (%d) program (%d) audio (%d) pkt size (%d) reserve (%d) frames!", inpuChannelNum, programIndex, audioIndex, pstPkt->size, currentFrameNum + 1);
    
    /* free lock */
    pthread_mutex_unlock(&(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].bufferLock));

    /* notify next task */
    assert(EVENT_SUCCESS == Event_Write(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].inputEventFd));
    
    return 0;

except_exit:
    return s32Ret;
}


inline S32 tsInput_ReadChannelAudioPkt(const S32 inpuChannelNum, const S32 programIndex, const S32 audioIndex, AVPacket **  pstPkt)
{
    S32 s32Ret = 0;
    S32 audioBufferIndex = 0;
    S32 currentFrameNum = 0;
    AVPacket * srcPkt = NULL;
    AVPacket * dstPkt = NULL;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    /* fill channel information */
    assert(inpuChannelNum < MAX_SUPPORT_TRANS_NUM);
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    /* if have start */
    if(  0x1 != pstChannelInfo->inputReadFlag  )
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "channel (%d) input is not start (%d)!", inpuChannelNum, pstChannelInfo->inputReadFlag); 
        s32Ret =  -1;
        goto except_exit;   
    }   

    /* verify input parameters */
    if( ( programIndex >= pstChannelInfo->programNum)
     || ( audioIndex >= pstChannelInfo->programInfo[programIndex].u32AudioStreamNum ) 
     || ( NULL == pstPkt)) 
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "wrong input parameters!"); 
        s32Ret =  -2;
        goto except_exit;   
    }    
    audioBufferIndex = pstChannelInfo->programInfo[programIndex].u32AudioBufferIndex[audioIndex];

    /* wait data */
    s32Ret = Event_Read(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].inputEventFd);
    if( EVENT_SUCCESS != s32Ret)
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "channel (%d) program (%d) audio (%d) wait error: (%d)!", inpuChannelNum, programIndex, audioIndex, s32Ret); 
        s32Ret =  -3;
        goto except_exit;   
    }

    /* if is empty */
    currentFrameNum = pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].numFrames;
    if( currentFrameNum <= 0 )
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) program (%d) audio (%d) is empty: (%d)!", inpuChannelNum, programIndex, audioIndex, currentFrameNum); 
        s32Ret =  -4;
        goto except_exit;
    }   
        
    /* get lock */
    pthread_mutex_lock(&(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].bufferLock));  

    /* read frame */
    srcPkt = pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].frameBuf[0].pktData;
    if(NULL == srcPkt)
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) program (%d) audio (%d) have (%d) frames!", inpuChannelNum, programIndex, audioIndex, currentFrameNum); 
    }
    assert( NULL !=  srcPkt);
    dstPkt = av_packet_clone(srcPkt);
    assert( NULL !=  dstPkt);
    *pstPkt = dstPkt;
    av_packet_free(&srcPkt);

    /* modify buffer */
    currentFrameNum = pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].numFrames;
    if( currentFrameNum >= 2)
    {
        memmove(&(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].frameBuf[0]),
               &(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].frameBuf[1]),
               (currentFrameNum  - 1) * (sizeof(INPUT_TS_FRAME_S)));
    }
    
    pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].numFrames--;   
    
    /* free lock */
    pthread_mutex_unlock(&(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].bufferLock));

    /* debug information */
    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "read channel (%d) program (%d) audio (%d) pkt size (%d) reserve (%d) frames!", inpuChannelNum, programIndex, audioIndex, (*pstPkt)->size, currentFrameNum - 1);
    
    return 0;

except_exit:
    return s32Ret;
}



inline S32 tsInput_GetChannelVideoPktNum(const S32 inpuChannelNum, const S32 programIndex )
{
    S32 s32Ret = 0;
    S32 s32FrameNum = 0;
    S32 videoIndex = 0;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    /* fill channel information */
    assert(( inpuChannelNum < MAX_SUPPORT_TRANS_NUM));
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);

    /* verify input parameters */
    if( programIndex >= pstChannelInfo->programNum) 
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "wrong input parameters!"); 
        s32Ret =  -1;
        goto except_exit;   
    }    
       
    /* find frame information */
    videoIndex = pstChannelInfo->programInfo[programIndex].u32VideoBufferIndex;
    pthread_mutex_lock(&(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].bufferLock));  
    s32FrameNum = pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].numFrames;
    pthread_mutex_unlock(&(pstChannelInfo->programInfo[programIndex].stStreams[videoIndex].bufferLock));

    /* debug */
    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) program (%d) video frame (%d)!", inpuChannelNum, programIndex, s32FrameNum);

    return s32FrameNum;
except_exit:
    return s32Ret;
}

inline S32 tsInput_GetChannelAudioPktNum(const S32 inpuChannelNum, const S32 programIndex, const S32 audioIndex)
{
    S32 s32Ret = 0;
    S32 streamIndex = 0;
    S32 audioBufferIndex = 0;
    S32 s32FrameNum = 0;
    AVPacket * srcPkt = NULL;
    AVPacket * dstPkt = NULL;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    /* fill channel information */
    assert(( inpuChannelNum < MAX_SUPPORT_TRANS_NUM));
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);
    
    /* verify input parameters */
    if(( programIndex >= pstChannelInfo->programNum)
     || ( audioIndex >= pstChannelInfo->programInfo[programIndex].u32AudioStreamNum )) 
    {
        LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_TSIN, "wrong input parameters!"); 
        s32Ret =  -1;
        goto except_exit;   
    }    
    
    /* find frame information */
    audioBufferIndex = pstChannelInfo->programInfo[programIndex].u32AudioBufferIndex[audioIndex]; 
    pthread_mutex_lock(&(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].bufferLock));      
    s32FrameNum = pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].numFrames;   
    pthread_mutex_unlock(&(pstChannelInfo->programInfo[programIndex].stStreams[audioBufferIndex].bufferLock));

    /* deubg */
    //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) program (%d) audio (%d) frame (%d)!", inpuChannelNum, programIndex, audioIndex, s32FrameNum);
    
    return s32FrameNum;

except_exit:
    return s32Ret;
}


inline bool tsInput_VideoBufferIsFull(const S32 inpuChannelNum, const S32 programIndex)
{
    bool isFull = false;
    S32 s32CurrentFrameNum = 0;

    s32CurrentFrameNum = tsInput_GetChannelVideoPktNum(inpuChannelNum, programIndex);
    if( s32CurrentFrameNum >=  INPUT_TS_MAX_FRAME_BUFFER_NUM / 2)
    {
        isFull = true;    
        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) program (%d) video (%d) is full!", inpuChannelNum, programIndex, s32CurrentFrameNum);
    }
    else
    {
        isFull = false;
        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) program (%d) video (%d) isn't full!", inpuChannelNum, programIndex, s32CurrentFrameNum);
    }

    return isFull;
}


inline bool tsInput_AudioBufferIsFull(const S32 inpuChannelNum, const S32 programIndex, const S32 audioIndex)
{
    bool isFull = false;
    S32 s32CurrentFrameNum = 0;

    s32CurrentFrameNum = tsInput_GetChannelAudioPktNum(inpuChannelNum, programIndex, audioIndex);
    if( s32CurrentFrameNum >=  INPUT_TS_MAX_FRAME_BUFFER_NUM / 2)
    {
        isFull = true;     
        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) program (%d) audio (%d) (%d) is full!", inpuChannelNum, programIndex, audioIndex, s32CurrentFrameNum);
    }
    else
    {
        isFull = false;
        //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) program (%d) audio (%d) (%d) isn't full!", inpuChannelNum, programIndex, audioIndex, s32CurrentFrameNum);
    }

    return isFull;
}

inline bool tsInput_InputBufferIsFull(const S32 inpuChannelNum, const S32 programIndex)
{
    bool isFull = false;

    if( (true == tsInput_VideoBufferIsFull(inpuChannelNum,programIndex))
    ||  (true == tsInput_AudioBufferIsFull(inpuChannelNum,programIndex, 0)))
    {
        isFull =  true;
    }

    return isFull;
}

S32 tsInput_WaitChannelStart(const S32 inpuChannelNum, const S32 timeOut)
{
    S32 s32Ret = 0;
    S32 s32Timeout = timeOut;
    TSIP_INPUT_CHANNEL_S * pstChannelInfo = NULL;

    /* fill channel information */
    assert(( inpuChannelNum < MAX_SUPPORT_TRANS_NUM));
    pstChannelInfo = tsInput_GetChannelHandler(inpuChannelNum);  

    while((0x1 != pstChannelInfo->inputReadFlag) && (s32Timeout > 0) )
    {
        usleep(1000);
        s32Timeout--;
    }

    if(timeOut <= 0)
    {
        s32Ret = -1;
    }

    return s32Ret;
}

/* for test  */
typedef struct
{
    S32 revExitFlag;
    S32 revRunState;
}TSINPUT_REV;

TSINPUT_REV g_stTSInput;



void * tsInput_RevFrame(void * arg)
{
    S32 channelIndex = 0; 
    S32 programIndex = 0; 
    S32 streamIndex = 0; 
    S32 fd = -1;
    S32 maxFd = 0;
    S32 s32Ret = 0;
    fd_set fds;
    TSINPUT_REV * pstTaskHandler = NULL;
    TSIP_INPUT_S * pstInputHandler = NULL;
    struct timeval tiemout;
    S32 s32Count = 0;
    AVPacket * pstPkt = NULL;
    S32 s32Number = 1;
    
    
    pstTaskHandler = (TSINPUT_REV *)arg;
    pstInputHandler =  tsInput_GetHandler();
    
    pstTaskHandler->revExitFlag = 0;
    pstTaskHandler->revRunState = 1;
    
    while(0x0 == pstTaskHandler->revExitFlag)
    {
        FD_ZERO(&fds);

        for( channelIndex = 0; channelIndex<s32Number; channelIndex++ )
        {
            for( programIndex = 0; programIndex<pstInputHandler->inputChannel[channelIndex].programNum; programIndex++ )
            {
                for( streamIndex = 0; streamIndex<pstInputHandler->inputChannel[channelIndex].programInfo[programIndex].u32StreamNum; streamIndex++ )
                {
                    fd = pstInputHandler->inputChannel[channelIndex].programInfo[programIndex].stStreams[streamIndex].inputEventFd;
                    FD_SET(fd, &fds); 
                    maxFd = fd > maxFd? fd+1 : maxFd;
                }
            }
        }

        tiemout.tv_sec = 2;
        s32Ret = select(maxFd,  &fds,  NULL,  NULL,  &tiemout);
        if( s32Ret > 0 )
        {
            //LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "receive data successfuly!");
            for( channelIndex = 0; channelIndex<s32Number; channelIndex++ )
            {
                for( programIndex = 0; programIndex<pstInputHandler->inputChannel[channelIndex].programNum; programIndex++ )
                {
                    /* video */
                    streamIndex = pstInputHandler->inputChannel[channelIndex].programInfo[programIndex].u32VideoBufferIndex;
                    fd = pstInputHandler->inputChannel[channelIndex].programInfo[programIndex].stStreams[streamIndex].inputEventFd;
                    if(FD_ISSET(fd, &fds))
                    {
                        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel -%d video event fd %d!", channelIndex, fd); 
                        tsInput_ReadChannelVideoPkt(channelIndex, programIndex, &pstPkt);
                        //if(10 == channelIndex)
                        {
                            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel -%d read video!", channelIndex);    
                        }
                        
                    }
                       
                    /* audio */
                    streamIndex = pstInputHandler->inputChannel[channelIndex].programInfo[programIndex].u32AudioBufferIndex[0];
                    fd = pstInputHandler->inputChannel[channelIndex].programInfo[programIndex].stStreams[streamIndex].inputEventFd;
                    if(FD_ISSET(fd, &fds))
                    {
                        tsInput_ReadChannelAudioPkt(channelIndex, programIndex, 0, &pstPkt);

                        //if(10 == channelIndex)
                        {
                            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel -%d read audio!", channelIndex);    
                        }
                    }
                }
            }
        }
        else
        {
            LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "receive data error!");
        }  
        
    }
    

    pstTaskHandler->revExitFlag = 1;
    pstTaskHandler->revRunState = 0;
    
}

S32 tsInput_Test(void)
{  
    S32 channelNum = 1;
    S32 channelIndex = 0; 
    S32 s32Count = 0;
    S32 s32Ret  = 0;

    /* start channel */
    for( channelIndex = 0; channelIndex<channelNum; channelIndex++ )
    {
        tsInput_StartChannel(channelIndex,  (int8_t *)"test.ts");
    }

    /* start rev */
    for( channelIndex = 0; channelIndex<channelNum; channelIndex++ )
    {
        THREAD_NEW_DETACH(tsInput_RevFrame, &(g_stTSInput),"Read  task");
    }

    /* main loop */
    while(s32Count < 10)
    {   
        sleep(1);
        s32Count++;
    }
    LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "main loop exit!", channelIndex);

    /* stop  rev */
    g_stTSInput.revExitFlag = 1;
    while( 0x1 == g_stTSInput.revRunState )
    {
        sleep(1);
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "wait rev exit!", channelIndex);
    }

    /* stop channel */
    for( channelIndex = 0; channelIndex<channelNum; channelIndex++ )
    {
        LOG_PRINTF(LOG_LEVEL_DEBUG, LOG_MODULE_TSIN, "channel (%d) input exit!", channelIndex);
        tsInput_StopChannel(channelIndex);
    }
    
    
	return 0;
}



