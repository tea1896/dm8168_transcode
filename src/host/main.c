#include <stdio.h>
#include <stdlib.h>
#include "mpegts/tsInput.h"

int main(int argc, char ** argv)
{
	printf("******  DM8168 (WELLAV Ltd.) COMPILE TIME :%s %s  ******\n", __DATE__, __TIME__);
	tsInput_Init();
	//dm8168_transcode(argc, argv);
		
	return 0;
}
