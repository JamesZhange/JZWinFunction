#pragma once
#include <mmsystem.h>
#include <windows.h>
#include "JZGlobal.h"

class MultimediaTimerOnTimeDelegate;




class MultimediaTimer
{
public:
	MultimediaTimer(MultimediaTimerOnTimeDelegate* OntimeDelegate = NULL);
	~MultimediaTimer();

	UINT StartTimer(UINT uDelay, int isRepeat);
	void StopTimer();

	GETPROP(int, isCreate)
	GET(int, isCreate) {
		return ((NULL == timerID)? NO:YES);
	}
	int tag;

private:
	MMRESULT timerID;

	UINT uResolution;

	MultimediaTimerOnTimeDelegate* mDelegate;
	static void CALLBACK TimeProc(UINT id, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
};




/************************************************************************/
/*        Timer Delegate                                                              */
/************************************************************************/

class MultimediaTimerOnTimeDelegate
{
public:
	virtual void OnMMTimeProc(MultimediaTimer* timer) = 0;

};
