#include "stdafx.h"
#include <iostream>
#include <direct.h>
#include "Winsvc.h"
#include "Defs.h"
#include "Utils.h"
#include "Prefs.h"
#include "RegkeyEx.h"
#include "WorkThread.h"
#include "ServiceUtils.h"
#include "resource.h"

SERVICE_STATUS			g_ServiceStatus;
SERVICE_STATUS_HANDLE	g_ServiceStatusHandle;
TCHAR	g_szServiceName[MAX_PATH] = _T("WatchDogService");
TCHAR	g_szServiceDescription[MAX_PATH] = _T("PDFhub WatchDog Service");

extern BOOL g_BreakSignal;
extern CPrefs g_prefs;
extern CUtils g_util;



// This function makes an entry into the application event log.
// wType:
//		EVENTLOG_SUCCESS
//		EVENTLOG_ERROR_TYPE
//		EVENTLOG_WARNING_TYPE
//		EVENTLOG_INFORMATION_TYPE
//		EVENTLOG_AUDIT_SUCCESS
//		EVENTLOG_AUDIT_FAILURE
//		EVENTLOG_AUDIT_FAILURE
// dwID
//		 MSG_ERR_EXIST
void LogEvent(WORD wType, DWORD dwID,
	const char* pszS1,
	const char* pszS2,
	const char* pszS3)
{
	const char* ps[3];
	ps[0] = pszS1;
	ps[1] = pszS2;
	ps[2] = pszS3;

	int iStr = 0;
	for (int i = 0; i < 3; i++) {
		if (ps[i] != NULL) iStr++;
	}

	// Check to see if the event source has been registered,
	// and if not then register it now.
	HANDLE hEventSource = ::RegisterEventSource(NULL,            // local machine
		g_szServiceName); // source name
	if (hEventSource) {
		::ReportEvent(hEventSource,
			wType,
			0,
			dwID,
			NULL,   // sid
			iStr,
			0,
			ps,
			NULL);
		DeregisterEventSource(hEventSource);
	}
}



//
// Load a message resource fom the .exe and format it with the passed insertions
//
UINT LoadMessage(DWORD dwMsgId, PTSTR pszBuffer, UINT cchBuffer, ...)
{
	va_list args;
	va_start(args, cchBuffer);

	return FormatMessage(
		FORMAT_MESSAGE_FROM_HMODULE,
		NULL,					// Module (e.g. DLL) to search for the Message. NULL = own .EXE
		dwMsgId,				// Id of the message to look up (from "Messages.h")
		LANG_NEUTRAL,			// Language: LANG_NEUTRAL = current thread's language
		pszBuffer,				// Destination buffer
		cchBuffer,				// Character count of destination buffer
		&args					// Insertion parameters
	);
}


