#pragma once
#include <Windows.h>
#include <MMSystem.h>
#include "JZGlobal.h"


class JZPCMRecorder;


/*****************************/
/*        ¼������            */
/*****************************/ 
class JZPCMRecorderDelegate
{
public:
	virtual void OnJZPCMRecorderOpen(JZPCMRecorder* recorder) = 0;
	virtual void OnJZPCMRecorderGetData(JZPCMRecorder* recorder, char* pBuff, unsigned int nBuffLen) = 0; // ע��pBuff��¼�������û�������delegate�ڲ������ͷ�
	virtual void OnJZPCMRecorderClose(JZPCMRecorder* recorder) = 0;
};




/*****************************/
/*        ¼��                */
/*****************************/

#define AUDIORECORDER_BUFFER_SIZE	(10 * 1024)  //���建������С


class JZPCMRecorder
{
public:
	JZPCMRecorder();
	~JZPCMRecorder();

public:
	int CreateRecorder(unsigned int channels,
		unsigned int samplesPerSec,
		unsigned int bitsPerSample,
		int nDeviceID = WAVE_MAPPER,
		unsigned int cacheBufferSize = AUDIORECORDER_BUFFER_SIZE,
		unsigned int cacheBufferNum = 10
		);
	int StartRecorder();
	int StopRecorder();
	int TeardownRecorder();

	JZPCMRecorderDelegate * delegate;
	

	static int JZPCMRecorder::waveInputDeviceCapacity(int &devNum, WAVEINCAPS** ppWavInDevCaqs);


	//--- ���� ---
	int tag;

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


private:

	static int _isRecorderRunning;

	// ¼��
	int rcChannels;
	int rcSamplesPerSec;
	int rcBitsPerSample;

	// bool m_record;	// m_record��ʾ�Ƿ�����¼��


	WAVEFORMATEX waveform;    //WAV�ļ�ͷ������Ƶ��ʽ
	DWORD dwDataLength, dwRepetitions; //dwDataLength���е����ݳ��ȣ�dwRepetitions�ظ�����
	HWAVEIN hWaveIn;     //�����豸���

	unsigned int _CacheBufferNum;
	unsigned int _CacheBufferSize;
	PWAVEHDR * _WaveHdrArray;//�����ļ�ͷ
	PBYTE * _RecordeBufferArray;  // ¼�����뻺����

	

	//---
	// WaveXAPI�ص�����
	static DWORD(CALLBACK AudioRecorder_Callback)(	
		HWAVEIN hwavein,							//	.�����豸
		UINT uMsg,									//	.��Ϣ
		DWORD dwInstance,							//	.����
		DWORD dwParam1,								//	.�����õĻ�����ָ��
		DWORD dwParam2);							//	.����
};

