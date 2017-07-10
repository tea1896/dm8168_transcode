
#include "buffer.h"

#include "FrameSink.h"
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <assert.h>
#include <fcntl.h>

// Even though we're not going to be doing anything with the incoming data, we still need to receive it.
// Define the size of the buffer that we'll use:
#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 100000

extern BufHndl *gRingBuffer;

FrameSink* FrameSink::createNew(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId) {
    return new FrameSink(env, subsession, streamId);
}

FrameSink::FrameSink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId)
    : MediaSink(env),
    fSubsession(subsession) {
        fStreamId = strDup(streamId);
        fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
    }

FrameSink::~FrameSink() {
    delete[] fReceiveBuffer;
    delete[] fStreamId;
}

void FrameSink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes,
        struct timeval presentationTime, unsigned durationInMicroseconds) {
    FrameSink* sink = (FrameSink*)clientData;
    sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
}

#include <iostream>
unsigned char const start_code[4] = {0x00, 0x00, 0x00, 0x01};

void FrameSink::afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
        struct timeval presentationTime, unsigned /*durationInMicroseconds*/) {
    int status;
    int bufId;

    status = Buf_getEmpty(gRingBuffer, &bufId, BUF_NO_WAIT);
    if (status != BUF_OK) {
        std::cout << "No empty ring buffer to use!!!" << std::endl;
        // Drop this frame.
        // Then continue, to request the next frame of data:
        continuePlaying();
        return;
    }

    BufInfo *info = Buf_getBufInfo(gRingBuffer, bufId);

    // Copy a start code before erery H264 frame.
    memcpy(info->virtAddr, start_code, 4);
    memcpy(info->virtAddr + 4, (void*)fReceiveBuffer, frameSize);

    info->size = frameSize + 4;

    status = Buf_putFull(gRingBuffer, bufId);
    assert(status == BUF_OK);

    // Then continue, to request the next frame of data:
    continuePlaying();
}

Boolean FrameSink::continuePlaying() {
    if (fSource == NULL)
        return False; // sanity check (should not happen)

    // Request the next frame of data from our input source.  "afterGettingFrame()" will get called later, when it arrives:
    fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE,
            afterGettingFrame, this, onSourceClosure, this);
    return True;
}
