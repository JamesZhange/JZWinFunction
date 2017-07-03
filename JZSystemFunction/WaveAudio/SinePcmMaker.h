#pragma once
#include "..\JZGlobal.h"


class SinePcmMaker
{
public:
	SinePcmMaker();
	~SinePcmMaker();

	// --- ���� ----
	// �趨�����ź�Ƶ��
	GETPROP(float, signFrequency)
	GET(float, _signFrequency) {
		return _signFrequency;
	}
	// �����0~1��
	GETPROP(float, signAmplitude)
	GET(float, _maxAmplitude) {
		return _maxAmplitude;
	}
	// ��ʼ��λ��0~2pi��
	GETPROP(float, signPhase)
	GET(float, _initPhase) {
		return _initPhase;
	}
	// ����Ƶ��
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
	int resetOutPhase();


protected:
	float _signFrequency;		// �趨�����ź�Ƶ��
	float _maxAmplitude;	// �����0~1��
	float _initPhase;			// ��ʼ��λ��0~2pi��

	float _sampleFrequence;	// ����Ƶ��
	float _sampleCycleTime;	// ��������

	int _makerAvailable;


	float roundPhase(float phase);
};

