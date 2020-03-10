#pragma once

void LogEvent(WORD wType, DWORD dwID,
	const char* pszS1,
	const char* pszS2,
	const char* pszS3);

BOOL InstallService();
BOOL DeleteService();
void WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
void WINAPI ServiceCtrlHandler(DWORD Opcode);

//
// Load a message resource fom the .exe and format it with the passed insertions
//
UINT LoadMessage(DWORD dwMsgId, PTSTR pszBuffer, UINT cchBuffer, ...);

//
// Installs our app as a source of events under the name pszName into the registry
//
void AddEventSource(PCTSTR pszName, DWORD dwCategoryCount = 0);

//
// Removes the passed source of events from the registry
//
void RemoveEventSource(PCTSTR pszName);


