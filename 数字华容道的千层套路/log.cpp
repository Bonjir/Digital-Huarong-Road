#define _CRT_SECURE_NO_WARNINGS

#include "log.h"

static FILE *flog;

string GetTimeStr()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char szTime[30] = { 0 };
	sprintf(szTime, "%d/%d/%d-%d:%d:%d", st.wYear, st.wMonth,
		st.wDay, st.wHour, st.wMinute, st.wSecond);
	return string(szTime);
}

void LogOn()
{
	LOADFILE:
	flog = fopen(LOGFILE, "a+");
	if (flog == NULL)
	{
		int dRet = MessageBox(NULL, "Cannnot open log file", "ERROR", MB_ICONHAND | MB_RETRYCANCEL);
		if (dRet == IDRETRY)
			goto LOADFILE;
	}
	fprintf(flog, "\n%s\tLog On\n", GetTimeStr().c_str());
}

void LogOff()
{
	fprintf(flog, "%s\tLog Off\n", GetTimeStr().c_str());
	fclose(flog);
}

void LogOutput(const char *szFormat, ...)
{
	fprintf(flog, "%s\t", GetTimeStr().c_str());
	va_list vararglist;
	va_start(vararglist, szFormat);
	vfprintf(flog, szFormat, vararglist);
	va_end(vararglist);
	fprintf(flog, "\n");
}