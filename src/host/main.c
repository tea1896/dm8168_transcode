#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "appGlobal.h"
#include "mpegts/tsInput.h"
#include "dm8168/dm8168.h"
#include "wv_log.h"

void main_CompileInfo(void)
{
    LOG_PRINTF(LOG_LEVEL_NOTICE, LOG_MODULE_SYS, "DM8168 (WELLAV Ltd.) COMPILE TIME :%s %s VER %d ******", __DATE__, __TIME__,SW_VER);
}

int main(int argc, char ** argv)
{
    /* Init log file */
    Log_Init(LOG_FILE_PATH, 1024);

    /* Get compile information */
    main_CompileInfo();

    /* Init ts input */
	assert(0 == tsInput_Init());

    
    tsInput_StartChannel(0,  (int8_t *)"test.ts");
    tsInput_StopChannel(0);
    tsInput_StartChannel(0,  (int8_t *)"test1.ts");
	tsInput_StopChannel(1);	
	return 0;
}


