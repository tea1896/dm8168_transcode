#include <unistd.h>
#include <sys/eventfd.h>

#include "event.h"
#include "log/log.h"


S32 Event_CreateFd(void)
{
	S32 fd = eventfd(0, 0);

	return fd;
}


S32 Event_Read(S32 fd)
{
	S32 ret = 0;
	S32 num = 0;
	U64 u64Val = 0;

	num = read(fd, &u64Val, sizeof(u64Val));
	if(num != sizeof(u64Val))
	{
		ret = EVENT_ERR;
	}

	return ret;
}


S32 Event_Write(S32 fd)
{
	S32 ret = 0;
	S32 num = 0;
	U64 u64Val = 1;

	num = write(fd, &u64Val, sizeof(u64Val));
	if(num != sizeof(u64Val))
	{
		//printf("Event_Write error\n");
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: call write failed!");
		ret = EVENT_ERR;
	}

	return ret;

}



