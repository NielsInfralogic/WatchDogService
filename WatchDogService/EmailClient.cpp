#include "stdafx.h"
#include "Defs.h"
#include "Utils.h"
#include "EmailClient.h"
#include <string.h>
#include <direct.h>
#include <winnetwk.h>
#include <CkEmail.h>
#include <CkEmailBundle.h>
#include <CkMailMan.h>
#include <CkStringArray.h>

CEmailClient::CEmailClient(void)
{
	m_SMTPconnectiontimeout = 60;
	m_POP3connectiontimeout = 60;
	m_POP3useSSL = FALSE;
	m_SMTPuseSSL = FALSE;
	m_proxyHTTPHost = _T("");
	m_proxyHTTPPort = 0;
}

CEmailClient::~CEmailClient(void)
{
}



void CEmailClient::SetSMTPConnectoinTimeout(int timeout)
{
	m_SMTPconnectiontimeout = timeout;
}

void CEmailClient::SetPOP3ConnectoinTimeout(int timeout)
{
	m_POP3connectiontimeout = timeout;
}

void CEmailClient::SetPOP3SSLmode(BOOL useSSL)
{
	m_POP3useSSL = useSSL;
}

void CEmailClient::SetSMTPSSLmode(BOOL useSSL)
{
	m_SMTPuseSSL = useSSL;
}



void CEmailClient::HTTPProxySettings(CString sProxyHTTPHostname, int nProxyHTTPPort)
{
	m_proxyHTTPHost = sProxyHTTPHostname;
	m_proxyHTTPPort = nProxyHTTPPort;
}

BOOL CEmailClient::TestPOP3Connection(CString sPOP3Server, int nPOP3Port, CString sPOP3Username, CString sPOP3Password, TCHAR *szErrorMessage)
{
	CkMailMan   mailman;
	CUtils util;

	bool success = mailman.UnlockComponent(_T("INFRAL.CB1092019_4czEsHZS615x"));
	if (success != true) {
		util.Logprintf(_T("POP3 ERROR: Error connecting to server %s - %s"), (LPCSTR)sPOP3Server, (LPCSTR)mailman.lastErrorText());
		sprintf(szErrorMessage, _T("Error connecting to POP3 server %s"), (LPCSTR)sPOP3Server);
		return FALSE;
	}

	if (m_POP3connectiontimeout > 0)
		mailman.put_ConnectTimeout(m_POP3connectiontimeout);

	if (m_proxyHTTPHost != _T("") && m_proxyHTTPPort > 0) {
		mailman.put_HttpProxyHostname(m_proxyHTTPHost);
		mailman.put_HttpProxyPort(m_proxyHTTPPort);
	}

	mailman.put_MailHost(sPOP3Server);
	mailman.put_PopUsername(sPOP3Username);
	mailman.put_PopPassword(sPOP3Password);

	if (m_POP3useSSL)
		mailman.put_PopSsl(true);

	if (nPOP3Port > 0 && nPOP3Port != 110)
		mailman.put_MailPort(nPOP3Port);

	int numMessages = mailman.GetMailboxCount();

	if (numMessages == -1) {
		util.Logprintf(_T("POP3 ERROR: Error getting mail count - %s"), mailman.lastErrorText());
		sprintf(szErrorMessage, _T("Error getting mail count"));
	}
	return numMessages != -1 ? TRUE : FALSE;
}


