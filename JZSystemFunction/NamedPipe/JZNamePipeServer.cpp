#include "stdafx.h"
#include "process.h"
#include "JZNamePipeServer.h"



JZNamePipeServer::JZNamePipeServer(std::wstring& sName, 
								   JZNamePipeServerEventDelegate* eventDelegate): m_ServerThread(NULL),
																				  m_eventDelegate(eventDelegate),
																				  m_hPipe(NULL),
																				  isPipeThreadReading(0)
{
	CString pipePrivateName(sName.c_str());
	CString pipeName;
	pipeName.Format(_T("\\\\.\\pipe\\%s"), pipePrivateName);
	m_sPipeName = pipeName.GetString();

	memset(&bReadBuf, 0, PIPE_DATA_BUF);
	m_PipeState = PipeServerState_Init;
	isServerThreadRunning = NO;
	StartServerThread();
}

JZNamePipeServer::~JZNamePipeServer()
{
	isServerThreadRunning = NO;
	TerminatePipe();

	if (NULL != m_ServerThread)
	{
		m_ServerThread->Stop();
		delete m_ServerThread;
		m_ServerThread = NULL;
	}
}




int JZNamePipeServer::StartServerThread()
{
	m_ServerThread = new JZCSharpThread<JZNamePipeServer>(*this, &JZNamePipeServer::ServerThreadPro);
	if (NULL == m_ServerThread)
	{
		DWORD errorcode = GetLastError();
		std::wcout << "Create pipe thread faild:" << errorcode << std::endl;
		m_LastError = PipeServerError_CreateServerDaemonThreadFaild;
		m_LastErrorParam = errorcode;
		OnPipeServerError(m_LastError, m_LastErrorParam);
		return -2;
	}
	m_ServerThread->Start(NULL);
	return 0;
}
DWORD JZNamePipeServer::ServerThreadPro()
{
	isServerThreadRunning = YES;
	while (YES == isServerThreadRunning)
	{
		Sleep(100);
		createNewPipeAndRun();
	}
	isServerThreadRunning = NO;
	return 0;
}

int JZNamePipeServer::createNewPipeAndRun()
{
	if (NULL != m_hPipe)
	{
		TerminatePipe();
	}

	if (m_sPipeName.empty())
	{
		std::wcout << "Error: Invalid pipe name" << std::endl;
		OnPipeServerError(PipeServerError_EmptyPipeName);
		return -3;
	}

	//step1:创建管道实例
	m_hPipe = CreateNamedPipe(m_sPipeName.c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 0, 0, 0, NULL);
	if (INVALID_HANDLE_VALUE == m_hPipe)
	{
		DWORD errorcode = GetLastError();
		std::wcout << "CreateNamedPipeA err"<< errorcode << std::endl;
		OnPipeServerError(PipeServerError_CreatePipeFaild, errorcode);
		return -1;
	}
	SetPipeServerState(PipeServerState_Init);

	//step2:等待客户端连接

	SetPipeServerState(PipeServerState_WaitForClient);

	if (FALSE == ConnectNamedPipe(m_hPipe, NULL)) // Wait for client to get connected
	{
		DWORD errorcode = GetLastError();
		std::wcout << "Error connecting to pipe client:" << errorcode << std::endl;
		OnPipeServerError(PipeServerError_WaitConnectFaild, errorcode);
		return -2;
	}

	SetPipeServerState(PipeServerState_Connected);

	// 轮询读取
	isPipeThreadReading = 1;
	while (isPipeThreadReading)
	{
		if (NULL != m_hPipe)
		{
			DWORD dwReadLen = 0;
			DWORD dwRet = ReadPipeData(m_hPipe, bReadBuf, &dwReadLen);
			if (0 == dwRet)
			{
				if (NULL != m_eventDelegate)
				{
					m_eventDelegate->NamedPipeServerReceiveData(*this, bReadBuf, dwReadLen);
				}
				Sleep(5);
			}
			else // some error
			{
				if (ERROR_BROKEN_PIPE != dwRet)
				{
					std::wcout << "ERROR_BROKEN_PIPE when server read data" << std::endl;
					OnPipeServerError(PipeServerError_ReadBrokenPipe, dwRet);
					return -4;
				}
				OnPipeServerError(PipeServerError_ReadBuffFaild);
				
				return -5;
			}
		}
		else
		{
			isPipeThreadReading = 0;
			OnPipeServerError(PipeServerError_ThreadExitUnexpected);
		}
	}

	return 0;
}


void JZNamePipeServer::TerminatePipe()
{
	if (NULL != m_hPipe)
	{
		FlushFileBuffers(m_hPipe);
		DisconnectNamedPipe(m_hPipe);
		SetPipeServerState(PipeServerState_DisconnectClient);
		
		CloseHandle(m_hPipe);
		SetPipeServerState(PipeServerState_Terminate);

		m_hPipe = NULL;
	}
}


#if 0
void JZNamePipeServer::SendData(BYTE* pbData, DWORD dwDataLen)
{
	if (NULL != m_hPipe)
	{
		DWORD ret = WritePipeData(m_hPipe, pbData, dwDataLen);
		if (0 != ret)
		{
			std::wcout << "WritePipeData err:" << ret << std::endl;
		}
	}
}
#endif



void JZNamePipeServer::SetPipeServerState(eumPipeServerState state)
{
	m_PipeState = state;
	if (NULL != m_eventDelegate)
	{
		m_eventDelegate->NamedPipeServerStateChange(*this, m_PipeState);
	}
}


void JZNamePipeServer::OnPipeServerError(eumPipeServerError err, DWORD paramL)
{
	if (NULL != m_eventDelegate)
	{
		m_eventDelegate->NamedPipeServerError(*this, err, paramL);
	}
}





/***************************************************************/
/*********     JZNamePipeServerEventDelegate      **************/
/***************************************************************/

void JZNamePipeServerEventDelegate::NamedPipeServerStateChange(JZNamePipeServer& server, eumPipeServerState stateCode)
{

}

void JZNamePipeServerEventDelegate::NamedPipeServerReceiveData(JZNamePipeServer& server, BYTE* pbData, DWORD pdwDataLen)
{

}

void JZNamePipeServerEventDelegate::NamedPipeServerError(JZNamePipeServer & server, eumPipeServerError errCode, DWORD paramL)
{
}
