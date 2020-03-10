#pragma once
#include "stdafx.h"


typedef struct EMAILFILEINFOSTRUCT {
	CString	sFileName;
	CTime	tWriteTime;
	CTime   tJobTime;
	DWORD	nFileSize;
	CString sUIDL;;
} EMAILFILEINFOSTRUCT;

typedef CList <EMAILFILEINFOSTRUCT, EMAILFILEINFOSTRUCT&> EMAILFILELISTTYPE;



class CEmailClient
{
public:
	CEmailClient(void);
	~CEmailClient(void);

	int		m_SMTPconnectiontimeout;
	int		m_POP3connectiontimeout;
	BOOL	m_POP3useSSL;
	BOOL    m_SMTPuseSSL;

	CString m_proxyHTTPHost;
	int		m_proxyHTTPPort;

	void	SetSMTPConnectoinTimeout(int timeout);
	void	SetPOP3ConnectoinTimeout(int timeout);
	void	SetPOP3SSLmode(BOOL useSSL);
	void	SetSMTPSSLmode(BOOL useSSL);

	void	HTTPProxySettings(CString sProxyHTTPHostname, int nProxyHTTPPort);

	BOOL	SendMail(CString sSMTPServer, int nSMTPPort, CString sSMTPUserName, CString sSMTPPassword, CString sMailFrom, CString sMailTo, CString sMailCC, CString sMailSubject, CString sMailBody, BOOL bHtmlBody);
	BOOL	SendMailAttachment(CString sSMTPServer, int nSMTPPort, CString sSMTPUserName, CString sSMTPPassword, CString sMailFrom, CString sMailTo, CString sMailCC, CString sMailSubject, CString sMailBody, BOOL bHtmlBody, CString sAttachmentFile);

	BOOL	TestPOP3Connection(CString sPOP3Server, int nPOP3Port, CString sPOP3Username, CString sPOP3Password, TCHAR *szErrorMessage);
	int		GetMailAttachments(CString sPOP3Server, int nPOP3Port, CString sPOP3Username, CString sPOP3Password, CString sEmailFilter, BOOL bDeleteAfter,
							CString sDestFolder, EMAILFILEINFOSTRUCT aFileList[], int nMaxFiles, TCHAR *szErrorMessage);

	CString GenerateTimeStamp();

};

