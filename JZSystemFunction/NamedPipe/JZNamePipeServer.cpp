#include "stdafx.h"
#include "process.h"
#include "JZNamePipeServer.h"



JZNamePipeServer::JZNamePipeServer(std::wstring& sName, 
								   JZNamePipeServerEventDelegate* eventDelegate,
								   eumPipeVector steamVector/* = PipeVector_Receiver*/): m_ServerThread(NULL),
																						 m_eventDelegate(eventDelegate),
																						 m_hPipe(NULL)
{
	CString pipePrivateName(sName.c_str());
	CString pipeName;
	pipeName.Format(_T("\\\\.\\pipe\\%s"), pipePrivateName);
	m_sPipeName = pipeName.GetString();

	isServerThreadRunning = NO;

	for (int i=0; i< PipeStateHandle_TotalNum; i++)
	{
		StateEvents[i] = NULL;
	}

	mSteamVector = steamVector;

	// �ݴ�
	if (NULL == pReadBuf)
	{
		pReadBuf = new BYTE[PIPE_DATA_BUF];
	}

	if (NULL != pReadBuf)
	{
		memset(pReadBuf, 0, PIPE_DATA_BUF);
		m_PipeState = PipeServerState_Init;
		StartServerThread();
	}
	else
	{
		m_PipeState = PipeServerState_InitFaild;
	}
}

JZNamePipeServer::~JZNamePipeServer()
{
	isServerThreadRunning = NO;

	// ֹͣ�ź�
	if (NULL != StateEvents[PipeStateHandle_ForceExit])
	{
		SetEvent(StateEvents[PipeStateHandle_ForceExit]);
		WaitForSingleObject(StateEvents[PipeStateHandle_ForceExitEnd], INFINITE);
	}

	if (NULL != m_ServerThread)
	{
		delete m_ServerThread;
		m_ServerThread = NULL;
	}

	CleanupPipe();
}




