#include <stdio.h>
#include <stdlib.h>
#include "mpegts/tsInput.h"
#include "dm8168/dm8168.h"

int main(int argc, char ** argv)
{
	printf("******  DM8168 (WELLAV Ltd.) COMPILE TIME :%s %s  ******\n", __DATE__, __TIME__);
	tsInput_Init();
    
	dm8168_transcode(argc, argv);
		
	return 0;
}