int CEmailClient::GetMailAttachments(CString sPOP3Server, int nPOP3Port, CString sPOP3Username, CString sPOP3Password, CString sEmailFilter, BOOL bDeleteAfter,
	CString sDestFolder, EMAILFILEINFOSTRUCT aFileList[], int nMaxFiles, TCHAR *szErrorMessage)
{
	CUtils util;
	int nAttachments = 0;
	_tcscpy(szErrorMessage, _T(""));

	CkMailMan   mailman;

	CStringArray aEmailList;
	aEmailList.RemoveAll();
	if (sEmailFilter.Trim() != _T(""))
		util.StringSplitter(sEmailFilter.Trim(), ",", aEmailList);

	bool success = mailman.UnlockComponent(_T("INFRAL.CB1092019_4czEsHZS615x"));
	if (success != true) {
		util.Logprintf(_T("POP3 ERROR: Error connecting to server %s - %s"), sPOP3Server, mailman.lastErrorText());
		return -1;
	}

	if (m_POP3connectiontimeout > 0)
		mailman.put_ConnectTimeout(m_POP3connectiontimeout);

	if (m_proxyHTTPHost != _T("") && m_proxyHTTPPort > 0) {
		mailman.put_HttpProxyHostname(m_proxyHTTPHost);
		mailman.put_HttpProxyPort(m_proxyHTTPPort);
	}

	mailman.put_MailHost(sPOP3Server);
	mailman.put_PopUsername(sPOP3Username);
	mailman.put_PopPassword(sPOP3Password);

	if (nPOP3Port > 0 && nPOP3Port != 110)
		mailman.put_MailPort(nPOP3Port);

	int numMessages = mailman.GetMailboxCount();

	if (numMessages == -1) {
		util.Logprintf(_T("POP3 ERROR: Error getting mail count - %s"), mailman.lastErrorText());
		mailman.Pop3EndSession();
		return -1;
	}

	if (numMessages == 0) {
		mailman.Pop3EndSession();
		return 0;
	}

	CkEmailBundle *bundle = 0;
	//  Copy the all email from the user's POP3 mailbox
	//  into a bundle object.  The email remains on the server.
	bundle = mailman.CopyMail();

	if (bundle == NULL) {
		util.Logprintf(_T("POP3 ERROR: Error copying mail to local storage - %s"), mailman.lastErrorText());
		//mailman.Pop3EndSession();
		return -1;
	}
	CkString sUIDL;
	CkEmail *email = 0;

	int nEmails = bundle->get_MessageCount();

	for (int i = 0; i <= bundle->get_MessageCount() - 1; i++) {

		email = bundle->GetEmail(i);

		int n = email->get_NumAttachments();

		//  You may also save individual attachments:
		email->get_Uidl(sUIDL);

		CString sFrom = email->ck_from();

		BOOL bPassed = FALSE;
		if (aEmailList.GetCount() > 0) {

			for (int em = 0; em < aEmailList.GetCount(); em++) {
				CString ss = aEmailList[em].Trim();
				if (ss.CompareNoCase(sFrom) != -1) {
					bPassed = TRUE;
					break;
				}
			}

		}
		else
			bPassed = TRUE;

		if (bPassed) {
			for (int j = 0; j <= n - 1; j++) {

				email->put_OverwriteExisting(false);
				CString sFileName = email->getAttachmentFilename(j);
				if (email->SaveAttachedFile(j, sDestFolder) == false) {
					util.Logprintf(_T("POP3 ERROR: Error saving attachment %s to folder %s - %s"), sFileName, sDestFolder, mailman.lastErrorText());
					continue;
				}
				sFileName = email->getAttachmentFilename(j);
				if (util.FileExist(sDestFolder + _T("\\") + sFileName)) {
					CString sNewFilename = util.GetFileName(sFileName, TRUE) + _T("_") + GenerateTimeStamp() + _T(".") + util.GetExtension(sFileName);
					::MoveFileEx(sDestFolder + _T("\\") + sFileName, sDestFolder + _T("\\") + sNewFilename, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
					aFileList[nAttachments].sFileName = sNewFilename;
					aFileList[nAttachments].nFileSize = util.GetFileSize(sDestFolder + _T("\\") + sNewFilename);
					aFileList[nAttachments].tJobTime = CTime::GetCurrentTime();
					aFileList[nAttachments].tWriteTime = CTime::GetCurrentTime();
					aFileList[nAttachments].sUIDL = sUIDL.getString();
					nAttachments++;
				}
			}

			if (bDeleteAfter)
				mailman.DeleteEmail(*email);
		}
		delete email;
	}
	delete bundle;

	return numMessages;
}

CString CEmailClient::GenerateTimeStamp()
{
	SYSTEMTIME ltime;
	CString sTimeStamp;

	::GetLocalTime(&ltime);
	sTimeStamp.Format(_T("%.4d%.2d%.2d%.2d%.2d%.2d%.3d"), (int)ltime.wYear, (int)ltime.wMonth, (int)ltime.wDay, (int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds);

	return sTimeStamp;
}

BOOL CEmailClient::SendMail(CString sSMTPServer, int nSMTPPort, CString sSMTPUserName, CString sSMTPPassword, CString sMailFrom, CString sMailTo, CString sMailCC, CString sMailSubject, CString sMailBody, BOOL bHtmlBody)
{
	return SendMailAttachment(sSMTPServer, nSMTPPort, sSMTPUserName, sSMTPPassword, sMailFrom, sMailTo, sMailCC, sMailSubject, sMailBody, bHtmlBody, _T(""));
}

BOOL CEmailClient::SendMailAttachment(CString sSMTPServer, int nSMTPPort, CString sSMTPUserName, CString sSMTPPassword, CString sMailFrom, CString sMailTo,
	CString sMailCC, CString sMailSubject, CString sMailBody, BOOL bHtmlBody, CString sAttachmentFile)
{
	CkMailMan mailman;
	CkEmail email; 
	CUtils util;

	if (sSMTPServer == _T("") || sMailFrom == _T("") || sMailTo == _T(""))
		return FALSE;

	bool success = mailman.UnlockComponent(_T("INFRAL.CB1092019_4czEsHZS615x"));
	if (success != true) {
		util.Logprintf(_T("POP3 ERROR: Error connecting to server %s - %s"), sSMTPServer, mailman.lastErrorText());
		return FALSE;
	}


	if (m_proxyHTTPHost != _T("") && m_proxyHTTPPort > 0) {
		mailman.put_HttpProxyHostname(m_proxyHTTPHost);
		mailman.put_HttpProxyPort(m_proxyHTTPPort);
	}


	//  Set the SMTP server.
	mailman.put_SmtpHost(sSMTPServer);

	//  Set the SMTP login/password (if required)
	mailman.put_SmtpUsername(sSMTPUserName);
	mailman.put_SmtpPassword(sSMTPPassword);

	if (m_SMTPconnectiontimeout > 0)
		mailman.put_ConnectTimeout(m_SMTPconnectiontimeout);


	if (m_SMTPuseSSL > 0)
		mailman.put_SmtpSsl(true);


	if (nSMTPPort > 0 && nSMTPPort != 25)
		mailman.put_SmtpPort(nSMTPPort);

	email.put_Subject(sMailSubject);
	email.put_From(sMailFrom);

	if (bHtmlBody)
		email.SetHtmlBody(sMailBody);
	else
		email.put_Body(sMailBody);

	CStringArray aMailList;
	util.StringSplitter(sMailTo, _T(",;"), aMailList);
	for (int i = 0; i < aMailList.GetCount(); i++)
		email.AddTo(aMailList[i], aMailList[i]);
	if (sMailCC != _T("")) {
		aMailList.RemoveAll();
		util.StringSplitter(sMailCC, _T(",;"), aMailList);
		for (int i = 0; i < aMailList.GetCount(); i++)
			email.AddCC(aMailList[i], aMailList[i]);
	}

	if (sAttachmentFile != _T("")) {
		const char * contentType = contentType = email.addFileAttachment(sAttachmentFile);
		if (contentType == NULL) {
			util.Logprintf(_T("SMTP WARNING: Unable to add attachment file %s - %s"), sAttachmentFile, email.lastErrorText());
			return FALSE;
		}
	}

	success = mailman.SendEmail(email);
	if (success != true) {
		util.Logprintf(_T("SMTP ERROR: Unable to connect to smtp server %s - return code %s"), sSMTPServer, mailman.lastErrorText());
		return FALSE;
	}
	else
		util.Logprintf(_T("SMTP INFO:  E-mail '%s' sent to %s with attachment %s"), sMailSubject, sMailTo, sAttachmentFile);

	success = mailman.CloseSmtpConnection();
	if (success != true) {
		util.Logprintf(_T("SMTP ERROR: Connection to SMTP server not closed cleanly"));
		return FALSE;
	}

	return  TRUE;
}
