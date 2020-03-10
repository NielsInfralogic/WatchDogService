//
//  MODULE:   Registry.cpp
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

#include "registry.h"

BOOL CRegistry::OpenKey(enum Keys hKey, LPCTSTR szKey)
{	
	if(RegOpenKeyEx((HKEY)hKey,(LPCSTR)szKey, 0, KEY_ALL_ACCESS, &m_hKey) == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		m_hKey = NULL;
		return FALSE;
	}
}

BOOL CRegistry::CreateKey(enum Keys hKey, LPCTSTR szKey)
{	
	if(RegCreateKeyEx((HKEY)hKey,(LPCSTR)szKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &m_hKey, NULL) == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		m_hKey = NULL;
		return FALSE;
	}
}

BOOL CRegistry::SetValue(LPCTSTR lpValueName, LPCTSTR lpData)
{
	ASSERT(m_hKey != NULL);

	DWORD dwType = REG_SZ;

	if(::RegSetValueEx(m_hKey, (LPCSTR)lpValueName, 0, dwType, (LPBYTE)(LPCTSTR)lpData, strlen(lpData)) == ERROR_SUCCESS)
	{
		::RegFlushKey(m_hKey);
		return TRUE;
	}
	return FALSE;
}

BOOL CRegistry::SetValue(LPCTSTR lpValueName, DWORD dwValue)
{
	ASSERT(m_hKey != NULL);
	
	DWORD dwType = REG_DWORD;
	
	if(::RegSetValueEx(m_hKey, (LPCSTR)lpValueName, 0, dwType, (LPBYTE)&dwValue, sizeof(DWORD)) == ERROR_SUCCESS)
	{
		::RegFlushKey(m_hKey);
		return TRUE;
	}
	return FALSE;
}


BOOL CRegistry::GetValue(LPCTSTR lpValueName, CString& strValue)
{
	BOOL bRet = FALSE;
	LPSTR lpstrValue;
	DWORD dwType = REG_SZ;
	DWORD lpcbData;
	
	bRet = FALSE;	
	lpstrValue = (LPSTR)malloc(sizeof(char) * 256);
	memset(lpstrValue, 0, sizeof(char) * 256);
	lpcbData = 1024;
		
	if(::RegQueryValueEx(m_hKey,
		lpValueName,
		NULL,
		&dwType, 
		(BYTE*)(LPCTSTR)lpstrValue,
		&lpcbData) == ERROR_SUCCESS)
	{
		bRet = TRUE;
		strValue = (LPCTSTR)lpstrValue;
	}
	else
	{
		strValue.Empty();
	}

	return bRet;
}	


BOOL CRegistry::GetValue(LPCTSTR lpValueName, DWORD& dwValue)
{
	BOOL bRet = FALSE;
	DWORD dwType = REG_DWORD;
	DWORD lpcbData = sizeof(DWORD);
	dwValue = 0;	
	
		
	if(RegQueryValueEx(m_hKey,
		lpValueName,
		NULL,
		&dwType, 
		(BYTE*)(DWORD)&dwValue,
		&lpcbData) == ERROR_SUCCESS)
		bRet = TRUE;

	return bRet;
}

BOOL CRegistry::DeleteKey(enum Keys hKey, LPCTSTR szKey)
{	
	return ::RegDeleteKey((HKEY)hKey,(LPCSTR)szKey) == ERROR_SUCCESS;
}

BOOL CRegistry::DeleteValue(LPCTSTR lpValueName)
{	
	if(::RegDeleteValue(m_hKey,(LPCSTR)lpValueName) == ERROR_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

void CRegistry::CloseKey()
{
	::RegCloseKey(m_hKey);
	m_hKey = NULL;
}

BOOL CRegistry::SaveKey(LPCTSTR lpszFileName)
{
	ASSERT(m_hKey != NULL);
	return ::RegSaveKey(m_hKey, lpszFileName, NULL) == ERROR_SUCCESS;
}

BOOL CRegistry::RestoreKey(LPCSTR lpszFileName, DWORD dwFlags)
{
	ASSERT(m_hKey != NULL);
	return ::RegRestoreKey(m_hKey, lpszFileName, dwFlags) == ERROR_SUCCESS;
}

BOOL CRegistry::LoadKey(enum Keys hKey, LPCTSTR lpszSubKey, LPCSTR lpszFileName)
{
	return ::RegLoadKey((HKEY)hKey, lpszSubKey, lpszFileName) == ERROR_SUCCESS;
}