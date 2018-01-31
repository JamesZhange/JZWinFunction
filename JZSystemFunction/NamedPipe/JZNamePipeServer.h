#pragma once
#include "JZNamePipe.h"
#include "JZGlobal.h"
#include "JZThread.h"

class JZNamePipeServerEventDelegate;


typedef enum _jznamedpipeserverrunningstate
{
	PipeServerState_Init = 0x100,
	PipeServerState_InitFaild,
	PipeServerState_WaitForClient,
	PipeServerState_Connected,
	PipeServerState_DisconnectClient,
	PipeServerState_Terminate,

}eumPipeServerState;

typedef enum _jznamedpipeservererrorcode
{
	PipeServerError_NoErr = PipeError_NoErr,

	PipeServerError_MemoryAllocFaild = PipeError_MemoryAllocFaild,

	
	PipeServerError_MildErr = 0x200,
	PipeServerError_WaitConnectFaild,
	PipeServerError_ReadBuffFaild,
	PipeServerError_WriteDataFaild,

	PipeServerError_Serious = 0x1000,
	PipeServerError_EmptyPipeName,
	PipeServerError_CreateServerDaemonThreadFaild,
	PipeServerError_CreatePipeFaild,
	PipeServerError_CreateThreadFaild,
	PipeServerError_ReadBrokenPipe,
	PipeServerError_ThreadExitUnexpected,

}eumPipeServerError;


typedef enum _jznamedpipeserverrunningstateeventhandle
{
	PipeStateHandle_Overlapped = 0,
	PipeStateHandle_ForceExit,
	PipeStateHandle_ForceExitEnd,

	PipeStateHandle_TotalNum

}eumPipeServerStateHandle;


class JZNamePipeServer: public JZNamePipe
{
public:
	JZNamePipeServer(std::wstring& sName, JZNamePipeServerEventDelegate* eventDelegate, eumPipeVector steamVector = PipeVector_Receiver);
	~JZNamePipeServer();

	void SendData(BYTE* pbData, DWORD dwDataLen);
	int DisconnectClient();

	GETPROP(eumPipeServerState, PipeState);
	GET(eumPipeServerState, PipeState) {
		return m_PipeState;
	}

private:
	
	std::wstring m_sPipeName;		// Pipe name
	HANDLE m_hPipe;                 // Pipe handle
	JZNamePipeServerEventDelegate* m_eventDelegate;

	// 退出信号（连接和接收数据时block，用外设事件的方式来控制退出）
	OVERLAPPED  WorkOP;
	HANDLE StateEvents[PipeStateHandle_TotalNum];


	DWORD ServerThreadPro();
	JZCSharpThread<JZNamePipeServer> *m_ServerThread;
	int StartServerThread();
	int isServerThreadRunning;

	int createNewPipeAndRun();

	void CleanupPipe(); 
	// void TerminatePipe();

	void OnPipeServerError(int err, DWORD paramL = 0);

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


