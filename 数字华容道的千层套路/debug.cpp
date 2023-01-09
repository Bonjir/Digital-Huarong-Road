#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "debug.h"

int __MAX(int a, int b)
{
	if (a > b) return a;
	return b;
}

int __MIN(int a, int b)
{
	if (a < b) return a;
	return b;
}

int __SQR(int x)
{
	return x * x;
}

int __ABS(int x)
{
	return x > 0 ? x : -x;
}

string IntToString(int a)
{
	if (a < 0)
		return "-" + IntToString(-a);
	if (a < 10) return string("") + char(a + '0');
	else return IntToString(a / 10) + char(a % 10 + '0');
}

wstring IntToStringW(int a)
{
	if (a < 0)
		return L"-" + IntToStringW(-a);
	if (a < 10) return wstring(L"") + wchar_t(a + '0');
	else return IntToStringW(a / 10) + wchar_t(a % 10 + '0');
}

int StringToInt(string s)
{
	if (s.length() <= 1)
		return (s[0] - '0');
	return StringToInt(s.substr(0, s.length() - 1)) * 10 + (s[s.length() - 1] - '0');
}

bool MsgComp(const char* a, const char* b)
{
	for (int i = __MIN(strlen(a), strlen(b)) - 1; i >= 0; i--)
	{
		if (a[i] != b[i])
			return 0;
	}
	return 1;
}

bool MsgComp2(const char *tar, const char *std)
{
	int ltar = strlen(tar), lstd = strlen(std);
	if (ltar < lstd)
		return 0;
	for (int i = 0; i < lstd; i++)
	{
		if (tar[i] != std[i])
			return 0;
	}
	return 1;
}

bool CutPrefix(char *tar, const char *std)
{
	if (MsgComp2(tar, std) == 0)
		return 0;
	int ltar = strlen(tar), lstd = strlen(std);
	for (int i = 0; i < ltar - lstd; i++)
	{
		tar[i] = tar[i + lstd];
	}
	tar[ltar - lstd] = '\0';
	return 1;
}

template<class type>
void swap(type *a, type *b)
{
	type c;
	c = a;
	a = b;
	b = c;
}

int kbgetch()
{
	int ch;
	do
		ch = getch();
	while (ch == KEY_MOUSE);
	return ch;
}

void ErrorLogOn(FILE *&ferror)
{
	ferror = fopen(ERRORLOGPATH, "a+");
	SYSTEMTIME st;
	GetLocalTime(&st);
	fprintf(ferror, "%04d %02d %02d %02d %02d %02d\n\t", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

void ErrorLogOff(FILE *&ferror)
{
	fprintf(ferror, "\n");
}

//wstring stringToWstring(const string str)
//{
//	LPCSTR pszSrc = str.c_str();
//	int nLen = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, NULL, 0);
//	if (nLen == 0)
//		return std::wstring(L"");
//
//	wchar_t* pwszDst = new wchar_t[nLen];
//	if (!pwszDst)
//		return std::wstring(L"");
//
//	MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pwszDst, nLen);
//	std::wstring wstr(pwszDst);
//	delete[] pwszDst;
//	pwszDst = NULL;
//
//	return wstr;
//}
//
//string wstringToString(const wstring wstr)
//{
//	LPCWSTR pwszSrc = wstr.c_str();
//	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
//	if (nLen == 0)
//		return std::string("");
//
//	char* pszDst = new char[nLen];
//	if (!pszDst)
//		return std::string("");
//
//	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
//	std::string str(pszDst);
//	delete[] pszDst;
//	pszDst = NULL;
//
//	return str;
//}