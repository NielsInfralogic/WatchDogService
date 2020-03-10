#include "stdafx.h"
#include "Defs.h"
#include "Prefs.h"
#include "Utils.h"

#include "Registry.h"

extern BOOL		g_BreakSignal;
extern CUtils g_util;


CPrefs::CPrefs(void)
{
	m_bypasspingtest = TRUE;
	m_bypassreconnect = FALSE;
	m_lockcheckmode = LOCKCHECKMODE_READWRITE;
	m_scripttimeout = 60;
	m_bSortOnCreateTime = FALSE;
	m_emailtimeout = 60;

	m_DBserver = _T("");
	m_Database = _T("PDFHUB");
	m_DBuser = _T("sa");
	m_DBpassword = _T("Infra2Logic");

	m_servicememorylimit = 1500;
}

CPrefs::~CPrefs(void)
{
}


void CPrefs::LoadIniFile(CString sIniFile)
{
	TCHAR Tmp[MAX_PATH];
	TCHAR Tmp2[MAX_PATH];


	GetPrivateProfileString("Setup", "UseShellExecute", "0", Tmp, 255, sIniFile);
	m_usesheelexecute = atoi(Tmp);

	GetPrivateProfileString("Setup", "UseNoCreateFlag", "1", Tmp, 255, sIniFile);
	m_createprocess_nocreateflag = atoi(Tmp);

	GetPrivateProfileString("Setup", "UsePSEXEC", "0", Tmp, 255, sIniFile);
	m_usepsexec = atoi(Tmp);

	GetPrivateProfileString("Setup", "PSEXECUsername", "", Tmp, 255, sIniFile);
	m_psexecusername = Tmp;

	GetPrivateProfileString("Setup", "PSEXECPassword", "", Tmp, 255, sIniFile);
	m_psexecpassword = Tmp;

	GetPrivateProfileString("Setup", "StartDelay", "10", Tmp, 255, sIniFile);
	m_startdelay = atoi(Tmp);

	GetPrivateProfileString("Setup", "NumberOfApplications", "1", Tmp, 255, sIniFile);
	m_numberofapplications = atoi(Tmp);

	for (int i = 0; i < m_numberofapplications; i++) {
		_stprintf(Tmp2, "Enable%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "1", Tmp, 255, sIniFile);
		m_enable[i] = atoi(Tmp);

		_stprintf(Tmp2, "AppFullPath%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_applicationname[i] = Tmp;

		_stprintf(Tmp2, "AppArguments%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_applicationargs[i] = Tmp;

		_stprintf(Tmp2, "AppLockFile%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_applicationlockfile[i] = Tmp;

		_stprintf(Tmp2, "WaitTimeBeforeAction%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "60", Tmp, 255, sIniFile);
		m_waittimebeforeaction[i] = atoi(Tmp);

		_stprintf(Tmp2, "ReportEmail%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_reportrestartemail[i] = atoi(Tmp);

		_stprintf(Tmp2, "RestartIfNoLockFile%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_restartifnolockfile[i] = atoi(Tmp);

		_stprintf(Tmp2, "RestartIfNotInProcessList%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_restartifnotinprocesslist[i] = atoi(Tmp);

		_stprintf(Tmp2, "RestartIfKeepAliveFileFails%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_restartifkeepalivefilefails[i] = atoi(Tmp);

		_stprintf(Tmp2, "KeepAliveTimeout%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_keepalivetimeout[i] = atoi(Tmp);

		_stprintf(Tmp2, "KeepAliveTestFile1_%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_keepalivetestfile[i] = Tmp;

		_stprintf(Tmp2, "KeepAliveTestFile2_%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_keepalivetestfile2[i] = Tmp;

		_stprintf(Tmp2, "KeepAliveTestFile3_%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_keepalivetestfile3[i] = Tmp;

		_stprintf(Tmp2, "ReportEmailReceiver%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_reportemailreceiver[i] = Tmp;

		_stprintf(Tmp2, "IsService%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_isservice[i] = atoi(Tmp);

		_stprintf(Tmp2, "ServiceName%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_servicename[i] = Tmp;

		_stprintf(Tmp2, "DailyRestart%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		m_dailyrestart[i] = atoi(Tmp);

		_stprintf(Tmp2, "DailyRestartHour%d", i + 1);
		GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		int nHour = atoi(Tmp);
		if (nHour > 23)
			nHour = 0;
		_stprintf(Tmp2, "DailyRestartMinute%d", i + 1);

		::GetPrivateProfileString("Setup", Tmp2, "", Tmp, 255, sIniFile);
		int nMinute = atoi(Tmp);
		if (nMinute > 59)
			nMinute = 0;
		m_dailyrestarttime[i] = COleDateTime(2000, 1, 1, nHour, nMinute, 0);

	}

	::GetPrivateProfileString("System", "ScriptTimeOut", "60", Tmp, _MAX_PATH, sIniFile);
	m_scripttimeout = _tstoi(Tmp);

	GetPrivateProfileString("Setup", "LockCheckMode", "2", Tmp, 255, sIniFile);
	m_lockcheckmode = _tstoi(Tmp);

	GetPrivateProfileString("Setup", "IgnoreLockCheck", "0", Tmp, 255, sIniFile);
	if (_tstoi(Tmp) > 0)
		m_lockcheckmode = 0;

	GetPrivateProfileString("Setup", "BypassPingTest", "1", Tmp, 255, sIniFile);
	m_bypasspingtest = _tstoi(Tmp);

	GetPrivateProfileString("Setup", "BypassReconnect", "0", Tmp, 255, sIniFile);
	m_bypassreconnect = _tstoi(Tmp);


	::GetPrivateProfileString("Setup", "MailOnFileError", "0", Tmp, _MAX_PATH, sIniFile);
	m_emailonfileerror = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MailOnFolderError", "0", Tmp, _MAX_PATH, sIniFile);
	m_emailonfoldererror = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MailSmtpServer", "", Tmp, _MAX_PATH, sIniFile);
	m_emailsmtpserver = Tmp;
	::GetPrivateProfileString("Setup", "MailSmtpPort", "25", Tmp, _MAX_PATH, sIniFile);
	m_mailsmtpport = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MailSmtpUsername", "", Tmp, _MAX_PATH, sIniFile);
	m_mailsmtpserverusername = Tmp;

	::GetPrivateProfileString("Setup", "MailSmtpPassword", "", Tmp, _MAX_PATH, sIniFile);
	m_mailsmtpserverpassword = Tmp;

	::GetPrivateProfileString("Setup", "MailUseSSL", "0", Tmp, _MAX_PATH, sIniFile);
	m_mailusessl = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MailFrom", "", Tmp, _MAX_PATH, sIniFile);
	m_emailfrom = Tmp;
	::GetPrivateProfileString("Setup", "MailTo", "", Tmp, _MAX_PATH, sIniFile);
	m_emailto = Tmp;
	::GetPrivateProfileString("Setup", "MailCc", "", Tmp, _MAX_PATH, sIniFile);
	m_emailcc = Tmp;
	::GetPrivateProfileString("Setup", "MailSubject", "", Tmp, _MAX_PATH, sIniFile);
	m_emailsubject = Tmp;

	::GetPrivateProfileString("Setup", "MailPreventFlooding", "0", Tmp, _MAX_PATH, sIniFile);
	m_emailpreventflooding = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MainPreventFloodingDelay", "10", Tmp, _MAX_PATH, sIniFile);
	m_emailpreventfloodingdelay = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MaxLogFileSize", "10485760", Tmp, _MAX_PATH, sIniFile);
	m_maxlogfilesize = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "DBserver", ".", Tmp, _MAX_PATH, sIniFile);
	m_DBserver = Tmp;
	::GetPrivateProfileString("Setup", "Database", "PDFHUB", Tmp, _MAX_PATH, sIniFile);
	m_Database = Tmp;
	::GetPrivateProfileString("Setup", "DBuser", "sa", Tmp, _MAX_PATH, sIniFile);
	m_DBuser = Tmp;
	::GetPrivateProfileString("Setup", "DBpassword", "Infra2Logic", Tmp, _MAX_PATH, sIniFile);
	m_DBpassword = Tmp;

	::GetPrivateProfileString("Setup", "ServiceMemoryLimit", "1500", Tmp, _MAX_PATH, sIniFile);
	m_servicememorylimit = atoi(Tmp);
	
}


BOOL CPrefs::LoadPreferencesFromRegistry()
{
	CRegistry pReg;

	// Set defaults
	m_logFilePath = _T("c:\\Temp");
	m_logging = 1;
	m_inifile = _T("c:\\Temp\\Config.ini");

	if (pReg.OpenKey(CRegistry::localMachine, "Software\\InfraLogic\\WatchDogService\\Parameters")) {
		CString sVal = _T("");
		DWORD nValue;
		if (pReg.GetValue("LogFileFolder", sVal))
			m_logFilePath = sVal;
	
		if (pReg.GetValue("Logging", nValue))
			m_logging = nValue;

		if (pReg.GetValue("ConfigFile", sVal))
			m_inifile = sVal;

		pReg.CloseKey();

		return TRUE;
	}

	return FALSE;
}