int JZNamePipeServer::StartServerThread()
{
	isServerThreadRunning = YES;
	m_ServerThread = new JZCSharpThread<JZNamePipeServer>(*this, &JZNamePipeServer::ServerThreadPro);
	if (NULL == m_ServerThread)
	{
		DWORD errorcode = GetLastError();
		std::wcout << "Create server pipe thread faild:" << errorcode << std::endl;
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
	while (YES == isServerThreadRunning)
	{
		Sleep(100);
		createNewPipeAndRun();
	}
	if (NULL != StateEvents[PipeStateHandle_ForceExitEnd])
	{
		SetEvent(StateEvents[PipeStateHandle_ForceExitEnd]);
	}
	
	return 0;
}

int JZNamePipeServer::createNewPipeAndRun()
{
	if (NULL != m_hPipe)
	{
		CleanupPipe();
	}

	if (m_sPipeName.empty())
	{
		std::wcout << "Error: Invalid pipe name" << std::endl;
		OnPipeServerError(PipeServerError_EmptyPipeName);
		return -3;
	}

	StateEvents[PipeStateHandle_Overlapped] = CreateEvent(NULL, TRUE, TRUE, NULL);   // OVERLPPED��s event
	StateEvents[PipeStateHandle_ForceExit] = CreateEvent(NULL, TRUE, FALSE, NULL);  // exit event
	StateEvents[PipeStateHandle_ForceExitEnd] = CreateEvent(NULL, TRUE, FALSE, NULL);  // exit event
	memset(&WorkOP, 0, sizeof(WorkOP));
	WorkOP.hEvent = StateEvents[PipeStateHandle_Overlapped];


	//-------------- step1:�����ܵ�ʵ�� ----------------
	m_hPipe = CreateNamedPipe(m_sPipeName.c_str(),			// pipe name
		PIPE_ACCESS_DUPLEX |								// read/write access 
		FILE_FLAG_OVERLAPPED |								// overlapped mode 
		FILE_FLAG_WRITE_THROUGH,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | 
		PIPE_WAIT,											// blocking mode
		1,													// number of instances "PIPE_UNLIMITED_INSTANCES", 
		0, 0, 0, NULL);
	if (INVALID_HANDLE_VALUE == m_hPipe)
	{
		DWORD errorcode = GetLastError();
		std::wcout << "CreateNamedPipeA err"<< errorcode << std::endl;
		OnPipeServerError(PipeServerError_CreatePipeFaild, errorcode);
		return -1;
	}
	SetPipeServerState(PipeServerState_Init);



	//---------------- step2:�ȴ��ͻ������� ----------------
	SetPipeServerState(PipeServerState_WaitForClient);

	BOOL fConnected, fPendingIO = FALSE;
	fConnected = ConnectNamedPipe(m_hPipe, &WorkOP);	// Wait for client to get connected������ WorkOP ��� FILE_FLAG_OVERLAPPED ʹ�����api��Ҫ����
	if (fConnected) {
		DWORD errorcode = GetLastError();
		std::wcout << "Error connecting to pipe client:" << errorcode << std::endl;
		OnPipeServerError(PipeServerError_WaitConnectFaild, errorcode);
		return -2;
	}

	// �жϣ����������� pending����״̬���գ��ȴ������ź�ȷ��������connected���������ӣ�����״̬Ϊ����
	switch (GetLastError()) {
	case ERROR_IO_PENDING:	// The overlapped connection in progress. 
		fPendingIO = TRUE;
		break;
	case ERROR_PIPE_CONNECTED:	// Client is already connected, so signal an event. 
		SetPipeServerState(PipeServerState_Connected);
		if (SetEvent(WorkOP.hEvent))
			break;
	default:// If an error occurs during the connect operation... 
	{
		DWORD errorcode = GetLastError();
		std::wcout << "Error connecting to pipe client:" << errorcode << std::endl;
		OnPipeServerError(PipeServerError_WaitConnectFaild, errorcode);
		return -2;
	}
	}


	BOOL fSuccess;
	DWORD dwRet;
	DWORD dwResult = WaitForMultipleObjects(2, StateEvents, FALSE, INFINITE);  // �����Ϳ���ͬʱ�ȴ����ӵ��źź������˳����ź�
	if (PipeStateHandle_Overlapped == (dwResult - WAIT_OBJECT_0) ) {
		if (TRUE == fPendingIO)
		{
			fSuccess = GetOverlappedResult(m_hPipe, &WorkOP, &dwRet, FALSE);		// do not wait 
			if (fSuccess)
			{
				// ���ӳɹ�
				SetPipeServerState(PipeServerState_Connected);
			}
		}
		else
		{
			// ����ʧ�ܣ����������˳�
			DWORD errorcode = GetLastError();
			std::wcout << "Error connecting to pipe client:" << errorcode << std::endl;
			OnPipeServerError(PipeServerError_WaitConnectFaild, errorcode);
			return -2;
		}
	}
	else if (PipeStateHandle_ForceExit == (dwResult - WAIT_OBJECT_0)) // �����˳�
	{
		return -2;
	}

	// ---------------- step3:��ѯ��ȡ ----------------

	if (PipeVector_Receiver == mSteamVector)  // server ��������
	{
		while (1)
		{
			if (NULL != m_hPipe)
			{
				DWORD dwReadLen = 0;
				DWORD dwRet = ReadPipeData(m_hPipe, pReadBuf, &dwReadLen);
				if (0 == dwRet)
				{
					if (NULL != m_eventDelegate)
					{
						m_eventDelegate->NamedPipeServerReceiveData(*this, pReadBuf, dwReadLen);
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
				OnPipeServerError(PipeServerError_ThreadExitUnexpected);
				return -1;
			}
		}
	}
	else  // server �������ݡ�ά�����ӣ��Ͽ�������server
	{
		WaitForSingleObject(StateEvents[PipeStateHandle_ForceExit], INFINITE);
	}

	return 0;
}


#if 0
void JZNamePipeServer::TerminatePipe()
{
	if (NULL != m_hPipe)
	{
		isServerThreadRunning = NO;
		// ֹͣ�ź�
		if (NULL != StateEvents[PipeStateHandle_ForceExit])
		{
			SetEvent(StateEvents[PipeStateHandle_ForceExit]);
			WaitForSingleObject(StateEvents[PipeStateHandle_ForceExitEnd], INFINITE);
		}
		
		CleanupPipe();
	}
}
#endif

void JZNamePipeServer::CleanupPipe()
{
	if (NULL != m_hPipe)
	{
		CancelSynchronousIo(m_hPipe);
		FlushFileBuffers(m_hPipe);
		DisconnectNamedPipe(m_hPipe);
		SetPipeServerState(PipeServerState_DisconnectClient);

		CloseHandle(m_hPipe);
		m_hPipe = NULL;
		SetPipeServerState(PipeServerState_Terminate);

		for (int i = 0; i < PipeStateHandle_TotalNum; i++)
		{
			if (NULL != StateEvents[i])
			{
				CloseHandle(StateEvents[i]);
				StateEvents[i] = NULL;
			}
		}
	}
}




void JZNamePipeServer::SendData(BYTE* pbData, DWORD dwDataLen)
{
	if (NULL != m_hPipe)
	{
		DWORD ret = WritePipeData(m_hPipe, pbData, dwDataLen);
		if (0 != ret)
		{
			std::wcout << "WritePipeData err:" << ret << std::endl;
			OnPipeServerError(PipeServerError_WriteDataFaild);
			DisconnectClient();
		}
	}
}


int JZNamePipeServer::DisconnectClient()
{
	if (PipeVector_Receiver == mSteamVector)
	{
		CleanupPipe();
	}
	else
	{
		SetEvent(StateEvents[PipeStateHandle_ForceExit]);
	}
	return 0;
}


void JZNamePipeServer::SetPipeServerState(eumPipeServerState state)
{
	m_PipeState = state;
	if (NULL != m_eventDelegate)
	{
		m_eventDelegate->NamedPipeServerStateChange(*this, m_PipeState);
	}
}


void JZNamePipeServer::OnPipeServerError(int err, DWORD paramL)
{
	if (NULL != m_eventDelegate)
	{
		m_eventDelegate->NamedPipeServerError(*this, (eumPipeServerError)err, paramL);
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
