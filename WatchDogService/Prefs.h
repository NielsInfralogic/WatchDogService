#pragma once
#include "Stdafx.h"
#include "Defs.h"


class CPrefs
{
public:
	CPrefs(void);
	~CPrefs(void);

	void	LoadIniFile(CString sIniFile);

	BOOL	LoadPreferencesFromRegistry();


	CString	m_apppath;
	CString m_title;

	BOOL	m_usesheelexecute;
	BOOL	m_createprocess_nocreateflag;
	BOOL	m_usepsexec;
	CString m_psexecusername;
	CString	m_psexecpassword;
	int		m_startdelay;
	int		m_numberofapplications;
	BOOL	m_enable[MAX_APPLICATIONS];
	CString m_applicationname[MAX_APPLICATIONS];
	CString m_applicationargs[MAX_APPLICATIONS];
	CString m_applicationlockfile[MAX_APPLICATIONS];
	int		m_waittimebeforeaction[MAX_APPLICATIONS];
	BOOL	m_reportrestartemail[MAX_APPLICATIONS];
	BOOL	m_restartifnolockfile[MAX_APPLICATIONS];
	BOOL	m_restartifnotinprocesslist[MAX_APPLICATIONS];
	BOOL	m_restartifkeepalivefilefails[MAX_APPLICATIONS];
	int		m_keepalivetimeout[MAX_APPLICATIONS];
	CString m_keepalivetestfile[MAX_APPLICATIONS];
	CString m_keepalivetestfile2[MAX_APPLICATIONS];
	CString m_keepalivetestfile3[MAX_APPLICATIONS];
	CString m_reportemailreceiver[MAX_APPLICATIONS];
	BOOL	m_isservice[MAX_APPLICATIONS];
	CString m_servicename[MAX_APPLICATIONS];
	BOOL	m_dailyrestart[MAX_APPLICATIONS];
	COleDateTime m_dailyrestarttime[MAX_APPLICATIONS];




	BOOL	m_bypasspingtest;
	BOOL	m_bypassreconnect;
	int		m_lockcheckmode;
	DWORD	m_maxlogfilesize;


	

	CString m_logFilePath;

	int		m_scripttimeout;


	BOOL	m_emailonfoldererror;
	BOOL	m_emailonfileerror;
	CString m_emailsmtpserver;
	int		m_mailsmtpport;
	CString m_mailsmtpserverusername;
	CString m_mailsmtpserverpassword;
	BOOL	m_mailusessl;
	CString m_emailfrom;
	CString m_emailto;
	CString m_emailcc;
	CString m_emailsubject;
	BOOL	m_emailpreventflooding;
	int		m_emailpreventfloodingdelay;
	int     m_emailtimeout;


	BOOL m_bSortOnCreateTime;

	int			m_logging;

	CString m_inifile;

	CString		m_DBserver;
	CString		m_Database;
	CString		m_DBuser;
	CString		m_DBpassword;
	BOOL		m_IntegratedSecurity;
	int			m_databaselogintimeout;
	int			m_databasequerytimeout;
	int			m_nQueryRetries;
	int			m_QueryBackoffTime;
	BOOL		m_persistentconnection;

	int m_servicememorylimit;


};

