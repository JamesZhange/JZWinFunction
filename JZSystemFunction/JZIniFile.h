#pragma once
#include <windows.h>


//配置文件管理
class IIniFile
{
public:
	virtual int GetInt(TCHAR* Section, TCHAR* Item) = 0;
	virtual int GetString(TCHAR* Section, TCHAR* Item, TCHAR* Value) = 0;

	virtual BOOL SetInt(TCHAR* Section, TCHAR* Item, int Value) = 0;
	virtual BOOL SetString(TCHAR* Section, TCHAR* Item, TCHAR* Value) = 0;

	virtual TCHAR* GetFileName() = 0;
};




class JZIniFile : public IIniFile
{
public:
	JZIniFile(TCHAR* FileName);
	virtual ~JZIniFile();

	int GetInt(TCHAR* Section, TCHAR* Item);	
	int GetString(TCHAR* Section, TCHAR* Item, TCHAR* Value);

	BOOL SetString(TCHAR* Section, TCHAR* Item, TCHAR* Value);
	BOOL SetInt(TCHAR* Section, TCHAR* Item, int Value);	

	TCHAR* GetFileName();

protected:
	TCHAR m_szFileName[MAX_PATH];
};