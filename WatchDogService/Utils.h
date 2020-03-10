#pragma once
#include "stdafx.h"
#include "Defs.h"

class CUtils
{
public:
	CUtils(void);
	~CUtils(void);
	CString Int2String(int n);
	CString UInt2String(int un);
	CString GetTempFolder();

	CString LimitErrorMessage(CString s);
	CString LimitErrorMessage(TCHAR *szStr);


	BOOL SetFileToCurrentTime(CString sFile);

	int		CRC32(CString sFileName);
	int		CRC32(CString sFileName, BOOL bFastMode);


	int		StringSplitter(CString sInput, CString sSeparators, CStringArray &sArr);
	BOOL	LockCheck(CString sFileToLock);
	BOOL	StableTimeCheck(CString sFileToTest, int nStableTimeSec);
	DWORD	GetFileSize(CString sFile);
	CTime	GetWriteTime(CString sFile);
	int		GetFileAgeMinutes(CString sFileName);
	int		GetFileAgeHours(CString sFileName);
	int		GetFolderList(CString sSpoolFolder, CStringArray &sList);
	int		GetFileAge(CString sFileName);
	CTime	Systime2CTime(SYSTEMTIME SysTime);

	BOOL	FileExist(CString sFile);
	BOOL	CheckFolder(CString sFolder);
	BOOL	CheckFolderWithPing(CString sFolder);

	BOOL	Reconnect(CString sFolder, CString sUserName, CString sPassWord);

	CString	GetExtension(CString sFileName);
	CString GetFileName(CString sFullName);
	CString GetFileName(CString sFullName, BOOL bExcludeExtension);

	CString GetFilePath(CString sFullName);
	CString	GetFirstFile(CString sSpoolFolder, CString sSearchMask);

	int  ScanDir(CString sSpoolFolder, CString sSearchMask, FILEINFOSTRUCT aFileList[], int nMaxFiles, CString sIgnoreMask,
						BOOL bIgnoreHidden, BOOL bKillZeroByteFiles);
	int	ScanDirCount(CString sSpoolFolder, CString sSearchMask);


	CTime	GetLogFileTime(CString sLogFileName);
	CTime	String2Time(CString s);
	void    ErrorFilePrintf(CString sFileName, char *msg, ...);

	int		TokenizeName(CString sName, CString sSeparators, CString sv[], CString sp[]);
	
	BOOL	TryMatchExpression(CString sMatchExpression, CString sInputString, BOOL bPartialMatch);
	CString FormatExpression(CString sMatchExpression, CString sFormatExpression, CString sInputString, BOOL bPartialMatch);

	BOOL	TryMatchExpressionEx(CString sMatchExpression, CString sInputString, BOOL bPartialMatch, CString &sErrorMessage);
	CString FormatExpressionEx(CString sMatchExpression, CString sFormatExpression, CString sInputString, BOOL bPartialMatch,  CString &sErrorMessage, BOOL *bMatched);


	BOOL IsolateReference(CString sNameFormat, CString sFileName, CString &sColor, CString &sPageNumber) ;

	CString GetRawMaskEx(CString sOrgMask);

	BOOL	DoCreateProcess(CString sCmdLine);
	BOOL	DoCreateProcess(CString sCmdLine, int nTimeout);
	int		DoCreateProcess(CString sCmdLine, int nTimeout, BOOL bBlocking, DWORD &dwExitCode, DWORD &dwProcessID);
	CString CreateProcessGetTextResult(CString sCmdLine, int nTimeout);
	void	KillProcess(DWORD dwProcessID);

	int		DeleteFiles(CString sSpoolFolder, CString sSearchMask);
	int		DeleteFiles(CString sSpoolFolder, CString sSearchMask, int sMaxAgeHours);
	int		MoveFiles(CString sSpoolFolder, CString sSearchMask, CString sOutputFolder, BOOL bIgnoreZeroLengt);
	BOOL    MoveFileWithRetry(CString sSourceFile, CString sDestinationFile, CString &sErrorMessage);
	BOOL	CopyFileWithRetry(CString sSourceFile, CString sDestinationFile, CString &sErrorMessage);

	void	TruncateLogFile(CString sFile, DWORD dwMaxSize);
	void	Logprintf(const TCHAR *msg, ...);
	int		FreeDiskSpaceMB(CString sPath);

	BOOL	SendMail(int nMailType, CString sMessage);
	BOOL	SendMail(CString sMailSubject, CString sMailBody);
	BOOL	SendMail(CString sMailSubject, CString sMailBody, CString sAttachMent);
	BOOL  SendMail(CString sMailTo, CString sMailCC, CString sMailSubject, CString sMailBody, CString sAttachMent);

	CString GenerateTimeStamp();


	CString  GetComputerName();
	CString LoadFile(CString sFilename);

	CString GetLastWin32Error();

	CString		Date2String(CTime tDate, CString sDateFormat, BOOL bAllowSeparators, int nWeekNumber);
	CString  Date2String(CTime tDate, CString sDateFormat);

	CString		Time2String(CTime tm);
	CString     Time2String(CTime tDate, CString sDateFormat);

	CTime		ParseDate(CString sDate, CString sDateFormat);
	CTime		GetNextDay(CTime dt);
	CString		CurrentTime2String();

	BOOL	SaveFile(CString sFileName, CString sContent);

	BOOL StartService(CString sServiceName);
	BOOL StopService(CString sServiceName);
	BOOL IsServiceStartedEx(CString sServiceName);
	int GetServiceMemory(CString sServiceName);
	CString EncodeBlowFish(CString sInput);
	CString DecodeBlowFish(CString sInput);

	BOOL Save(CString  csFileName, CString csDoc);
	BOOL Load(CString  csFileName, CString &csDoc);

	DWORD GenerateGUID(int nSpooler, int nTick);

	int		GetDirectoryList(CString sSpoolFolder, CString sSearchMask, FILEINFOSTRUCT aFileList[], int nMaxDirs);



	int		CRC32Poll(CString sFileName);


	CString		GetFilePID(CString sFileOrFolderPathName);
	CString		GetProcessOwner(CString sFileOrFolderPathName);
	UINT		GetOwner(LPCSTR szFileOrFolderPathName, LPSTR pUserNameBuffer, int nSizeInBytes);
	BOOL	ForceHandlesClose(CString sFilename);

	int		CopyFiles(CString sSpoolFolder, CString sSearchMask, CString sDestFolder);

	int		GetLogFileStatus(CString sFileName, CString &sErrorMessage);
	int		GetLogFileStatus(CString sFileName, CString &sErrorMessage, BOOL &bHasErrorSeverityHit, CString &sHitErrorMessage);

	CString	GetModuleLoadPath();

	int		DeleteOldFiles(CString sSpoolFolder, CString sSearchMask, int sMaxAgeHours);
	int		DeleteOldFiles(CString sSpoolFolder, CString sSearchMask, int nMaxAgeDays, int nMaxAgeHours, int nMaxAgeMinutes);
	BOOL	DeleteFileEx(CString sFile);
	BOOL	IsValidIP(CString sIP);

	int		ScanDirSimple(CString sSpoolFolder, CString sSearchMask, CStringArray &aFileList, CString sIgnoreMask, int nMaxFiles);

	void	LogDeletedFiles(int nMasterSet, CString sMessage);

	BOOL	DeleteFolder(CString sFolder);
	int		GetDirectoryList(CString sSpoolFolder, CString sSearchMask, CStringArray &aDirList);


};
