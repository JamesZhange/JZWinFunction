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
	int EventWait(unsigned long ms);
	void Abort();
private:
	HANDLE m_Event;
	int m_bAbort;
};



/****************************************************************/
/*                       ÓÃÀý                                   */
/***************************************************************/

/*******************************************

JZLock alock;

alock.Lock();
some code 1
alock.Unlock();

alock.Lock();
some code 2
alock.Unlock();

{
	JZScopeLock(&alock);
	some code 3;
}

//------------
class AThreadClass
{
protected:
	JZEvent m_Event;
};

AThreadClass::Stop()
{
	m_Event.Abort();
}

*******************************************/
