#include "ReadPCMFileManager.h"

int FindString(char *s, int s_len, char *d, int d_len, int *pos);



ReadPCMFileManager::ReadPCMFileManager()
{
	pPCMFile = NULL;
	_isAvailable = NO;
	WaveHeaderOffset = 0;
	_isPcmHaveHeader = NO;
	_currentFileFullPath = _T("");
	_currentFilePath = _T("");
	_currentFileName = _T("");
	memset(&_FileFormat, 0, sizeof(_FileFormat));
}

ReadPCMFileManager::ReadPCMFileManager(CString filepath)
{
	pPCMFile = NULL;
	_isAvailable = NO;
	WaveHeaderOffset = 0;
	_isPcmHaveHeader = NO;
	_currentFileFullPath = _T("");
	_currentFilePath = _T("");
	_currentFileName = _T("");
	memset(&_FileFormat, 0, sizeof(_FileFormat));

	int ret = openFileWithPath(filepath);
}


ReadPCMFileManager::~ReadPCMFileManager()
{
	closeCurrentFile();
}




int ReadPCMFileManager::openFileWithPath(CString filepath)
{
	closeCurrentFile();

	pPCMFile = _wfopen(filepath.GetBuffer(), _T("rb"));
	if (NULL != pPCMFile)
	{
		saveCurrentPath(filepath);

		fseek(pPCMFile, 0, SEEK_END);
		int fileLen = ftell(pPCMFile);
		fseek(pPCMFile, 0, SEEK_SET);

		//跳过wav头
		WaveHeaderOffset = getWaveHeaderOffset();
		fseek(pPCMFile, WaveHeaderOffset, SEEK_SET); // 这样下次取数据时候取到的是 PCM 数据
		fileLen = fileLen - WaveHeaderOffset;

		if (fileLen > 0)
		{
			_isAvailable = YES;
			return 0;
		}
		else
		{
			closeCurrentFile();
			return 2;
		}
	}
	return -3;
}

int ReadPCMFileManager::closeCurrentFile()
{
	if (NULL != pPCMFile)
	{
		fclose(pPCMFile);
		pPCMFile = NULL;
	}
	_isAvailable = NO;
	WaveHeaderOffset = 0;
	_isPcmHaveHeader = NO;
	memset(&_FileFormat, 0, sizeof(_FileFormat));
	cleanCurrentPath();
	return 0;
}

int ReadPCMFileManager::loadNextPCMDataLen(unsigned int needLen, strcPCMData& outData, int loopload)
{
	if (0 != needLen)
	{
		if ((NULL != outData.pPCMBuffer) && (outData.nBufferLen >= needLen))
		{
			int readlen = fread(outData.pPCMBuffer, 1, needLen, pPCMFile);
			if ((readlen < needLen) && (0 != loopload))
			{
				int needadd = needLen - readlen;
				while (needadd > 0)
				{
					fseek(pPCMFile, WaveHeaderOffset, SEEK_SET);
					
					int reReadLen = fread(outData.pPCMBuffer + readlen, 1, needadd, pPCMFile);
					readlen += reReadLen;
					needadd = needLen - readlen;
				}
			}
			else if (0 == readlen) {
				return 1;
			}

			outData.nDataLen = readlen;
			outData.nPCMSamplerate = _FileFormat.dwSamplesPerSec;
			outData.nPCMBitPerSample = _FileFormat.wBitsPerSample;
			outData.nPCMChannels = _FileFormat.wChannels;
			return 0;
		}
		return -2;
	}
	return -3;
}

int ReadPCMFileManager::resetFilePosition()
{
	if (NULL != pPCMFile)
	{
		fseek(pPCMFile, WaveHeaderOffset, SEEK_SET);
	}
	return 0;
}





//**************************************************************//
// 头文件参数解析
// 参数0：input_buf 输入BUF
// 参数1：输入数据的长度
// 参数2：头信息
// 参数3：headerlen实际头长度,相对码流文件长度的偏移
//**************************************************************//
// static
int ReadPCMFileManager::getWaveHeader(char *input_buf,  int inputlen,
	WAVEDEC_HEADER* wave_header,  int *headerlen)
{
	char *header_tmp_buf;
	int ret;
	int pos;
	header_tmp_buf = input_buf;

	//read RIFF chunk
	ret = FindString("RIFF", 4, header_tmp_buf, inputlen, &pos);
	if (ret != 0)
	{
		printf("error read RIFF!\n");
		return -1;
	}
	memcpy(&(wave_header->riff_header), header_tmp_buf + pos, sizeof(RIFF_HEADER));

	//read fmt chunk
	ret = FindString("fmt", 3, header_tmp_buf, inputlen, &pos);
	if (ret != 0)
	{
		printf("error read fmt!\n");
		return -1;
	}
	memcpy(&(wave_header->fmt_block), header_tmp_buf + pos, sizeof(FMT_BLOCK));

	//read fact chunk
	ret = FindString("fact", 3, header_tmp_buf, inputlen, &pos);
	if (ret == 0)
	{
		memcpy(&(wave_header->fact_block), header_tmp_buf + pos, sizeof(FACT_BLOCK));
	}
	else
	{
		//fact chunk不是必须头文件信息
		//printf("no fact data!\n");
		wave_header->fact_block.szFactID[0] = -1;
	}

	//read data chunk
	ret = FindString("data", 4, header_tmp_buf, inputlen, &pos);
	if (ret != 0)
	{
		printf("error read data,wave hader buf NO data chunk!\n");
		wave_header->data_block.szDataID[0] = -1;
		return -1;
	}
	else
	{
		memcpy(&(wave_header->data_block), header_tmp_buf + pos, sizeof(DATA_BLOCK));
	}
	*headerlen = pos + sizeof(DATA_BLOCK);


	return 0;
}


