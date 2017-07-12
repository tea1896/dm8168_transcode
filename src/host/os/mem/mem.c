#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mem.h"
#include "wv_log.h"


void *Mem_malloc(U32 u32Len, const char *filename, U32 u32Line)
{
	void *ptr = NULL;

	ptr = malloc(u32Len);
	if(NULL == ptr)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: malloc fialed in the file[%s, %u]", filename, u32Line);
	}

	return ptr;
}


void Mem_free(void *ptr, const char *filename, U32 u32Line)
{
	if(ptr)
	{
		free(ptr);
	}
	else
	{
		Log_Printf(LOG_LEVEL_WARNING, LOG_MODULE_SYS, "Warnnig: Free NULL Pointer file[%s,%u]", filename, u32Line);
	}
}


void *Mem_resize(void *ptr, U32 u32Len, const char *filename, U32 u32Line)
{
	if(NULL == ptr)
	{
		Log_Printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: Pointer is NULL[%s, %d]", filename, u32Len);
		return NULL;
	}

	ptr = realloc(ptr, u32Len);
	if(NULL == ptr)
	{
		Log_Printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: realloc failed in the file[%s, %d]", filename, u32Line);
	}

	return ptr;
}
