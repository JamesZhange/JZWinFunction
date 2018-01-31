#include "stdafx.h"
#include "process.h"
#include "JZNamePipeClient.h"



JZNamePipeClient::JZNamePipeClient(JZNamePipeClientEventDelegate* eventDelegate, eumPipeVector steamVector): m_eventDelegate(eventDelegate),
																											 m_hPipe(INVALID_HANDLE_VALUE)
{
	isPipeConnected = NO;
	mServerPipeName = _T("");
	ifRunReceiveThread = NO;
	m_ReceiveThread = NULL;
	mSteamVector = steamVector;
	ReceiveThreadEndHandle = CreateEvent(NULL, TRUE, TRUE, NULL);
	CleanupThreadEndHandle = CreateEvent(NULL, TRUE, TRUE, NULL);
	// 
	m_CleanupPipeThread = new JZCSharpThread<JZNamePipeClient>(*this, &JZNamePipeClient::CleanupReceiveThreadPro);
	if (NULL == m_CleanupPipeThread)
	{
		DWORD errorcode = GetLastError();
		std::wcout << "Create client receive pipe thread faild:" << errorcode << std::endl;
		m_LastError = PipeClientError_CreateReceiveThreadFaild;
		m_LastErrorParam = errorcode;
		OnPipeClientError(m_LastError, m_LastErrorParam);
	}

	// �ݴ�
	if (NULL == pReadBuf)
	{
		pReadBuf = new BYTE[PIPE_DATA_BUF];
	}
	if (NULL != pReadBuf)
	{
		memset(pReadBuf, 0, PIPE_DATA_BUF);
		m_PipeState = PipeClientState_Init;
	}
	else
	{
		m_PipeState = PipeClientState_InitFaild;
	}
}

JZNamePipeClient::~JZNamePipeClient()
{
	DisconnectToServer();
	WaitForSingleObject(CleanupThreadEndHandle, INFINITE);

#if 1	// if 0 ������ڴ�й©�����̵߳�Stop�ẳס��ԭ����
	if (NULL != m_CleanupPipeThread)
	{
		delete m_CleanupPipeThread;
		m_CleanupPipeThread = NULL;
	}
#endif
}


int JZNamePipeClient::ConnectToPipeServer(std::wstring& sName, DWORD nTimeOut)
{
	if (sName.empty())
	{
		OnPipeClientError(PipeClientError_EmptyPipeName);
	}

	// clean up
	// TerminatePipe();

	// connect
	CString pipeName = _T("\\\\.\\pipe\\");
	CString pipePrivateName(sName.c_str());
	pipeName = pipeName + pipePrivateName;
	std::wstring FullPipeName = pipeName.GetString();

	//step1:���ܵ��Ƿ����
	if (!WaitNamedPipe(FullPipeName.c_str(), nTimeOut))  // ������ӣ���ʱ ����
	{
		// wprintf(_T("�ܵ��޷���"));
		return PipeClientError_TryConnectTimeOut;
	}

	//step2:���ӹܵ�
	m_hPipe = CreateFile(FullPipeName.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE != m_hPipe)
	{
		//�ɹ�
		printf("���ӹܵ��ɹ�\n");
		SetPipeClientState(PipeClientState_Connected);
		isPipeConnected = YES;
		mServerPipeName = pipePrivateName;

		if (PipeVector_Receiver == mSteamVector)
		{
			StartReceiveThread();
		}

		return 0;
	}
	else
	{
		DWORD errCode = GetLastError();
		OnPipeClientError(PipeClientError_ConnectFaild, errCode);
		return -1;
	}
}


