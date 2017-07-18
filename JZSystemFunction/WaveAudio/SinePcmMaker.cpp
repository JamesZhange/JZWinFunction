#include <math.h>
#include "SinePcmMaker.h"


SinePcmMaker::SinePcmMaker()
{
	_makerAvailable = NO;
}


SinePcmMaker::~SinePcmMaker()
{
}

int SinePcmMaker::createMaker(float signFrequency, float maxAmplitude, float initPhase, float sampleFrequence)
{
	if ((signFrequency > 0) && (maxAmplitude >0 ) && (sampleFrequence > 0))
	{
		if (sampleFrequence > (2 * signFrequency))
		{
			_signFrequency = signFrequency;
			_maxAmplitude = maxAmplitude;
			_initPhase = roundPhase(initPhase);
			_sampleFrequence = sampleFrequence;

			_sampleCycleTime = 1 / _sampleFrequence;

			_makerAvailable = YES;
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return -2;
	}
	
}

int SinePcmMaker::sinePcmForSampleN(int sampleN)
{
	// 如果全部定点化，那这个函数可以用数组来简化，极大提高效率，这也是写这个类的初衷
	int SamplesN = _maxAmplitude * sin(2 * PI * _signFrequency * (sampleN*_sampleCycleTime) + _initPhase); 

	return SamplesN;
}

int SinePcmMaker::sinePcmForSampleN(int sampleN, float customPhase)
{
	// 如果全部定点化，那这个函数可以用数组来简化，极大提高效率，这也是写这个类的初衷
	int SamplesN = _maxAmplitude * sin(2 * PI * _signFrequency * (sampleN*_sampleCycleTime) + customPhase);

	return SamplesN;
}

//
float SinePcmMaker::sineAmplitudeForSampleN(int sampleN, float customPhase)
{
	float SamplesN = sin(2 * PI * _signFrequency * (sampleN*_sampleCycleTime) + customPhase);
	return SamplesN;
}

int SinePcmMaker::resetOutPhase()
{
	return 0;
}

float SinePcmMaker::roundPhase(float phase)
{
	float absphase = JZ_FABS(phase);
	if ((0 <= absphase) && (absphase <= 2*PI))
	{
		return absphase;
	}
	else
	{
		float roundphase = absphase - 2 * PI;
		while (roundphase > 2*PI)
		{
			roundphase = absphase - 2 * PI;
		}
		return roundphase;
	}
}
