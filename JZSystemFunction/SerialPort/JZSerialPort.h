/*
* CComPort 再封装成习惯使用的样式
*
*
*
*还没完成
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
	virtual void OnCommunicateBreak(JZSerialPort* port) = 0; //开通或关闭串口通信时会引发此消息
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
		// ----设置回调
	}

protected:
	JZPCMPlayerDelegate* _delegate;
};

