#include "stdafx.h"
#include "MultimediaTimer.h"
#include "JZGlobal.h"
#pragma comment(lib,"winmm.lib")

JZEvent MultimediaTimer::procEvent;


MultimediaTimer::MultimediaTimer(MultimediaTimerOnTimeDelegate* OntimeDelegate)
{
	mDelegate = OntimeDelegate;
	timerID = NULL;
	tag = 0;
// 	// Set resolution to the minimum supported by the system
// 	TIMECAPS tc;
// 	timeGetDevCaps(&tc, sizeof(TIMECAPS));
// 	uResolution = min(max(tc.wPeriodMin, 0), tc.wPeriodMax);
// 	timeBeginPeriod(uResolution);
	uResolution = 1;
}

MultimediaTimer::~MultimediaTimer()
{
	StopTimer();
	// timeEndPeriod(uResolution);
}

UINT MultimediaTimer::StartTimer(UINT uInterval, int isRepeat)
{
	UINT fuEvent = TIME_ONESHOT;
	if (NO != isRepeat)
	{
		fuEvent = TIME_PERIODIC;
	}

	timerID = timeSetEvent(
		uInterval,
		uResolution,
		TimeProc,
		(DWORD)this,
		fuEvent);

	return timerID;
}

void MultimediaTimer::StopTimer()
{
	if (NULL != timerID)
	{
		// printf("Stop timer, wait signal!\n");
		procEvent.EventWait(INFINITE);
		// printf("Stop timer, wait signal out, kill timer\n");
		timeKillEvent(timerID);
		timerID = NULL;
	}
}

int ddebugcount = 0;
void CALLBACK MultimediaTimer::TimeProc(UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	// printf("in time proc %d\n", ddebugcount);
	procEvent.EventReset();
	MultimediaTimer* pThis = (MultimediaTimer*)dwUser;
	if (NULL != pThis->mDelegate)
	{
		pThis->mDelegate->OnMMTimeProc(pThis);
	}
	// printf("Out time proc %d\n", ddebugcount++);
	procEvent.EventSet();
	// printf("Out time proc Set Event\n");
}
