
#ifndef _FRAME_SINK_H_
#define _FRAME_SINK_H_

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <stdlib.h>

class FrameSink: public MediaSink {
    public:
        static FrameSink* createNew(UsageEnvironment& env,
                MediaSubsession& subsession, // identifies the kind of data that's being received
                char const* streamId = NULL); // identifies the stream itself (optional)

    private:
        FrameSink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId);
        // called only by "createNew()"
        virtual ~FrameSink();

        static void afterGettingFrame(void* clientData, unsigned frameSize,
                unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);
        void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
                struct timeval presentationTime, unsigned durationInMicroseconds);

    private:
        // redefined virtual functions:
        virtual Boolean continuePlaying();

    private:
        u_int8_t* fReceiveBuffer;
        MediaSubsession& fSubsession;
        char* fStreamId;
};

#endif
