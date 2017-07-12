#include "log/log.h"
#include "thread.h"

pthread_t Thread_NewDetach(ThreadFunc func, void *thr_data, const char *funcname, const char *filename, U32 line)
{
	S32 ret = 0;
	pthread_attr_t thr_attr;
	pthread_t tid;

	ret = pthread_attr_init(&thr_attr);
	if(0 != ret)
	{
		Log_Printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "[%s][%s]%d: call func:pthread_attr_init failed\r\n", filename, funcname, line);
		goto end;
	}

	ret = pthread_attr_setdetachstate(&thr_attr, PTHREAD_CREATE_DETACHED);
	if(0 != ret)
	{
		Log_Printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "[%s][%s]%d: call func:pthread_attr_setdetachstate\r\n", filename, funcname, line);
		goto end;
	}

	ret = pthread_create(&tid, &thr_attr, func, thr_data);
	if(0 != ret)
	{
		Log_Printf(LOG_LEVEL_ERROR, LOG_MODULE_SYS, "[%s][%s]%d: call func:pthread_create failed\r\n", filename, funcname, line);
		goto end;
	}
	else
	{
		Log_Printf(LOG_LEVEL_INFO, LOG_MODULE_SYS,"[%s]%d: Create %s thread success!!\r\n", filename, line, funcname);
	}

end:
	pthread_attr_destroy(&thr_attr);
	
	if(ret)
	{
		return 0;
	}
	else
	{
		return tid;
	}

}