// static
int ReadPCMFileManager::getWaveHeaderLen(char *input_buf, 
	int inputlen, 
	int *headerlen)
{
	int ret = 0;
	WAVEDEC_HEADER wave_header = { 0 };
	ret = getWaveHeader(input_buf, inputlen, &wave_header, headerlen);

	return ret;
}

#define HEADER_BUF_LEN 1024
int ReadPCMFileManager::getWaveHeaderOffset()
{
	if (NULL == pPCMFile)
	{
		return -1;
	}

	char InBuf[HEADER_BUF_LEN] = { 0 };
	int offset = 0;
	int ret = 0;
	int filecurrentset = ftell(pPCMFile);

	fseek(pPCMFile, 0, SEEK_SET);
	fread(InBuf, 1, HEADER_BUF_LEN, pPCMFile);
	fseek(pPCMFile, filecurrentset, SEEK_SET);

	WAVEDEC_HEADER wave_header = { 0 };
	ret = getWaveHeader(InBuf, HEADER_BUF_LEN, &wave_header, &offset);

	if (0 != wave_header.fmt_block.wavFormat.wChannels)
	{
		_FileFormat.wFormatTag = wave_header.fmt_block.wavFormat.wFormatTag;
		_FileFormat.wChannels = wave_header.fmt_block.wavFormat.wChannels;
		_FileFormat.dwSamplesPerSec = wave_header.fmt_block.wavFormat.dwSamplesPerSec;
		_FileFormat.dwAvgBytesPerSec = wave_header.fmt_block.wavFormat.dwAvgBytesPerSec;
		_FileFormat.wBlockAlign = wave_header.fmt_block.wavFormat.wBlockAlign;
		_FileFormat.wBitsPerSample = wave_header.fmt_block.wavFormat.wBitsPerSample;
		_isPcmHaveHeader = YES;
	}
	else
	{
		memset(&_FileFormat, 0, sizeof(_FileFormat));
		_isPcmHaveHeader = NO;
	}

	return offset;
}

int ReadPCMFileManager::getWaveFormat(int& isFileHaveHeader, WAVE_FORMAT& fileFormat)
{
	if (NULL == pPCMFile)
	{
		return -3;
	}

	isFileHaveHeader = _isPcmHaveHeader;

	if (YES == _isPcmHaveHeader)
	{

		if (0 == _FileFormat.wChannels) // 没有初始化
		{
			// 目前代码打开文件后调用 getWaveHeaderOffset() 即初始化，暂时不写这里
			printf("some serious error in getWaveFormat\n");
			return -2;
		}

		if ((int)&fileFormat != (int)&_FileFormat)
		{
			fileFormat.wFormatTag = _FileFormat.wFormatTag;
			fileFormat.wChannels = _FileFormat.wChannels;
			fileFormat.dwSamplesPerSec = _FileFormat.dwSamplesPerSec;
			fileFormat.dwAvgBytesPerSec = _FileFormat.dwAvgBytesPerSec;
			fileFormat.wBlockAlign = _FileFormat.wBlockAlign;
			fileFormat.wBitsPerSample = _FileFormat.wBitsPerSample;
			
		}

		return 0;
	}
	else
	{
		return -1;
	}
}



void ReadPCMFileManager::saveCurrentPath(CString filepath)
{
	_currentFileFullPath = filepath;

	int pos = filepath.ReverseFind('\\');
	_currentFilePath = filepath.Left(pos+1);

	_currentFileName = filepath.Right(filepath.GetLength() - pos -1);
}

void ReadPCMFileManager::cleanCurrentPath()
{
	_currentFileFullPath = _T("");
	_currentFilePath = _T("");
	_currentFileName = _T("");
}


//----------------------

/*-----------------------------------------------------*/
//  用于头解析，头chunk字符串查找
//
//
/*-----------------------------------------------------*/
int FindString(char *s, int s_len, char *d, int d_len, int *pos)
{
	int i = 0;
	int j = 0;
	char *temp_s;
	char *temp_d;
	temp_s = s;
	temp_d = d;

	while (d_len > i)
	{
		while (j < s_len)
		{
			if (*temp_s++ == *temp_d++)
			{
				j++;
				if (j == s_len)
				{
					*pos = i;
					return 0;
				}
			}
			else
			{
				break;
			}
		}

		j = 0;
		temp_s = s;
		d++;
		temp_d = d;
		i++;
	}
	return -1;
}
