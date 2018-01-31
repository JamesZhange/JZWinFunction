#include "JZScopeLock.h"

JZLock::JZLock(void)
{
#if defined(WIN32) || defined(WINCE)
	InitializeCriticalSection(&m_cs);
#else

	pthread_mutexattr_t attrs;

	m_pmutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if(m_pmutex)
	{
		pthread_mutexattr_init(&attrs);
		pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_RECURSIVE_NP);

		if( pthread_mutex_init(m_pmutex, &attrs) != 0)
		{
			free(m_pmutex);
			m_pmutex = NULL;
		}
	}
#endif
}

JZLock::~JZLock(void)
{
#if defined(WIN32) || defined(WINCE)
	DeleteCriticalSection(&m_cs);
#else
	if(m_pmutex)
	{
		pthread_mutex_destroy(m_pmutex);
		free(m_pmutex);
	}
#endif
}

void JZLock::Lock()
{
#if defined(WIN32) || defined(WINCE)
	EnterCriticalSection(&m_cs);
#else
	if(m_pmutex)
		pthread_mutex_lock(m_pmutex);
#endif
}

void JZLock::Unlock()
{
#if defined(WIN32) || defined(WINCE)
	LeaveCriticalSection(&m_cs);
#else
	if(m_pmutex)
		pthread_mutex_unlock(m_pmutex);
#endif
}

JZEvent::JZEvent()
{
	m_bAbort = 0;
#if defined(WIN32) || defined(WINCE)
	m_Event = CreateEvent(0,0,0,0);
#else
	m_Event = (sem_t*)malloc(sizeof(sem_t));
	if(sem_init((sem_t*)m_Event, 0, 0) != 0)
	{
		free(m_Event);
		m_Event = 0;
	}
#endif
}

JZEvent::~JZEvent()
{
	if(m_Event)
	{
#if defined(WIN32) || defined(WINCE)
		CloseHandle(m_Event);
#else
		sem_destroy((sem_t*)m_Event);
		free(m_Event);

#endif
	}
}

void JZEvent::EventSet()
{
	if(m_Event)
	{
#if defined(WIN32) || defined(WINCE)

		SetEvent(m_Event);
#else
		sem_post((sem_t*)m_Event);

#endif
	}
}

// [2017-07-28] james
void JZEvent::EventReset() 
{
	if (m_Event)
	{
#if defined(WIN32) || defined(WINCE)

		ResetEvent(m_Event);
#else
		// ?
#endif
	}
}

int JZEvent::EventWait(unsigned long ms)
{
#if defined(WIN32) || defined(WINCE)
	DWORD ret = WaitForSingleObject(m_Event, ms);
	if(!m_bAbort)
	{
		if(ret == WAIT_OBJECT_0) 
			return 0;
		else if(ret == WAIT_TIMEOUT) 
			return 1;
	}
#else
	if(ms == INFINITE)
	{
		int ret = sem_wait((sem_t*)m_Event);
		if(!m_bAbort && ret == 0) 
			return 0;
	}
	else
	{
		struct timespec time;
		if(clock_gettime(CLOCK_REALTIME, &time) == 0)
		{
			timeout += time.tv_nsec / 1000000;
			time.tv_sec += timeout / 1000;
			time.tv_nsec = (timeout % 1000) * 1000000 + (time.tv_nsec % 1000000);
			int ret = sem_timed_wait((sem*)m_Event, &time);
			if(!m_bAbort)
			{
				if(ret == 0) 
					return 0;
				else if(errno == ETIMEDOUT)
					return 1;
			}
		}
	}
#endif
	m_bAbort = 0;
	return -1;
}

void JZEvent::Abort()
{
	m_bAbort = 1;
	EventSet();
}