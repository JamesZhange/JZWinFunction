#pragma once
#include "JZNamePipe.h"
#include "JZGlobal.h"
#include "JZThread.h"

class JZNamePipeServerEventDelegate;


typedef enum _jznamedpipeserverrunningstate
{
	PipeServerState_Init = 0x100,
	PipeServerState_WaitForClient,
	PipeServerState_Connected,
	PipeServerState_DisconnectClient,
	PipeServerState_Terminate,
}eumPipeServerState;

typedef enum _jznamedpipeservererrorcode
{
	PipeServerError_NoErr = 0x0,

	PipeServerError_MildErr = 0x200,
	PipeServerError_WaitConnectFaild,
	PipeServerError_ReadBuffFaild,

	PipeServerError_Serious = 0x1000,
	PipeServerError_EmptyPipeName,
	PipeServerError_CreateServerDaemonThreadFaild,
	PipeServerError_CreatePipeFaild,
	PipeServerError_CreateThreadFaild,
	PipeServerError_ReadBrokenPipe,
	PipeServerError_ThreadExitUnexpected,

}eumPipeServerError;



class JZNamePipeServer: JZNamePipe
{
public:
	JZNamePipeServer(std::wstring& sName, JZNamePipeServerEventDelegate* eventDelegate);
	~JZNamePipeServer();

	// void SendData(BYTE* pbData, DWORD dwDataLen);
	// void DisconnectClient();

	GETPROP(eumPipeServerState, PipeState);
	GET(eumPipeServerState, PipeState) {
		return m_PipeState;
	}

private:
	BYTE bReadBuf[PIPE_DATA_BUF];
	// BYTE bWritebuf[PIPE_DATA_BUF];

	const std::wstring m_sPipeName; // Pipe name
	HANDLE m_hPipe;                 // Pipe handle
	// HANDLE m_hThread;               // Pipe thread
	JZNamePipeServerEventDelegate* m_eventDelegate;


	DWORD ServerThreadPro();
	JZCSharpThread<JZNamePipeServer> *m_ServerThread;
	int StartServerThread();
	int isServerThreadRunning;

	int createNewPipeAndRun();
	int isPipeThreadReading;

	void TerminatePipe();

	eumPipeServerError m_LastError; // 用线程来检查错误并控制退出其他线程
	DWORD m_LastErrorParam;
	
	void OnPipeServerError(eumPipeServerError err, DWORD paramL = 0);

	eumPipeServerState m_PipeState;
	void SetPipeServerState(eumPipeServerState state);

};




/***************************************************************/
/*********     JZNamePipeServerEventDelegate      **************/
/***************************************************************/



class JZNamePipeServerEventDelegate
{
public:
	virtual void NamedPipeServerStateChange(JZNamePipeServer& server, eumPipeServerState stateCode);
	virtual void NamedPipeServerReceiveData(JZNamePipeServer& server, BYTE* pbData, DWORD pdwDataLen);

	virtual void NamedPipeServerError(JZNamePipeServer& server, eumPipeServerError errCode, DWORD paramL=0);
};


