#pragma once

#include "stdafx.h"

#define MAX_APPLICATIONS 20
#define MAX_ERRMSG 2048
#define MAXFILESINSCAN 1000

#define LOCKCHECKMODE_NONE		0
#define LOCKCHECKMODE_READ		1
#define LOCKCHECKMODE_READWRITE	2
#define LOCKCHECKMODE_RANGELOCK	3

#define MAILTYPE_DBERROR		0
#define MAILTYPE_FILEERROR		1
#define MAILTYPE_FOLDERERROR	2


typedef struct {
	CString	sFileName;
	CString	sFolder;
	CTime	tJobTime;
	CTime	tWriteTime;
	DWORD	nFileSize;
} FILEINFOSTRUCT;

typedef CList <FILEINFOSTRUCT, FILEINFOSTRUCT&> FILELISTTYPE;

typedef struct {
	int m_ID;
	CString m_name;
} ITEMSTRUCT;

typedef CArray <ITEMSTRUCT, ITEMSTRUCT&> ITEMLIST;

