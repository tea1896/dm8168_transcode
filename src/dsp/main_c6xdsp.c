/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <mcfw/src_bios6/utils/utils.h>
#include <mcfw/interfaces/link_api/system.h>
#include <mcfw/src_bios6/links_common/system/system_priv_common.h>

Int32 System_start(Task_FuncPtr chainsMainFunc);

Int32 AppLink_init();
Int32 AppLink_deInit();

Void C6XDSP_main(UArg arg0, UArg arg1)
{
    AppLink_init();

    char ch;

    while (1)
    {
        Task_sleep(100);
        Utils_getChar(&ch, BIOS_NO_WAIT);
        if (ch == 'x')
            break;
    }

    AppLink_deInit();
}

int main(void)
{
    float dspFreq = Utils_getFrequency(PLL_MODULE_DSP);

    Vps_printf("***** VIDEO Firmware build time %s %s %s %s\n", __TIME__,__DATE__,__GNUC__,__VERSION__);

    if (dspFreq <= 0)  
        dspFreq = SYSTEM_DSP_FREQ;
    Utils_setCpuFrequency(dspFreq * 1000 * 1000);

    System_start(C6XDSP_main);

    BIOS_start();

    return (0);
}
