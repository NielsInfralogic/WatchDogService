#pragma once
#include <afxdb.h>

class CDatabaseManager
{
public:
	CDatabaseManager(void);
	virtual ~CDatabaseManager(void);

	BOOL	InitDB(CString sDBserver, CString sDatabase, CString sDBuser, CString sDBpassword, BOOL bIntegratedSecurity, CString &sErrorMessage);
	BOOL	InitDB(CString &sErrorMessage);
	void	ExitDB();

	BOOL	IsOpen();
	BOOL	TestConnection( CString &sErrorMessage);

private:
	BOOL	m_DBopen;
	CDatabase *m_pDB;

	BOOL	m_IntegratedSecurity;
	CString m_DBserver;
	CString m_Database;
	CString m_DBuser;
	CString m_DBpassword;
	BOOL	m_PersistentConnection;

};

