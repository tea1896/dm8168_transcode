/*
 * =====================================================================================
 *
 *       Filename:  mutex.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/11/17 09:13:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <pthread.h>

#include "mutex.h"
#include "log/log.h"

static pthread_mutex_t s_mutex;

S32 Mutex_Init(void)
{
	S32 ret = 0;

	ret = pthread_mutex_init(&s_mutex, NULL);

	if(0 != ret)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: call pthread_mutex_init failed!");

		return MUTEX_ERR;
	}

	return SUCCESS;
}


S32  Mutex_Lock(void)
{
	S32 ret = 0;

	ret = pthread_mutex_lock(&s_mutex);

	if(0 != ret)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: call pthread_mutex_lock fail!!");

		return MUTEX_ERR;
	}

	return SUCCESS;
}


S32  Mutex_Unlock(void)
{
	S32 ret = 0;

	ret = pthread_mutex_unlock(&s_mutex);

	if(0 != ret)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: call pthread_mutex_unlock fail!!");

		return MUTEX_ERR;
	}

	return SUCCESS;
}


S32 Mutex_Destroy(void)
{
	S32 ret = 0;

	ret = pthread_mutex_destroy(&s_mutex);

	if(0 != ret)
	{
		LOG_PRINTF(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "Error: call pthread_mutex_destroy fail!!");

		return MUTEX_ERR;
	}

	return SUCCESS;
}
