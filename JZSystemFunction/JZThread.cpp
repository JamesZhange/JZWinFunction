#include "JZThread.h"

JZThread::JZThread(void)
{
	m_Running = 0;
	m_Thread = 0;
}

JZThread::~JZThread(void)
{
	Stop();
}

int JZThread::Start(void* pArg)
{
	unsigned long ThreadID;
	m_Running = 1;

#if defined(WIN32) || defined(WINCE)
	m_Thread = CreateThread(0,0,ThreadProc,this,0,&ThreadID);
#else
	m_Thread = (HANDLE)malloc(sizeof(pthread_t));
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_RR);
	if( pthread_create( (pthread_t*)m_hThread, &attr, ThreadProc, pArg) != 0 )
	{
		free(m_Thread);
		m_Thread = NULL;
	}
	pthread_attr_destroy(&attr);
#endif
	return 0;
}

void JZThread::Stop()
{
	m_Running = 0;
	m_Event.Abort();
	if(m_Thread)
	{
#if defined(WIN32) || defined(WINCE)
		WaitForSingleObject(m_Thread, INFINITE);
		CloseHandle(m_Thread);
#else
		pthread_join((pthread_t*)m_Thread, NULL);
		free(m_Thread);
#endif
	}
	m_Thread = 0;
}

void JZThread::SetPriority(int priority)
{
	if(m_Thread)
	{
#if defined(WIN32) || defined(WINCE)
		SetThreadPriority(m_Thread, priority);	
#else
		struct sched_param sch;
		int sched_policy = SCHED_RR;

		int max = sched_get_priority_max(sched_policy);
		int min = sched_get_priority_min(sched_policy);
		switch(priority)
		{
		case THREAD_PRIORITY_TIME_CRITICAL:
			sch.sched_priority = max;
			break;
		case THREAD_PRIORITY_HIGHEST:
			sch.sched_priority = min + (max-min) / 6 * 5;
			break;
		case THREAD_PRIORITY_ABOVE_NORMAL:
			sch.sched_priority = min + (max-min) / 6 * 4;
			break;
		case THREAD_PRIORITY_NORMAL:
			sch.sched_priority = min + (max-min) / 6 * 3;
			break;
		case THREAD_PRIORITY_BELOW_NORMAL:
			sch.sched_priority = min + (max-min) / 6 * 2;
			break;
		case THREAD_PRIORITY_LOWEST:
			sch.sched_priority = min + (max-min) / 6 * 1;
			break;
		case THREAD_PRIORITY_IDLE:
			sch.sched_priority = min;
			break;
		default:
			sch.sched_priority = min + (max-min) / 6 * 3;
			break;
		}
		pthread_setschedparam((pthread_t*)m_Thread, sched_policy, &sch);
#endif
	}
}

int JZThread::GetPriority()
{
	if(m_Thread)
	{
#if defined(WIN32) || defined(WINCE)
		return GetThreadPriority(m_Thread);	
#else
		struct sched_param sch;
		int sched_policy = SCHED_RR;
		pthread_getschedparam((pthread_t*)m_Thread, &sched_policy, &sch);

		int max = sched_get_priority_max(sched_policy);
		int min = sched_get_priority_min(sched_policy);
		if(sch.sched_priority >= max)
			return THREAD_PRIORITY_TIME_CRITICAL:
		else if( sch.sched_priority >= min + (max-min) / 6 * 5 )	
			return THREAD_PRIORITY_HIGHEST:
		else if( sch.sched_priority >= min + (max-min) / 6 * 4 )	
			return THREAD_PRIORITY_ABOVE_NORMAL:
		else if( sch.sched_priority >= min + (max-min) / 6 * 3 )	
			return THREAD_PRIORITY_NORMAL:
		else if( sch.sched_priority >= min + (max-min) / 6 * 2 )	
			return THREAD_PRIORITY_BELOW_NORMAL:
		else if( sch.sched_priority >= min + (max-min) / 6 * 1 )	
			return THREAD_PRIORITY_LOWEST:
		else
			return THREAD_PRIORITY_IDLE:
#endif
	}

	return -1;
}
