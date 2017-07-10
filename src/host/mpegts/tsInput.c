#include <stdio.h>
#include <unistd.h>
#include "tsInput.h"
#ifdef __cplusplus
extern "C"
{
#include "libavutil/opt.h"
#include "libavutil/time.h"
#include "libavutil/mathematics.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/audio_fifo.h"
#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
};
#endif

int tsInput_Init(void)
{
	av_register_all();
	avdevice_register_all();
	avformat_network_init();
	
	printf("tsInput_Init !\n");
	return 0;
}