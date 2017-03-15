#pragma once
#include <Windows.h>
#include <MMSystem.h>
#include "JZGlobal.h"
#include "ANCGlobalDefine.h"


class JZPCMRecorder;


/*****************************/
/*        ¼������            */
/*****************************/ 
class JZPCMRecorderDelegate
{
public:
	virtual void OnJZPCMRecorderOpen(JZPCMRecorder* recorder) = 0;
	virtual void OnJZPCMRecorderGetData(JZPCMRecorder* recorder, char* pBuff, unsigned int nBuffLen) = 0;
	virtual void OnJZPCMRecorderClose(JZPCMRecorder* recorder) = 0;
};




/*****************************/
/*        ¼��                */
/*****************************/

#define JZAUDIORECORDERBUFFERNUM		10		// ¼������buffer����


class ANCWORKSPACEDLL_API JZPCMRecorder
{
public:
	JZPCMRecorder();
	~JZPCMRecorder();

public:
	int CreateRecorder(unsigned int channels,
		unsigned int samplesPerSec, 
		unsigned int bitsPerSample,
		unsigned int cacheBufferNum = JZAUDIORECORDERBUFFERNUM);
	int StartRecorder();
	int StopRecorder();
	int TeardownRecorder();

	JZPCMRecorderDelegate * delegate;
	int tag;

	//--- ���� ---
	GETPROP(int, Channels)
	GET(int, Channels) {
		return rcChannels;
	}

	GETPROP(int, SamplesPerSec)
	GET(int, SamplesPerSec) {
		return rcSamplesPerSec;
	}

	GETPROP(int, BitsPerSample)
	GET(int, BitsPerSample) {
		return rcBitsPerSample;
	}

	GETPROP(int, isRecorderRunning)
	GET(int, isRecorderRunning) {
		return _isRecorderRunning;
	}

	GETPROP(int, RecordeChunksCount)
	GET(int, RecordeChunksCount) {
		return _RecordeChunksCount;
	}

private:

	// ¼��
	int rcChannels;
	int rcSamplesPerSec;
	int rcBitsPerSample;

	// bool m_record;	// m_record��ʾ�Ƿ�����¼��


	WAVEFORMATEX waveform;    //WAV�ļ�ͷ������Ƶ��ʽ
	DWORD dwDataLength, dwRepetitions; //dwDataLength���е����ݳ��ȣ�dwRepetitions�ظ�����
	HWAVEIN hWaveIn;     //�����豸���

	unsigned int _CacheBufferNum;
	PWAVEHDR * _WaveHdrArray;//�����ļ�ͷ
	PBYTE * _RecordeBufferArray;  // ¼�����뻺����

	static int _isRecorderRunning;
	static int _RecordeChunksCount;

	//---
	// WaveXAPI�ص�����
	static DWORD(CALLBACK AudioRecorder_Callback)(	
		HWAVEIN hwavein,							//	.�����豸
		UINT uMsg,									//	.��Ϣ
		DWORD dwInstance,							//	.����
		DWORD dwParam1,								//	.�����õĻ�����ָ��
		DWORD dwParam2);							//	.����
};

