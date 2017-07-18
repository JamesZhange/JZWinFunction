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
Purpose :  д��ܵ�����
Input   :  hPipe           --  [IN] �ܵ����
		   pbData          --  [IN] д�������
		   dwDataLen       --  [IN] д�����ݳ���
Return  :  0 -- �ɹ� ���� -- ʧ�ܣ����ض�Ӧ�Ĵ�����
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
Purpose :  ��ȡ�ܵ�����
Input   :  hPipe           --  [IN] �ܵ����
		   pbData          --  [OUT] ����������
		   pdwDataLen      --  [OUT] �������ݳ���
Return  :  0 -- �ɹ� ���� -- ʧ�ܣ����ض�Ӧ�Ĵ�����
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

	//�ȶ�ȡ����
	bRet = ReadFile(hPipe, bTemp, sizeof(DWORD), &dwLen, NULL);
	if (!bRet || dwLen == 0)
	{
		dwRet = GetLastError();
		if (dwRet == ERROR_BROKEN_PIPE)
		{
			printf("�ͻ����ѹر�����\n");
			return dwRet;
		}
		else
		{
			printf("��ȡ�ͻ�������ʧ��,[%#x]\n", dwRet);
			return dwRet;
		}
	}
	if (dwLen != sizeof(DWORD))
	{
		printf("�������ݳ��Ȳ���ȷ��recv len[%d]\n", dwLen);
		return 1;
	}

	memcpy(&dwTempLen, bTemp, sizeof(DWORD));

	//��ȡ����
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
				printf("�ͻ����ѹر�����\n");
				return dwRet;
			}
			else
			{
				printf("��ȡ����ʧ��,[%#x]\n", dwRet);
				return dwRet;
			}
		}
		dwTempLen -= dwLen;
		p += dwLen;
		*pdwDataLen += dwLen;
	}

	return 0;
}