//
// Installs our app as a source of events under the name pszName into the registry
//
void AddEventSource(PCTSTR pszName, DWORD dwCategoryCount /* =0 */)
{
	HKEY	hRegKey = NULL;
	DWORD	dwError = 0;
	TCHAR	szPath[MAX_PATH];

	_stprintf(szPath, _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s"), pszName);

	// Create the event source registry key
	dwError = RegCreateKey(HKEY_LOCAL_MACHINE, szPath, &hRegKey);

	// Name of the PE module that contains the message resource
	GetModuleFileName(NULL, szPath, MAX_PATH);

	// Register EventMessageFile
	dwError = RegSetValueEx(hRegKey, _T("EventMessageFile"), 0, REG_EXPAND_SZ,
		(PBYTE)szPath, (_tcslen(szPath) + 1) * sizeof TCHAR);

	// Register supported event types
	DWORD dwTypes = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;
	dwError = RegSetValueEx(hRegKey, _T("TypesSupported"), 0, REG_DWORD,
		(LPBYTE)&dwTypes, sizeof dwTypes);

	// If we want to support event categories, we have also to register	the CategoryMessageFile.
	// and set CategoryCount. Note that categories need to have the message ids 1 to CategoryCount!

	if (dwCategoryCount > 0) {

		dwError = RegSetValueEx(hRegKey, _T("CategoryMessageFile"), 0, REG_EXPAND_SZ,
			(PBYTE)szPath, (_tcslen(szPath) + 1) * sizeof TCHAR);

		dwError = RegSetValueEx(hRegKey, _T("CategoryCount"), 0, REG_DWORD,
			(PBYTE)&dwCategoryCount, sizeof dwCategoryCount);
	}

	RegCloseKey(hRegKey);
}


//
// Removes the passed source of events from the registry
//
void RemoveEventSource(PCTSTR pszName)
{
	DWORD dwError = 0;
	TCHAR szPath[MAX_PATH];

	_stprintf(szPath, _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s"), pszName);
	dwError = RegDeleteKey(HKEY_LOCAL_MACHINE, szPath);
}


void WINAPI ServiceCtrlHandler(DWORD Opcode)
{
	switch (Opcode)
	{
	case SERVICE_CONTROL_PAUSE:
		g_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;

	case SERVICE_CONTROL_CONTINUE:
		g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;

	case SERVICE_CONTROL_STOP:
	{
		//			LogEvent(EVENTLOG_SUCCESS,IDS_SOCKETSTOPPED,szPort,NULL,NULL);

		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwCheckPoint = 0;
		g_ServiceStatus.dwWaitHint = 0;

		SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
//		NOTIFYICONDATA ndata;
//		ndata.hWnd = hwnd;
//		ndata.uID = 2000;
//		Shell_NotifyIcon(NIM_DELETE, &ndata);
		g_BreakSignal = true;
	}

	break;

	case SERVICE_CONTROL_INTERROGATE:
		break;
	}
	return;
}

BOOL InstallService()
{
	CRegKeyEx	keyClasses;

	SC_HANDLE schSCManager, schService;

	schSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager == NULL) {
		g_util.Logprintf(_T("Couldn't open service manager"));
		return FALSE;
	}

	// Get the executable file path 
	TCHAR szFilePath[_MAX_PATH];
	DWORD dwFLen = ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);
	if (dwFLen == 0 || dwFLen == MAX_PATH) {
		g_util.Logprintf(_T("GetModuleFileName failed"));
		return FALSE;
	}

	schService = ::CreateService(schSCManager, g_szServiceName, g_szServiceDescription,           // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, // service type 
	  //  SERVICE_DEMAND_START,      // start type 
		SERVICE_AUTO_START,

		SERVICE_ERROR_NORMAL,      // error control type 
		szFilePath,				   // service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 

	if (schService == NULL) {
		::CloseServiceHandle(schSCManager);
		g_util.Logprintf(_T("Couldn't create service"));
		return FALSE;
	}

	if (::StartService(schService, NULL, 0) == FALSE) {
		::CloseServiceHandle(schSCManager);
		g_util.Logprintf(_T("Couldn't start service"));
		return FALSE;
	}

	::CloseServiceHandle(schService);
	::CloseServiceHandle(schSCManager);

	// Save execute path in registry
	keyClasses.Create(HKEY_LOCAL_MACHINE, _T("Software\\InfraLogic\\WatchDogService"));
	keyClasses.DeleteValue(_T("Parameters"));
	

	keyClasses.SetKeyValue(_T("Parameters"), (DWORD)0, _T("Logging"));
	keyClasses.SetKeyValue(_T("Parameters"), _T("c:\\WatchDogService"), _T("LogFileFolder"));

	keyClasses.SetKeyValue(_T("Parameters"), (DWORD)15, _T("DBLoginTimeout"));
	keyClasses.SetKeyValue(_T("Parameters"), (DWORD)15, _T("DBQueryTimeout"));

	keyClasses.SetKeyValue(_T("Parameters"), (DWORD)5, _T("DBQueryRetries"));
	keyClasses.SetKeyValue(_T("Parameters"), (DWORD)500, _T("DBQueryBackoffTime"));



	keyClasses.SetKeyValue(_T("Parameters"), _T("c:\\WatchDogService\\Config.ini"), _T("ConfigFile"));

	keyClasses.Close();

	keyClasses.Create(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application"));
	keyClasses.DeleteValue(g_szServiceName);
	keyClasses.SetKeyValueEx(g_szServiceName, szFilePath, _T("EventMessageFile"));
	keyClasses.SetKeyValue(g_szServiceName, EVENTLOG_SUCCESS | EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE, _T("TypesSupported"));

	keyClasses.Close();

	keyClasses.Open(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Services"));
	keyClasses.SetKeyValue(g_szServiceName, g_szServiceDescription, _T("Description"));
	keyClasses.Close();

	return TRUE;
}

BOOL DeleteService()
{
	SC_HANDLE schSCManager;
	SC_HANDLE hService;

	schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (schSCManager == NULL) {
		g_util.Logprintf(_T("Couldn't open service manager"));
		return FALSE;
	}

	hService = OpenService(schSCManager, g_szServiceName, SERVICE_ALL_ACCESS);
	if (hService == NULL) {
		::CloseServiceHandle(schSCManager);
		g_util.Logprintf(_T("Couldn't open service"));
		return FALSE;
	}

	SERVICE_STATUS status;
	::ControlService(hService, SERVICE_CONTROL_STOP, &status);

	BOOL bDelete = ::DeleteService(hService);
	::CloseServiceHandle(hService);
	::CloseServiceHandle(schSCManager);

	if (bDelete)
		return TRUE;

	g_util.Logprintf(_T("Service could not be deleted"));
	return FALSE;
}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
	g_BreakSignal = false;
	TCHAR	szEventStr[100];
	int bOkToRun = TRUE;

	// Fill the SERVICE_STATUS structure with appropriate values to communicate with the SCM.
	g_ServiceStatus.dwServiceType = SERVICE_WIN32;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING; // Signals the SCM that the service is starting
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;   // Accepts Stop/Start only in Service control program. 
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 0;

	// Register the Service Control Handler Function
	g_ServiceStatusHandle = RegisterServiceCtrlHandler(g_szServiceName, ServiceCtrlHandler);
	if (g_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0) {
		//LogEvent(EVENTLOG_ERROR_TYPE,IDS_ERRREG,NULL,NULL,NULL);
		bOkToRun = FALSE;
	}

	g_ServiceStatus.dwCurrentState = bOkToRun ? SERVICE_RUNNING : SERVICE_STOPPED;
	g_ServiceStatus.dwCheckPoint = 0;
	g_ServiceStatus.dwWaitHint = 0;
	if (!SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus)) {
		//LogEvent(EVENTLOG_ERROR_TYPE,IDS_ERRSTARTING,NULL,NULL,NULL);
	}

	// Call the actual processing functions.
	strcpy(szEventStr, "WatchDogService");
	//	LogEvent(EVENTLOG_SUCCESS, IDS_STARTEDOK, szEventStr,NULL,NULL);


	///////////////////////////////////////////////////
	// Here is the actual starting pont
	///////////////////////////////////////////////////

	// Startup 1: Load settings from registry

	if (g_prefs.LoadPreferencesFromRegistry() == FALSE)
		g_util.Logprintf("CANNOT READ Registry: Software\\InfraLogic\\WatchDogService\\Parameters..\r\n");
	else {
		g_util.Logprintf("INFO: Registry read: Software\\InfraLogic\\WatchDogService\\Parameters..\r\n");
		//LogEvent(EVENTLOG_SUCCESS, IDS_REGISTRYREADOK, szEventStr, NULL, NULL);
	}

	TCHAR buf[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, buf);
	g_prefs.m_apppath = buf;

	CString s = g_util.GetModuleLoadPath();
	if (s != _T(""))
		g_prefs.m_apppath = s;

	
	g_prefs.LoadPreferencesFromRegistry();
	// Startup 3: Read the ini-preferences.
	if (g_prefs.m_inifile != "") {
		g_prefs.LoadIniFile(g_prefs.m_inifile);
		g_util.Logprintf("INFO: Ini config file %s read:\r\n", g_prefs.m_inifile);
		//LogEvent(EVENTLOG_SUCCESS, IDS_INILOADED, szEventStr, NULL, NULL);
	}

	



	WorkThread(NULL); 

	return;
}