int JZNamePipeClient::StartReceiveThread()
{
	// create
	ifRunReceiveThread = YES;
	m_ReceiveThread = new JZCSharpThread<JZNamePipeClient>(*this, &JZNamePipeClient::ReceiveThreadPro);
	if (NULL == m_ReceiveThread)
	{
		DWORD errorcode = GetLastError();
		std::wcout << "Create client receive pipe thread2 faild:" << errorcode << std::endl;
		m_LastError = PipeClientError_CreateReceiveThreadFaild;
		m_LastErrorParam = errorcode;
		OnPipeClientError(m_LastError, m_LastErrorParam);
		return -2;
	}

	// start
	m_ReceiveThread->Start(NULL);

	return 0;
}
DWORD JZNamePipeClient::ReceiveThreadPro()
{
	printf("--In Receive Thread Pro\n");
	ResetEvent(ReceiveThreadEndHandle);
	m_CleanupPipeThread->Start(NULL);

	DWORD ret = 0;
	while (ifRunReceiveThread)
	{
		if (INVALID_HANDLE_VALUE != m_hPipe)
		{
			DWORD dwReadLen = 0;
			DWORD dwRet = ReadPipeData(m_hPipe, pReadBuf, &dwReadLen);
			if (0 == dwRet)
			{
				if (NULL != m_eventDelegate)
				{
					m_eventDelegate->NamedPipeClientReceiveData(*this, pReadBuf, dwReadLen);
				}
				// Sleep(5);
			}
			else // some error
			{
				if (ERROR_BROKEN_PIPE != dwRet)
				{
					std::wcout << "ERROR_BROKEN_PIPE when server read data" << std::endl;
					OnPipeClientError(PipeClientError_ReadBrokenPipe, dwRet);
					ret = -4;
					break;
				}
				OnPipeClientError(PipeClientError_ReadBuffFaild);
				ifRunReceiveThread = NO;
				ret = -5;
				break;
			}
		}
		else
		{
			OnPipeClientError(PipeClientError_ThreadExitUnexpected);
			ret = -1;
			break;
		}
	}
	SetEvent(ReceiveThreadEndHandle);

	printf("--Out Receive Thread Pro\n");
	return ret;
}


DWORD JZNamePipeClient::CleanupReceiveThreadPro()
{
	ResetEvent(CleanupThreadEndHandle);
	printf("--In CleanupReceiveThreadPro\n");

	WaitForSingleObject(ReceiveThreadEndHandle, INFINITE);
	if (NULL != m_ReceiveThread)
	{
		delete m_ReceiveThread;
		m_ReceiveThread = NULL;
	}
	CleanupPipe();

	printf("--Out CleanupReceiveThreadPro\n");
	SetEvent(CleanupThreadEndHandle);
	return 0;
}



int JZNamePipeClient::DisconnectToServer()
{
	ifRunReceiveThread = NO;
	return 0;
}


DWORD JZNamePipeClient::CleanupPipe()
{
	if (INVALID_HANDLE_VALUE != m_hPipe)
	{
		CancelSynchronousIo(m_hPipe);
		FlushFileBuffers(m_hPipe);
		DisconnectNamedPipe(m_hPipe);
		CloseHandle(m_hPipe);
		SetPipeClientState(PipeClientState_Disconnect);

		m_hPipe = INVALID_HANDLE_VALUE;
	}
	isPipeConnected = NO;
	mServerPipeName = _T("");
	return 0;
}






void JZNamePipeClient::SendData(BYTE* pbData, DWORD dwDataLen)
{
	if (INVALID_HANDLE_VALUE != m_hPipe)
	{
		DWORD ret = WritePipeData(m_hPipe, pbData, dwDataLen);
		if (0 != ret)
		{
			std::wcout << "WritePipeData err:" << ret << std::endl;
			OnPipeClientError(PipeClientError_WriteDataFaild);
		}
	}
}


void JZNamePipeClient::SetPipeClientState(eumPipeClientState state)
{
	m_PipeState = state;
	if (NULL != m_eventDelegate)
	{
		m_eventDelegate->NamedPipeClientStateChange(*this, m_PipeState);
	}
}


void JZNamePipeClient::OnPipeClientError(int err, DWORD paramL)
{
	if (NULL != m_eventDelegate)
	{
		m_eventDelegate->NamedPipeClientError(*this, (eumPipeClientError)err, paramL);
	}
}






/***************************************************************/
/*********     JZNamePipeClientEventDelegate      **************/
/***************************************************************/

void JZNamePipeClientEventDelegate::NamedPipeClientStateChange(JZNamePipeClient& client, eumPipeClientState stateCode)
{

}
void JZNamePipeClientEventDelegate::NamedPipeClientReceiveData(JZNamePipeClient & client, BYTE * pbData, DWORD pdwDataLen)
{
}
void JZNamePipeClientEventDelegate::NamedPipeClientError(JZNamePipeClient & client, eumPipeClientError errCode, DWORD paramL)
{
}
