#include "stdafx.h"
#include "MultimediaTimer.h"
#include "JZGlobal.h"
#pragma comment(lib,"winmm.lib")


MultimediaTimer::MultimediaTimer(MultimediaTimerOnTimeDelegate* OntimeDelegate)
{
	mDelegate = OntimeDelegate;
	timerID = NULL;
	tag = 0;
	// Set resolution to the minimum supported by the system
	TIMECAPS tc;
	timeGetDevCaps(&tc, sizeof(TIMECAPS));
	uResolution = min(max(tc.wPeriodMin, 0), tc.wPeriodMax);
	timeBeginPeriod(uResolution);
}

MultimediaTimer::~MultimediaTimer()
{
	StopTimer();
	timeEndPeriod(uResolution);
}

UINT MultimediaTimer::StartTimer(UINT uDelay, int isRepeat)
{
	UINT fuEvent = TIME_ONESHOT;
	if (NO != isRepeat)
	{
		fuEvent = TIME_PERIODIC;
	}

	timerID = timeSetEvent(
		uDelay,
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
		timeKillEvent(timerID);
		timerID = NULL;
	}
}

void CALLBACK MultimediaTimer::TimeProc(UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	MultimediaTimer* pThis = (MultimediaTimer*)dwUser;
	if (NULL != pThis->mDelegate)
	{
		pThis->mDelegate->OnMMTimeProc(pThis);
	}

}
