#pragma once

#include <string>
//#include <conio.h>
#include "curses.h"
#include <Windows.h>
#include <cstdio>

#ifndef getch
#define getch _getch
#endif
#ifndef kbhit
#define kbhit _kbhit
#endif
#undef printf
#undef puts

using std::string;
using std::wstring;

//short __MAX(short a, short b);
//short __MIN(short a, short b);
int __MAX(int a, int b);
int __MIN(int a, int b);
int __SQR(int x);
int __ABS(int x);

string IntToString(int a);
wstring IntToStringW(int a);
int StringToInt(string s);

bool MsgComp(const char* a, const char* b);
bool MsgComp2(const char *tar, const char *std);
bool CutPrefix(char *tar, const char *std);

int kbgetch();

#ifndef loop
#define loop while(1)
#endif

#define DEBUG
#define UNDONE

#define ERRORLOGPATH "errorlog.dat"

void ErrorLogOn(FILE *&ferror);
void ErrorLogOff(FILE *&ferror);

