#include "stdafx.h"
#include "Defs.h"
#include "Utils.h"
#include "Prefs.h"
#include "WorkThread.h"
#include "DatabaseManager.h"
#include "SeException.h"

extern CUtils	g_util;
extern CPrefs	g_prefs;
extern BOOL g_BreakSignal;

UINT WorkThread(LPVOID nothing)
{
	int nTicks = 0;
	int i = -1;
	BOOL bHasDBError = FALSE;
	CString sMsg = _T("");
	CDatabaseManager db;

	TCHAR szComputer[256];
	DWORD bufSize = 256;
	::GetComputerName(szComputer, &bufSize);
	
	int bRestartDay[MAX_APPLICATIONS];
	for (int k = 0; k < MAX_APPLICATIONS; k++)
		bRestartDay[k] = 0;

	if (g_prefs.m_startdelay > 0)
		::Sleep(g_prefs.m_startdelay * 1000);

	while (g_BreakSignal == FALSE) {
		for (int i = 0; i < 20; i++) {
			::Sleep(100);
			if (g_BreakSignal)
				break;
		}
		nTicks++;

		if (g_BreakSignal)
			break;

		// Check access to database... report with email 
		if ((nTicks % 60) == 0 && g_prefs.m_DBserver != _T("") && g_prefs.m_Database != _T("") && g_prefs.m_DBuser != _T("") && g_prefs.m_DBpassword != _T(""))
		{
			CString sErrorMessage = _T("");
			BOOL bConnection = db.InitDB(g_prefs.m_DBserver, g_prefs.m_Database, g_prefs.m_DBuser, g_prefs.m_DBpassword, g_prefs.m_IntegratedSecurity, sErrorMessage);
			if (bConnection == FALSE)
				bConnection = db.TestConnection(sErrorMessage);

			if (bConnection == FALSE) {
				g_util.SendMail(MAILTYPE_DBERROR, "WatchDog on " + g_util.GetComputerName() + " detected PDFHUB Database connection error!\r\n" + sErrorMessage);
			}
		}

		if (g_prefs.m_numberofapplications == 0)
			continue;

		if (++i == g_prefs.m_numberofapplications)
			i = 0;

		if (g_prefs.m_enable[i] == FALSE) {
			::Sleep(200);
			continue;
		}

		BOOL bHasRestart = FALSE;
		if (g_BreakSignal)
			break;

		if (g_prefs.m_dailyrestart[i] == TRUE) {
			int nHoursBegin = g_prefs.m_dailyrestarttime[i].GetHour();
			int nMinute = g_prefs.m_dailyrestarttime[i].GetMinute();
			CTime tNow = CTime::GetCurrentTime();

			if (bRestartDay[i] != tNow.GetDay() && tNow.GetHour() >= nHoursBegin && tNow.GetMinute() > nMinute) {

				//					util.StopServiceViaBatch(g_servicename[i]);
				g_util.StopService(g_prefs.m_servicename[i]);
				Sleep(10000);
				//util.StartServiceViaBatch(g_servicename[i]);
				g_util.StartService(g_prefs.m_servicename[i]);
				Sleep(2000);

				BOOL nServiceRunning = g_util.IsServiceStartedEx(g_prefs.m_servicename[i]);
				if (nServiceRunning) {
					g_util.Logprintf("INFO:  Service %s forced restarted", g_prefs.m_servicename[i]);
				}
				else {
					g_util.Logprintf("ERROR: Could not issue NET START command for service %s", g_prefs.m_servicename[i]);
				}
				::Sleep(61 * 1000); // Make sure we do not fall into this restart again ..

				bRestartDay[i] = tNow.GetDay();
			}
		}

		int nMemoryConsumptionMB = g_util.GetServiceMemory(g_prefs.m_servicename[i]);
		//g_util.Logprintf("INFO:  Service %s uses %d MB", g_prefs.m_servicename[i], nMemoryConsumptionMB);
		if (nMemoryConsumptionMB > g_prefs.m_servicememorylimit) {
			g_util.StopService(g_prefs.m_servicename[i]);
			Sleep(10000);
			//util.StartServiceViaBatch(g_servicename[i]);
			g_util.StartService(g_prefs.m_servicename[i]); 
			Sleep(2000);

			BOOL nServiceRunning = g_util.IsServiceStartedEx(g_prefs.m_servicename[i]);
			if (nServiceRunning) {
				g_util.Logprintf("INFO:  Service %s forced restarted due to excessive memory consumption", g_prefs.m_servicename[i]);
			}

		}

		BOOL nServiceRunning = g_util.IsServiceStartedEx(g_prefs.m_servicename[i]);
		if (nServiceRunning) {
			Sleep(2000);
			continue;
		}
		::Sleep(g_prefs.m_waittimebeforeaction[i] * 1000);
		//util.StartServiceViaBatch(g_servicename[i]);
		g_util.StartService(g_prefs.m_servicename[i]);
		Sleep(2000);
		nServiceRunning = g_util.IsServiceStartedEx(g_prefs.m_servicename[i]);
		if (nServiceRunning) {
			g_util.Logprintf("INFO:  Service %s restarted", g_prefs.m_servicename[i]);
			if (g_prefs.m_reportrestartemail[i] && g_prefs.m_reportemailreceiver[i] != "") {
				sMsg.Format("Service %s restarted on %s", g_prefs.m_servicename[i], szComputer);
				g_util.SendMail(g_prefs.m_reportemailreceiver[i], _T(""), g_prefs.m_emailsubject, sMsg, _T(""));
			}

		}
		else {

			g_util.Logprintf("ERROR: Could not issue NET START command for service %s", g_prefs.m_servicename[i]);
			if (g_prefs.m_reportrestartemail[i] && g_prefs.m_reportemailreceiver[i] != "") {
				sMsg.Format("Service %s failed to restarted on %s", g_prefs.m_servicename[i], szComputer);
				g_util.SendMail(g_prefs.m_reportemailreceiver[i], _T(""), g_prefs.m_emailsubject, sMsg, _T(""));
			}
		}
		::Sleep(2000);
	}

	return 0;
}
