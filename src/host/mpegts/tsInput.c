#include <stdio.h>
#include <unistd.h>
#include "tsInput.h"

TSIP_INPUT_S g_stTsInput;

TSIP_INPUT_S *  tsInput_GetHandler(void)
{
    return &g_stTsInput;
}

TSIP_INPUT_CHANNEL_S *  tsInput_GetChannelHandler(int32_t channelIndex)
{
    return &(g_stTsInput.inputChannel[channelIndex]);
}

int32_t tsInput_Init(void)
{
    int32_t channelIndex = 0;
    int32_t programIndex = 0;
    int32_t audioIndex = 0;
    int32_t eventFd = OS_INVALID_FD;
    TSIP_INPUT_S * pstTSInputHandler = NULL;

    pstTSInputHandler =  tsInput_GetHandler();      

    for( channelIndex = 0; channelIndex < MAX_SUPPORT_TRANS_NUM; channelIndex++ )
    {
        pstTSInputHandler->inputChannel[channelIndex].channelId = channelIndex;
        memset(pstTSInputHandler->inputChannel[channelIndex].inputURL, 0, OS_MAX_LINE_LEN);
        pstTSInputHandler->inputChannel[channelIndex].programNum = 0;
        pstTSInputHandler->inputChannel[channelIndex].inputReadFlag = 0;
        for( programIndex = 0; programIndex < INPUT_TS_MAX_PROGRAM_NUM; programIndex++ )
        {
            pstTSInputHandler->inputChannel[channelIndex].programBuffer[programIndex].videoStreamBuffer.ifmt_ctx = NULL;
            pstTSInputHandler->inputChannel[channelIndex].programBuffer[programIndex].videoStreamBuffer.numFrames = NULL;
            eventFd = eventfd ( 0, EFD_NONBLOCK | EFD_SEMAPHORE );
            if( OS_INVALID_FD == eventFd )
            {
                printf("Can't create event_fd for input channle (%d) programe (%d)\n", channelIndex, programIndex);
            }
            pstTSInputHandler->inputChannel[channelIndex].programBuffer[programIndex].videoStreamBuffer.inputEventFd = eventFd;

            for( audioIndex = 0; audioIndex < INPUT_TS_MAX_PROGRAM_NUM; audioIndex++ )
            {
                pstTSInputHandler->inputChannel[channelIndex].programBuffer[programIndex].audioStreamBuffer[audioIndex].ifmt_ctx = NULL;
                pstTSInputHandler->inputChannel[channelIndex].programBuffer[programIndex].audioStreamBuffer[audioIndex].numFrames = NULL;
                eventFd = eventfd ( 0, EFD_NONBLOCK | EFD_SEMAPHORE );
                if( OS_INVALID_FD == eventFd )
                {
                    printf("Can't create event_fd for input channle (%d) programe (%d) audio (%d)\n", channelIndex, programIndex, audioIndex);
                }
                pstTSInputHandler->inputChannel[channelIndex].programBuffer[programIndex].audioStreamBuffer[audioIndex].inputEventFd = eventFd;
            }
        }
    }

    return 0;
}

int32_t tsInput_StartChannel(const int32_t inpuChannelNum)
{
    return 0;
}

int32_t tsInput_StopChannel(const int32_t inpuChannelNum)
{
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

