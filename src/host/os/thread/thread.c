#include "wv_log.h"
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


int get_thread_policy(pthread_attr_t *attr)
{
    int policy;
    int rs = pthread_attr_getschedpolicy(attr,&policy);
    assert(rs==0);
    switch(policy)
    {
        case SCHED_FIFO:
            LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_SYS,"policy= SCHED_FIFO\n");
            break;
        case SCHED_RR:
            LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_SYS,"policy= SCHED_RR");
            break;
        case SCHED_OTHER:
            LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_SYS,"policy=SCHED_OTHER\n");
            break;
        default:
            LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_SYS,"policy=UNKNOWN\n");
            break;
    }
    return policy;
}

void show_thread_priority(pthread_attr_t *attr,int policy)
{
  int priority = sched_get_priority_max(policy);
  assert(priority!=-1);
  LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_SYS,"max_priority=%d\n",priority);
  priority= sched_get_priority_min(policy);
  assert(priority!=-1);
  LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_SYS,"min_priority=%d\n",priority);
}

int get_thread_priority(pthread_attr_t *attr)
{
  struct sched_param param;
  int rs = pthread_attr_getschedparam(attr,&param);
  assert(rs==0);
  LOG_PRINTF(LOG_LEVEL_INFO, LOG_MODULE_SYS,"priority=%d",param.__sched_priority);
  return param.__sched_priority;
}

void set_thread_policy(pthread_attr_t *attr,int policy)
{
  int rs = pthread_attr_setschedpolicy(attr,policy);
  assert(rs==0);
  get_thread_policy(attr);
}



