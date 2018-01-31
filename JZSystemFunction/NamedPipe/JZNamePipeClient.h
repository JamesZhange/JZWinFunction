#pragma once
#include "JZNamePipe.h"
#include "JZGlobal.h"
#include "JZThread.h"

class JZNamePipeClientEventDelegate;


typedef enum _jznamedpipeclientrunningstate
{
	PipeClientState_Init = 0x100,
	PipeClientState_InitFaild,
	PipeClientState_Connected,
	PipeClientState_Disconnect,
	// PipeClientState_Terminate,
}eumPipeClientState;

typedef enum _jznamedpipeclienterrorcode
{
	PipeClientError_NoErr = PipeError_NoErr,

	PipeClientError_MemoryAllocFaild = PipeError_MemoryAllocFaild,

	PipeClientError_MildErr = 0x200,
	PipeClientError_TryConnectTimeOut,
	PipeClientError_ConnectFaild,
	PipeClientError_ReadBuffFaild,
	PipeClientError_WriteDataFaild,

	PipeClientError_Serious = 0x1000,
	PipeClientError_EmptyPipeName,
	PipeClientError_CreateReceiveThreadFaild,
	PipeClientError_CreateCleanupThreadFaild,
	PipeClientError_ReadBrokenPipe,
	PipeClientError_ThreadExitUnexpected,

}eumPipeClientError;




class JZNamePipeClient: public JZNamePipe
{
public:
	JZNamePipeClient(JZNamePipeClientEventDelegate* eventDelegate, eumPipeVector steamVector = PipeVector_Sender);
	~JZNamePipeClient();

	// int ConnectToPipeServer(DWORD nTimeOut); // nTimeOut �ȴ���ʱ������
	int ConnectToPipeServer(std::wstring& sName, DWORD nTimeOut);
	void SendData(BYTE* pbData, DWORD dwDataLen);
	int DisconnectToServer();

	GETPROP(eumPipeClientState, PipeState);
	GET(eumPipeClientState, PipeState) {
		return m_PipeState;
	}

	GETPROP(CString, ServerName);
	GET(CString, ServerName) {
		return mServerPipeName;
	}

private:

	// std::wstring* m_pPipeName;		// Pipe name
	HANDLE m_hPipe;                 // Pipe handle
	// HANDLE m_hThread;            // Pipe thread
	JZNamePipeClientEventDelegate* m_eventDelegate;

	int isPipeConnected;

	// ����ѭ��
	int StartReceiveThread();
	BOOL ifRunReceiveThread;
	JZCSharpThread<JZNamePipeClient> *m_ReceiveThread;
	DWORD ReceiveThreadPro();

	// ״̬�ź�
	JZCSharpThread<JZNamePipeClient> *m_CleanupPipeThread; // �������̳߳�����Ҫ�˳�ʱ���������߳�������pipe����Ҫ���ⲿ����
	DWORD CleanupReceiveThreadPro();
	HANDLE ReceiveThreadEndHandle; // ��ʶ�����߳̽���
	HANDLE CleanupThreadEndHandle; // ��ʶ�����߳̽���

	//
	DWORD CleanupPipe();

	void OnPipeClientError(int err, DWORD paramL = 0);

	eumPipeClientState m_PipeState;
	void SetPipeClientState(eumPipeClientState state);

	CString mServerPipeName;
};




/***************************************************************/
/*********     JZNamePipeClientEventDelegate      **************/
/***************************************************************/



class JZNamePipeClientEventDelegate
{
public:
	virtual void NamedPipeClientStateChange(JZNamePipeClient& client, eumPipeClientState stateCode);
	virtual void NamedPipeClientReceiveData(JZNamePipeClient& client, BYTE* pbData, DWORD pdwDataLen);
	virtual void NamedPipeClientError(JZNamePipeClient& client, eumPipeClientError errCode, DWORD paramL=0);
};


