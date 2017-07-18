#pragma once
#include "..\JZGlobal.h"


class SinePcmMaker
{
public:
	SinePcmMaker();
	~SinePcmMaker();

	// --- 属性 ----
	// 设定正弦信号频率
	GETPROP(float, signFrequency)
	GET(float, _signFrequency) {
		return _signFrequency;
	}
	// 振幅（0~1）
	GETPROP(float, signAmplitude)
	GET(float, _maxAmplitude) {
		return _maxAmplitude;
	}
	// 初始相位（0~2pi）
	GETPROP(float, signPhase)
	GET(float, _initPhase) {
		return _initPhase;
	}
	// 采样频率
	GETPROP(float, sampleFrequence)
	GET(float, _sampleFrequence) {
		return _sampleFrequence;
	}

	GETPROP(int, makerAvailable)
	GET(int, _makerAvailable) {
		return _makerAvailable;
	}
	// 
	int createMaker(float signFrequency, float maxAmplitude, float initPhase, float sampleFrequence);
	int sinePcmForSampleN(int sampleN);
	int sinePcmForSampleN(int sampleN, float customPhase);  // for debug
	float sineAmplitudeForSampleN(int sampleN, float customPhase);  // 浮点值输出
	int resetOutPhase();


protected:
	float _signFrequency;		// 设定正弦信号频率
	float _maxAmplitude;	// 振幅（0~1）
	float _initPhase;			// 初始相位（0~2pi）

	float _sampleFrequence;	// 采样频率
	float _sampleCycleTime;	// 采样周期

	int _makerAvailable;


	float roundPhase(float phase);
};

