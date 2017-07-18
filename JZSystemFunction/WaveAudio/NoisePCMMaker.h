#pragma once
#include <random>


typedef float _tNoiseType;



class NoisePCMMaker
{
public:
	NoisePCMMaker();
	~NoisePCMMaker();

	int CreateMaker(float Mean, float StdDev);
	void TeardownMaker();
	_tNoiseType GetOneSamplePoint();


private:
	float mMeanValue;
	float mStandardDeviation;
	
	// v2
	std::default_random_engine *p_generator;
	std::normal_distribution<_tNoiseType>* p_distribution; // ÕýÌ¬·Ö²¼
	
};







