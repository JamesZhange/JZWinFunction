#include "stdafx.h"
#include "process.h"
#include "JZNamePipeClient.h"



JZNamePipeClient::JZNamePipeClient(std::wstring& sName,
								   JZNamePipeClientEventDelegate* eventDelegate): m_sPipeName(sName),
																				  m_eventDelegate(eventDelegate),
																				  m_hPipe(NULL)
{
	memset(&bWritebuf, 0, PIPE_DATA_BUF);
	m_PipeState = PipeClientState_Init;
	isPipeConnected = NO;
}

JZNamePipeClient::~JZNamePipeClient()
{
	TerminatePipe();
}


int JZNamePipeClient::ConnectToPipeServer(DWORD nTimeOut)
{
	if (m_sPipeName.empty())
	{
		OnPipeClientError(PipeClientError_EmptyPipeName);
	}

	TerminatePipe();

	//step1:���ܵ��Ƿ����
	if (!WaitNamedPipe(m_sPipeName.c_str(), nTimeOut))  // ������ӣ���ʱ ����
	{
		// wprintf(_T("�ܵ��޷���"));
		return PipeClientError_TryConnectTimeOut;
	}

	//step2:���ӹܵ�
	m_hPipe = CreateFile(m_sPipeName.c_str(),
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
		// int ret = RunPipe();
		// return ret;
		return 0;
	}
	else
	{
		DWORD errCode = GetLastError();
		OnPipeClientError(PipeClientError_ConnectFaild, errCode);
		return -1;
	}
}

int JZNamePipeClient::DisconnectToServer()
{
	TerminatePipe();
	return 0;
}


DWORD JZNamePipeClient::TerminatePipe()
{
	if (NULL != m_hPipe)
	{
		CloseHandle(m_hPipe);
		SetPipeClientState(PipeClientState_Disconnect);

		m_hPipe = NULL;
	}
	isPipeConnected = NO;
	return 0;
}


void JZNamePipeClient::SendData(BYTE* pbData, DWORD dwDataLen)
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


void JZNamePipeClient::SetPipeClientState(eumPipeClientState state)
{
	m_PipeState = state;
	if (NULL != m_eventDelegate)
	{
		m_eventDelegate->NamedPipeClientStateChange(*this, m_PipeState);
	}
}


void JZNamePipeClient::OnPipeClientError(eumPipeClientError err, DWORD paramL)
{
	if (NULL != m_eventDelegate)
	{
		m_eventDelegate->NamedPipeClientError(*this, err, paramL);
	}
}






/***************************************************************/
/*********     JZNamePipeClientEventDelegate      **************/
/***************************************************************/

void JZNamePipeClientEventDelegate::NamedPipeClientStateChange(JZNamePipeClient& client, eumPipeClientState stateCode)
{

}

void JZNamePipeClientEventDelegate::NamedPipeClientReceiveData(JZNamePipeClient& client, BYTE* pbData, DWORD pdwDataLen)
{

}

void JZNamePipeClientEventDelegate::NamedPipeClientError(JZNamePipeClient & client, eumPipeClientError errCode, DWORD paramL)
{
}
