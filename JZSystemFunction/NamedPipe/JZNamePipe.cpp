#include "stdafx.h"
#include "JZNamePipe.h"


JZNamePipe::JZNamePipe()
{
	tag = 0;
}


JZNamePipe::~JZNamePipe()
{
}


/*************************************************************************
Purpose :  写入管道数据
Input   :  hPipe           --  [IN] 管道句柄
		   pbData          --  [IN] 写入的数据
		   dwDataLen       --  [IN] 写入数据长度
Return  :  0 -- 成功 其他 -- 失败，返回对应的错误码
Modify  :
Remark  :
*************************************************************************/
DWORD JZNamePipe::WritePipeData(HANDLE hPipe, BYTE* pbData, DWORD dwDataLen)
{
	BOOL        bRet;
	DWORD       dwRet;
	BYTE        bTemp[PIPE_DATA_BUF] = { 0 };
	DWORD       dwTempLen = 0;
	BYTE*       p = NULL;
	DWORD       dwNumWrite = 0;
	DWORD       dwTotleLen = 0;

	p = bTemp;
	memcpy(p, &dwDataLen, sizeof(DWORD));
	p += sizeof(DWORD);
	memcpy(p, pbData, dwDataLen);

	dwTotleLen = dwDataLen + sizeof(DWORD);
	p = bTemp;

	while (dwTotleLen)
	{
		bRet = WriteFile(hPipe, p, dwTotleLen, &dwNumWrite, NULL);
		if (!bRet || dwNumWrite == 0)
		{
			dwRet = GetLastError();
			printf("WriteFile err[%#x]\n", dwRet);
			return dwRet;
		}

		dwTotleLen -= dwNumWrite;
		p += dwNumWrite;
	}

	return 0;
}


/*************************************************************************
Purpose :  读取管道数据
Input   :  hPipe           --  [IN] 管道句柄
		   pbData          --  [OUT] 读出的数据
		   pdwDataLen      --  [OUT] 读出数据长度
Return  :  0 -- 成功 其他 -- 失败，返回对应的错误码
Modify  :
Remark  :
*************************************************************************/
DWORD JZNamePipe::ReadPipeData(HANDLE hPipe, BYTE* pbData, DWORD* pdwDataLen)
{
	BOOL    bRet;
	DWORD   dwRet;
	BYTE    bTemp[PIPE_DATA_BUF] = { 0 };
	DWORD   dwLen = 0;
	DWORD   dwTempLen = 0;
	BYTE*   p = NULL;

	//先读取长度
	bRet = ReadFile(hPipe, bTemp, sizeof(DWORD), &dwLen, NULL);
	if (!bRet || dwLen == 0)
	{
		dwRet = GetLastError();
		if (dwRet == ERROR_BROKEN_PIPE)
		{
			printf("客户端已关闭链接\n");
			return dwRet;
		}
		else
		{
			printf("读取客户端数据失败,[%#x]\n", dwRet);
			return dwRet;
		}
	}
	if (dwLen != sizeof(DWORD))
	{
		printf("接收数据长度不正确，recv len[%d]\n", dwLen);
		return 1;
	}

	memcpy(&dwTempLen, bTemp, sizeof(DWORD));

	//读取数据
	p = pbData;
	dwLen = 0;
	*pdwDataLen = 0;
	while (dwTempLen)
	{
		bRet = ReadFile(hPipe, p + dwLen, dwTempLen, &dwLen, NULL);
		if (!bRet || dwLen == 0)
		{
			dwRet = GetLastError();
			if (dwRet == ERROR_BROKEN_PIPE)
			{
				printf("客户端已关闭链接\n");
				return dwRet;
			}
			else
			{
				printf("读取数据失败,[%#x]\n", dwRet);
				return dwRet;
			}
		}
		dwTempLen -= dwLen;
		p += dwLen;
		*pdwDataLen += dwLen;
	}

	return 0;
}
