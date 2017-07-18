#pragma once
#include "JZNamePipe.h"
#include "JZGlobal.h"
#include "JZThread.h"

class JZNamePipeClientEventDelegate;


typedef enum _jznamedpipeclientrunningstate
{
	PipeClientState_Init = 0x100,
	PipeClientState_Connected,
	PipeClientState_Disconnect,
	// PipeClientState_Terminate,
}eumPipeClientState;

typedef enum _jznamedpipeclienterrorcode
{
	PipeClientError_NoErr = 0x0,

	PipeClientError_MildErr = 0x200,
	PipeClientError_TryConnectTimeOut,
	PipeClientError_ConnectFaild,
	PipeClientError_ReadBuffFaild,
	PipeClientError_WriteDataFaild,

	PipeClientError_Serious = 0x1000,
	PipeClientError_EmptyPipeName,
	// PipeClientError_CreatePipeFaild,
	PipeClientError_CreateThreadFaild,
	// PipeClientError_ReadBrokenPipe,
	PipeClientError_ThreadExitUnexpected,

}eumPipeClientError;



class JZNamePipeClient: public JZNamePipe
{
public:
	JZNamePipeClient(JZNamePipeClientEventDelegate* eventDelegate);
	~JZNamePipeClient();

	// int ConnectToPipeServer(DWORD nTimeOut); // nTimeOut µÈ´ý³¬Ê±£¬ºÁÃë
	int ConnectToPipeServer(std::wstring& sName, DWORD nTimeOut);
	void SendData(BYTE* pbData, DWORD dwDataLen);
	int DisconnectToServer();

	GETPROP(eumPipeClientState, PipeState);
	GET(eumPipeClientState, PipeState) {
		return m_PipeState;
	}

private:
	BYTE bWritebuf[PIPE_DATA_BUF];

	// std::wstring* m_pPipeName;		// Pipe name
	HANDLE m_hPipe;                 // Pipe handle
	// HANDLE m_hThread;            // Pipe thread
	JZNamePipeClientEventDelegate* m_eventDelegate;

	int isPipeConnected;

	DWORD TerminatePipe();

	void OnPipeClientError(eumPipeClientError err, DWORD paramL = 0);

	eumPipeClientState m_PipeState;
	void SetPipeClientState(eumPipeClientState state);

};




/***************************************************************/
/*********     JZNamePipeClientEventDelegate      **************/
/***************************************************************/



class JZNamePipeClientEventDelegate
{
public:
	virtual void NamedPipeClientStateChange(JZNamePipeClient& client, eumPipeClientState stateCode);
	virtual void NamedPipeClientError(JZNamePipeClient& client, eumPipeClientError errCode, DWORD paramL=0);
};


