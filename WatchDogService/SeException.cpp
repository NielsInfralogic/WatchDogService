#include "stdafx.h"
#include "SeException.h"
#include "Prefs.h"

extern CPrefs g_prefs;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define CASE(nSeCode,CsString) case EXCEPTION_##nSeCode: \
										CsString.Format(_T("Exception %s (0x%.8x) at address 0x%.8x."),_T(#nSeCode),EXCEPTION_##nSeCode,m_pExcPointers->ExceptionRecord->ExceptionAddress); \
										break;

void SeTranslator(UINT nSeCode, _EXCEPTION_POINTERS* pExcPointers)
{
	throw new CSeException(nSeCode, pExcPointers);
}

IMPLEMENT_DYNAMIC(CSeException, CException)

CSeException::CSeException(UINT nSeCode, _EXCEPTION_POINTERS* pExcPointers)
{
	m_nSeCode = nSeCode;
	m_pExcPointers = pExcPointers;
}

CSeException::CSeException(CSeException & CseExc)
{
	m_nSeCode = CseExc.m_nSeCode;
	m_pExcPointers = CseExc.m_pExcPointers;
}

UINT CSeException::GetSeCode()
{
	return m_nSeCode;
}

_EXCEPTION_POINTERS* CSeException::GetSePointers()
{
	return m_pExcPointers;
}

PVOID CSeException::GetExceptionAddress()
{
	return m_pExcPointers->ExceptionRecord->ExceptionAddress;
}

void CSeException::Delete(void)
{
#ifdef _DEBUG
	m_bReadyForDelete = TRUE;
#endif
	delete this;
}

void CSeException::ReportError(TCHAR *szSourceOfException)
{
	CString strMessage;
	TCHAR szFinalLine[32000];
	DWORD	nBytesWritten; 
	SYSTEMTIME	ltime;

	GetErrorMessage(strMessage);

	//rc = AfxMessageBox(strMessage,nType,nIDHelp);

	HANDLE hFile = ::CreateFile(g_prefs.m_logFilePath + _T("\\MaintServiceExceptions.log"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	// Seek to end of file
	::SetFilePointer(hFile, 0, NULL, FILE_END);
	::GetLocalTime(&ltime);

	_stprintf(szFinalLine, "[%.2d-%.2d-%.4d %.2d:%.2d:%.2d.%.3d] Source:%s - %s\r\n", (int)ltime.wDay, (int)ltime.wMonth, (int)ltime.wYear, (int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds, szSourceOfException, (LPCSTR)strMessage);
	::WriteFile(hFile, szFinalLine, (DWORD)_tcsclen(szFinalLine), &nBytesWritten, NULL);
	::CloseHandle(hFile);

}

BOOL CSeException::GetErrorMessage(CString & CsErrDescr, PUINT pnHelpContext/* = NULL*/)
{
	BOOL rc = TRUE;

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	switch (m_nSeCode) {
		CASE(ACCESS_VIOLATION, CsErrDescr);
		CASE(DATATYPE_MISALIGNMENT, CsErrDescr);
		CASE(BREAKPOINT, CsErrDescr);
		CASE(SINGLE_STEP, CsErrDescr);
		CASE(ARRAY_BOUNDS_EXCEEDED, CsErrDescr);
		CASE(FLT_DENORMAL_OPERAND, CsErrDescr);
		CASE(FLT_DIVIDE_BY_ZERO, CsErrDescr);
		CASE(FLT_INEXACT_RESULT, CsErrDescr);
		CASE(FLT_INVALID_OPERATION, CsErrDescr);
		CASE(FLT_OVERFLOW, CsErrDescr);
		CASE(FLT_STACK_CHECK, CsErrDescr);
		CASE(FLT_UNDERFLOW, CsErrDescr);
		CASE(INT_DIVIDE_BY_ZERO, CsErrDescr);
		CASE(INT_OVERFLOW, CsErrDescr);
		CASE(PRIV_INSTRUCTION, CsErrDescr);
		CASE(IN_PAGE_ERROR, CsErrDescr);
		CASE(ILLEGAL_INSTRUCTION, CsErrDescr);
		CASE(NONCONTINUABLE_EXCEPTION, CsErrDescr);
		CASE(STACK_OVERFLOW, CsErrDescr);
		CASE(INVALID_DISPOSITION, CsErrDescr);
		CASE(GUARD_PAGE, CsErrDescr);
		CASE(INVALID_HANDLE, CsErrDescr);
	default:
		CsErrDescr = _T("Unknown exception.");
		rc = FALSE;
		break;
	}

	return rc;
}

BOOL CSeException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext/* = NULL*/)
{
	ASSERT(lpszError != NULL && AfxIsValidString(lpszError, nMaxError));

	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	CString strMessage;
	GetErrorMessage(strMessage);

	if ((UINT)strMessage.GetLength() >= nMaxError) {
		lpszError[0] = 0;
		return FALSE;
	}
	else {
		lstrcpyn(lpszError, strMessage, nMaxError);
		return TRUE;
	}
}
