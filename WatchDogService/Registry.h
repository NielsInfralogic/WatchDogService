//
//  MODULE:   Registry.h
//
//	AUTHOR: Carlos Antollini 
//
//  mailto: cantollini@hotmail.com
//
//	Date: 08/04/2001
//
//	Version 1.01
// 

#include "stdafx.h"
#include "Winsvc.h"

class CRegistry
{
public:
	enum cregRestoreEnum
	{
		regVolatile = REG_WHOLE_HIVE_VOLATILE,
		regRefresh = REG_REFRESH_HIVE,
		regNoLazy = REG_NO_LAZY_FLUSH
	};

	enum Keys
	{
		classesRoot = (DWORD)HKEY_CLASSES_ROOT,
		currentUser = (DWORD)HKEY_CURRENT_USER,
		localMachine = (DWORD)HKEY_LOCAL_MACHINE,
		currentConfig = (DWORD)HKEY_CURRENT_CONFIG,
		users = (DWORD)HKEY_USERS,
		performanceData = (DWORD)HKEY_PERFORMANCE_DATA,	//Windows NT/2000
		dynData = (DWORD)HKEY_DYN_DATA						//Windows 95/98
	};

	CRegistry()
		{
			m_hKey = NULL;
		};

	~CRegistry()
		{
			CloseKey();
			m_hKey = NULL;
		};

	BOOL OpenKey(enum Keys hKey, LPCTSTR szKey);
	BOOL CreateKey(enum Keys hKey, LPCTSTR szKey);
	BOOL DeleteKey(enum Keys hKey, LPCTSTR szKey);
	BOOL DeleteValue(LPCTSTR lpValueName);
	BOOL GetValue(LPCTSTR lpValueName, CString& strValue);
	BOOL GetValue(LPCTSTR lpValueName, DWORD& dwValue);
	BOOL SetValue(LPCTSTR lpValueName, LPCTSTR lpData);
	BOOL SetValue(LPCTSTR lpValueName, DWORD dwValue);
	BOOL SaveKey(LPCTSTR lpszFileName);
	BOOL RestoreKey(LPCSTR lpszFileName, DWORD dwFlag);
	BOOL LoadKey(enum Keys hKey, LPCTSTR lpszSubKey, LPCSTR lpszFileName);
	void CloseKey();	

protected:
	HKEY m_hKey;
};