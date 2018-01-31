#pragma once
#include <windows.h>

class JZLock
{
public:
	JZLock(void);
	~JZLock(void);

	void Lock();
	void Unlock();

private:
#if defined(WIN32) || defined(WINCE)
	CRITICAL_SECTION m_cs;
#else
	pthread_mutex_t* m_pmutex;
#endif
};


class JZScopeLock
{
public:
	JZScopeLock(JZLock* lock)
	{
		lock->Lock();
		m_Lock = lock;
	}
	~JZScopeLock(void)
	{
		m_Lock->Unlock();
	}

private:
	JZLock* m_Lock;
};


class JZEvent
{
public:
	JZEvent();
	~JZEvent();

	void EventSet();
	void EventReset();
	int EventWait(unsigned long ms);  // INFINITE:一直等待
	void Abort();
private:
	HANDLE m_Event;
	int m_bAbort;
};



/****************************************************************/
/*                       用例                                   */
/***************************************************************/

/*******************************************

JZLock alock;

// exp1:
alock.Lock();
some code 1
alock.Unlock();

alock.Lock();
some code 2
alock.Unlock();

// exp2:
{
	JZScopeLock scopeLock(&alock);
	some code 3;
}

//------------
// exp3 线程信号:
class AThreadClass
{
protected:
	JZEvent m_Event;
};

AThreadClass::Stop()
{
	m_Event.Abort();  // 其他 EventWait 等待的地方会收到该消息并开始后续执行
}

*******************************************/
