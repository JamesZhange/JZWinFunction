#include "stdafx.h"

#include "JZPCMRecorder.h"



#define JZ_AUDIORECORDER_BUFFER_SIZE	(1 * 1024) //���建������С


// ��̬������ʼ��
int JZPCMRecorder::_isRecorderRunning = NO;
int JZPCMRecorder::_RecordeChunksCount = 0;





JZPCMRecorder::JZPCMRecorder()
{
}


JZPCMRecorder::~JZPCMRecorder()
{
}



/******************************************************************/
/*                           ¼���ӿ�                             */
/******************************************************************/

int JZPCMRecorder::CreateRecorder(unsigned int channels,
	unsigned int samplesPerSec,
	unsigned int bitsPerSample,
	unsigned int cacheBufferNum)
{
	int ret = 0;
	if (cacheBufferNum < 2)
	{
		cacheBufferNum = 2;
	}
	_CacheBufferNum = cacheBufferNum;

	_WaveHdrArray = (PWAVEHDR *)malloc(_CacheBufferNum * sizeof(PWAVEHDR));
	if (NULL != _WaveHdrArray)
	{
		for (int i = 0; i < _CacheBufferNum; i++)
		{
			_WaveHdrArray[i] = reinterpret_cast<PWAVEHDR>(malloc(sizeof(WAVEHDR))); //�������ļ�ͷ�����ڴ�ռ�
			if (NULL == _WaveHdrArray[i])
			{
				TeardownRecorder();
				return -3;
			}
		}
	}
	else
	{
		TeardownRecorder();
		return -3;
	}
	

	_RecordeBufferArray = (PBYTE *)malloc(_CacheBufferNum * sizeof(PBYTE));
	if (NULL != _RecordeBufferArray)
	{
		for (int i = 0; i < _CacheBufferNum; i++)
		{
			_RecordeBufferArray[i] = (PBYTE)malloc(JZ_AUDIORECORDER_BUFFER_SIZE);
			if (NULL == _RecordeBufferArray[i]) {
				TeardownRecorder();
				printf("����¼���ڴ�ʧ�ܣ�");   
				return -3;
			}
		}
	}
	else
	{
		TeardownRecorder();
		return -3;
	}
	

	rcChannels = channels;
	rcSamplesPerSec = samplesPerSec;
	rcBitsPerSample = bitsPerSample;


	//open waveform audo for input     
	waveform.wFormatTag = WAVE_FORMAT_PCM;
	waveform.nChannels = rcChannels;
	waveform.nSamplesPerSec = rcSamplesPerSec;
	waveform.wBitsPerSample = rcBitsPerSample;
	waveform.nAvgBytesPerSec = (waveform.nChannels * waveform.nSamplesPerSec * waveform.wBitsPerSample / 8);// 176400;    
	waveform.nBlockAlign = (waveform.nChannels *waveform.wBitsPerSample / 8); // 4;
	waveform.cbSize = 0;

	if (waveInOpen(&hWaveIn, WAVE_MAPPER, &waveform, (DWORD_PTR)AudioRecorder_Callback, (DWORD_PTR)this, CALLBACK_FUNCTION))
	{
		TeardownRecorder();
		printf("¼���ӿ��޷��򿪣�");
		// MessageBeep(MB_ICONEXCLAMATION);
		// AfxMessageBox(_T("¼���ӿ��޷��򿪣�"));
		return -2;
	}

	for (int i = 0; i < _CacheBufferNum; i++)
	{
		_WaveHdrArray[i]->lpData = (LPSTR)_RecordeBufferArray[i];  //  
		_WaveHdrArray[i]->dwBufferLength = JZ_AUDIORECORDER_BUFFER_SIZE;
		_WaveHdrArray[i]->dwBytesRecorded = 0;
		_WaveHdrArray[i]->dwUser = 0;
		_WaveHdrArray[i]->dwFlags = 0;
		_WaveHdrArray[i]->dwLoops = 1;
		_WaveHdrArray[i]->lpNext = NULL;
		_WaveHdrArray[i]->reserved = 0;
		ret = waveInPrepareHeader(hWaveIn, _WaveHdrArray[i], sizeof(WAVEHDR));

		// Add the buffers     
		ret = waveInAddBuffer(hWaveIn, _WaveHdrArray[i], sizeof(WAVEHDR));    //����������ַ��ӵ������豸��
	}

	_isRecorderRunning = NO;
	_RecordeChunksCount = 0;
	return 0;
}

int JZPCMRecorder::StartRecorder() {
	// Begin sampling     
	m_record = true;
	dwDataLength = 0;
	waveInStart(hWaveIn);  //�������豸����ʼ¼��

	return 0;
}

int JZPCMRecorder::StopRecorder()
{
	m_record = false;
	waveInReset(hWaveIn); // ֹͣ¼��
	waveInClose(hWaveIn); // �ر������豸
	return 0;
}

int JZPCMRecorder::TeardownRecorder()
{
	if (NULL != _WaveHdrArray)
	{
		for (int i=0; i<_CacheBufferNum; i++)
		{
			if (NULL != _WaveHdrArray[i])
			{
				waveInUnprepareHeader(hWaveIn, _WaveHdrArray[i], sizeof(WAVEHDR));
				free(_WaveHdrArray[i]);
			}
			
		}
		
		free(_WaveHdrArray);
		_WaveHdrArray = NULL;
	}
	
	if (NULL != _RecordeBufferArray)
	{
		for (int i=0; i<_CacheBufferNum; i++)
		{
			if (NULL != _RecordeBufferArray[i])
			{
				free(_RecordeBufferArray[i]);
			}
		}
		free(_RecordeBufferArray);
		_RecordeBufferArray = NULL;
	}

	return 0;
}





/******************************************************************/
/*                           �ص�                                 */
/******************************************************************/

DWORD JZPCMRecorder::AudioRecorder_Callback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	JZPCMRecorder* recorder = (JZPCMRecorder*)dwParam2;
	// ��Ϣswitch
	switch (uMsg)
	{
	case WIM_OPEN: // �豸�ɹ��Ѵ�
	{
		_RecordeChunksCount = 0;
		_isRecorderRunning = YES;
	}
		break;

		
	case WIM_DATA: // ����������������	
	{
		PWAVEHDR recordWavHeader = (PWAVEHDR)dwParam1;

		// implementAlgorithmAndPlay(recordWavHeader, rcChannels, rcBitsPerSample, rcSamplesPerSec);

		memset(recordWavHeader->lpData, 0, recordWavHeader->dwBytesRecorded);

		waveInAddBuffer(hwavein, (PWAVEHDR)dwParam1, sizeof(WAVEHDR));//����������ӻص��豸��
	}
		break;

	case WIM_CLOSE: // �����ɹ����
	{
		_isRecorderRunning = NO;
	}
		break;

	default:
		break;
	}
	return 0;
}
