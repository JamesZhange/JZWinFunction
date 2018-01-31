/**
* ��װ�����ܵ�
* [2017-06-29] James.Zhang
*/

#pragma once

#include <string>
#include <iostream>
#include "windows.h"


#define PIPE_DATA_BUF     (50*1024)

/*
* ����ֻд���������յĹ���������������Ϊ����
* ��Ȼ�����㵫Ч�ʸ�һ�㣬��Ҫ�ٳ�ʼ��ʱȷ�����ݵĽ��պͷ��ͷ�
*/
typedef enum 
{
	PipeVector_Sender,
	PipeVector_Receiver,
}eumPipeVector;


typedef enum _jznamedpipeerrorcode
{
	PipeError_MemoryAllocFaild = -100,

	PipeError_NoErr = 0x0,
	
}eumPipeError;




class JZNamePipe
{
public:
	JZNamePipe();
	~JZNamePipe();
	
	int tag;

protected:
	eumPipeVector mSteamVector;
	BYTE *pReadBuf;

	BYTE* pWriteBuf;
	DWORD WritePipeData(HANDLE hPipe, BYTE* pbData, DWORD dwDataLen);
	DWORD ReadPipeData(HANDLE hPipe, BYTE* pbData, DWORD* pdwDataLen);

	// 
	int m_LastError;
	DWORD m_LastErrorParam;
};

