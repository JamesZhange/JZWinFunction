/*
* CComPort �ٷ�װ��ϰ��ʹ�õ���ʽ
*
*
*
*��û���
*
*
*/

#pragma once
#include "JZGlobal.h"
#include "ThreadSerial.h"






class JZSerialPort;

/************************************/
/*    JZSerialPort Delegate         */
/************************************/

class JZPCMPlayerDelegate
{
public:
	virtual void OnReceiveData(JZSerialPort* port, void* pBuff, int nBufflen) = 0;
	virtual void OnCommunicateBreak(JZSerialPort* port) = 0; //��ͨ��رմ���ͨ��ʱ����������Ϣ
};










/************************************/
/*          JZSerialPort            */
/************************************/

class JZSerialPort : public CComPort
{
public:
	JZSerialPort();
	~JZSerialPort();

	PROP(JZPCMPlayerDelegate*, delegate)
	GET(JZPCMPlayerDelegate*, delegate) {
		return _delegate;
	}
	SET(JZPCMPlayerDelegate*, delegate) {
		// ----���ûص�
	}

protected:
	JZPCMPlayerDelegate* _delegate;
};

