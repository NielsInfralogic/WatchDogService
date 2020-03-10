#include "stdafx.h"
#include "DatabaseManager.h"
#include "Utils.h"
#include "Prefs.h"


extern CPrefs g_prefs;
extern CUtils g_util;

CDatabaseManager::CDatabaseManager(void)
{
	m_DBopen = FALSE;
	m_pDB = NULL;

	m_DBserver = _T(".");
	m_Database = _T("ControlCenter");
	m_DBuser = "saxxxxxxxxx";
	m_DBpassword = "xxxxxx";
	m_IntegratedSecurity = FALSE;
	m_PersistentConnection = FALSE;

}

CDatabaseManager::~CDatabaseManager(void)
{
	ExitDB();
	if (m_pDB != NULL)
		delete m_pDB;
}

BOOL CDatabaseManager::InitDB(CString sDBserver, CString sDatabase, CString sDBuser, CString sDBpassword, BOOL bIntegratedSecurity, CString &sErrorMessage)
{
	m_DBserver = sDBserver;
	m_Database = sDatabase;
	m_DBuser = sDBuser;
	m_DBpassword = sDBpassword;
	m_IntegratedSecurity = bIntegratedSecurity;

	return InitDB(sErrorMessage);
}


int CDatabaseManager::InitDB(CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (m_pDB) {
		if (m_pDB->IsOpen() == FALSE) {
			try {
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
		}
	}

	if (!m_PersistentConnection)
		ExitDB();

	if (m_DBopen)
		return TRUE;

	if (m_DBserver == _T("") || m_Database == _T("") || m_DBuser == _T("")) {
		sErrorMessage = _T("Empty server, database or username not allowed");
		return FALSE;
	}

	if (m_pDB == NULL)
		m_pDB = new CDatabase;

	m_pDB->SetLoginTimeout(g_prefs.m_databaselogintimeout);
	m_pDB->SetQueryTimeout(g_prefs.m_databasequerytimeout);

	CString sConnectStr = _T("Driver={SQL Server}; Server=") + m_DBserver + _T("; ") +
		_T("Database=") + m_Database + _T("; ");

	if (m_IntegratedSecurity)
		sConnectStr += _T(" Integrated Security=True;");
	else
		sConnectStr += _T("USER=") + m_DBuser + _T("; PASSWORD=") + m_DBpassword + _T(";");

	try {
		if (!m_pDB->OpenEx((LPCTSTR)sConnectStr, CDatabase::noOdbcDialog)) {
			sErrorMessage.Format(_T("Error connecting to database with connection string '%s'"), (LPCSTR)sConnectStr);
			return FALSE;
		}
	}
	catch (CDBException* e) {
		sErrorMessage.Format(_T("Error connecting to database - %s (%s)"), (LPCSTR)e->m_strError, (LPCSTR)sConnectStr);
		e->Delete();
		return FALSE;
	}

	m_DBopen = TRUE;
	return TRUE;
}

void CDatabaseManager::ExitDB()
{
	if (!m_DBopen)
		return;

	if (m_pDB)
		m_pDB->Close();

	m_DBopen = FALSE;

	return;
}

BOOL CDatabaseManager::IsOpen()
{
	return m_DBopen;
}

//
// SERVICE RELATED METHODS
//

BOOL CDatabaseManager::TestConnection( CString &sErrorMessage)
{
	BOOL bSuccess = FALSE;
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	CRecordset Rs(m_pDB);
	CString sSQL, s;
	sSQL.Format("SELECT TOP 1 Name FROM PublicationNames WITH (NOLOCK)");
	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format(_T("Query failed - %s"), (LPCSTR)sSQL);
			return FALSE;
		}

		if (!Rs.IsEOF()) {

			Rs.GetFieldValue((short)0, s);
			bSuccess = TRUE;
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format(_T("ERROR (DATABASEMGR): Query failed - %s"), (LPCSTR)e->m_strError);
		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}

	return bSuccess ? TRUE : FALSE;
}