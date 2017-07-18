#include "NoisePCMMaker.h"
#include <windows.h>
#include <time.h>
#include <iostream>
#include <random>
#include <string>

// #include <tchar.h>
// #include <mmsystem.h>
// #include <cstdint>
// #include <algorithm>
// #include <iostream>
// #include <functional>
// #include <algorithm>
// #include <vector>

// #define USEINGFUNCTION01
#define USEINGFUNCTION02



NoisePCMMaker::NoisePCMMaker()
{
	mMeanValue = 0;
	mStandardDeviation = 1;

	p_generator = NULL;
	p_distribution = NULL;
}


NoisePCMMaker::~NoisePCMMaker()
{
	TeardownMaker();
}



/********************************************************************************/
/*																				*/
/*	 https://github.com/ToDayL/White-Gaussian-Noise-WGN/blob/master/WGN.cpp		*/
/*																				*/
/********************************************************************************/

#ifdef USEINGFUNCTION01

int NoisePCMMaker::CreateMaker(float Mean, float StdDev)
{
	mMeanValue = Mean;
	mStandardDeviation = StdDev;
	srand((unsigned)(time(NULL)));

	return 0;
}


float NoisePCMMaker::GetOneSamplePoint()
{
	double rn1 = rand()*(1. / RAND_MAX);
	double rn2 = rand()*(1. / RAND_MAX);
	double R = sqrt(-2. * log(rn2));
	double theta = 2 * 3.14159265 * rn1;
	double Z = R*cos(theta);
	double x = mMeanValue + Z * mStandardDeviation;
	return x;
}


#endif // USEINGFUNCTION01






/********************************************************************************/
/*																				*/
/*	          https://github.com/avakar/noisegen              */
/*																				*/
/********************************************************************************/

#ifdef USEINGFUNCTION02

int NoisePCMMaker::CreateMaker(float Mean, float StdDev)
{
	TeardownMaker();
	p_generator = new std::default_random_engine(time(NULL));
	p_distribution = new std::normal_distribution<_tNoiseType>(Mean, StdDev);

	return 0;
}


_tNoiseType NoisePCMMaker::GetOneSamplePoint()
{
	if ((NULL != p_generator) && (NULL != p_distribution))
	{
		return (*p_distribution)(*p_generator);
	}
}


void NoisePCMMaker::TeardownMaker()
{
	if (NULL != p_generator)
	{
		delete p_generator;
		p_generator = NULL;
	}
	if (NULL != p_distribution)
	{
		delete p_distribution;
		p_distribution = NULL;
	}
}

#endif
