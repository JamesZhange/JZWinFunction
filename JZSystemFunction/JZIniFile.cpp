#include <tchar.h>
#include "JZIniFile.h"

JZIniFile::JZIniFile(TCHAR* FileName)
{
	TCHAR path[MAX_PATH] = {0};
	GetModuleFileName(NULL, path, MAX_PATH);

	TCHAR* ptr = _tcsrchr(path, TEXT('\\'));
	if(ptr)
		ptr[1] = 0;
	_tcscpy(m_szFileName, path);
	_tcscat(m_szFileName, FileName);
}

JZIniFile::~JZIniFile()
{
}

int JZIniFile::GetInt(TCHAR* Section, TCHAR* Item)
{
	return ::GetPrivateProfileInt(Section, Item, 0, m_szFileName);
}

int JZIniFile::GetString(TCHAR* Section, TCHAR* Item, TCHAR* Value)
{
	return ::GetPrivateProfileString(Section, Item, 0, Value, 128, m_szFileName);	
}

BOOL JZIniFile::SetString(TCHAR* Section, TCHAR* Item, TCHAR* Value)
{
	return ::WritePrivateProfileString(Section, Item, Value, m_szFileName);
}

BOOL JZIniFile::SetInt(TCHAR* Section, TCHAR* Item, int Value)
{
	TCHAR szValue[MAX_PATH] = {0};
	_stprintf(szValue, TEXT("%d"), Value);
	return ::WritePrivateProfileString(Section, Item, szValue, m_szFileName);
}

TCHAR* JZIniFile::GetFileName()
{
	return m_szFileName;
}
