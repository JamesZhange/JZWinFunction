#include "JZPCMRecorder.h"

// #define JZ_AUDIORECORDER_BUFFER_SIZE	(10 * 1024)  //���建������С


// ��̬������ʼ��
int JZPCMRecorder::_isRecorderRunning = NO;





JZPCMRecorder::JZPCMRecorder()
{
	_CacheBufferNum = 4;
	_CacheBufferSize = 1024;
}


JZPCMRecorder::~JZPCMRecorder()
{
}



int JZPCMRecorder::waveInputDeviceCapacity(int &devNum, WAVEINCAPS** ppWavInDevCaqs)
{
	devNum = 0;
	*ppWavInDevCaqs = NULL;
	devNum = waveInGetNumDevs();
	if (devNum > 0)
	{
		*ppWavInDevCaqs = (WAVEINCAPS*)malloc(devNum * sizeof(WAVEINCAPS));
		for (int i = 0; i < devNum; i++)
		{
			MMRESULT mmResult = waveInGetDevCaps(i, ((WAVEINCAPS*)(*ppWavInDevCaqs) + i), sizeof(WAVEINCAPS));
		}
		
		return 0;
	}

	return 1;

}




/******************************************************************/
/*                           ¼���ӿ�                             */
/******************************************************************/

int JZPCMRecorder::CreateRecorder(unsigned int channels,
	unsigned int samplesPerSec,
	unsigned int bitsPerSample,
	int nDeviceID,
	unsigned int cacheBufferSize,
	unsigned int cacheBufferNum
	)
{
	int ret = 0;
	if (cacheBufferNum < 2)
	{
		cacheBufferNum = 2;
	}
	_CacheBufferNum = cacheBufferNum;

	if (cacheBufferSize < 0)
	{
		cacheBufferSize = 1024;
	}
	_CacheBufferSize = cacheBufferSize;

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
			_RecordeBufferArray[i] = (PBYTE)malloc(_CacheBufferSize);
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

	if (waveInOpen(&hWaveIn, nDeviceID, &waveform, (DWORD_PTR)AudioRecorder_Callback, (DWORD_PTR)this, CALLBACK_FUNCTION))
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
		_WaveHdrArray[i]->dwBufferLength = _CacheBufferSize;
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
	return 0;
}

int JZPCMRecorder::StartRecorder() {
	// Begin sampling     
	_isRecorderRunning = YES;
	dwDataLength = 0;
	waveInStart(hWaveIn);  //�������豸����ʼ¼��

	return 0;
}

int JZPCMRecorder::StopRecorder()
{
	_isRecorderRunning = NO;
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
	JZPCMRecorder* recorder = (JZPCMRecorder*)dwInstance;
	// ��Ϣswitch
	switch (uMsg)
	{
	case WIM_OPEN: // �豸�ɹ��Ѵ�
	{
		if (NULL != recorder->delegate)
		{
			recorder->delegate->OnJZPCMRecorderOpen(recorder);
		}
	}
		break;

		
	case WIM_DATA: // ����������������	
	{
		if (YES == _isRecorderRunning)
		{
			PWAVEHDR recordWavHeader = (PWAVEHDR)dwParam1;

			if (NULL != recorder->delegate)
			{
				recorder->delegate->OnJZPCMRecorderGetData(recorder, recordWavHeader->lpData, recordWavHeader->dwBytesRecorded);
			}

			memset(recordWavHeader->lpData, 0, recordWavHeader->dwBytesRecorded);

			waveInAddBuffer(hwavein, (PWAVEHDR)dwParam1, sizeof(WAVEHDR)); // ����������ӻص��豸��
		}
		
	}
		break;

	case WIM_CLOSE: // �����ɹ����
	{
		if (NULL != recorder->delegate)
		{
			recorder->delegate->OnJZPCMRecorderClose(recorder);
		}
	}
		break;

	default:
		break;
	}
	return 0;
}
