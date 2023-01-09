#pragma once
#ifndef _LOG_H_
#define _LOG_H_

#include <cstdio>
#include <Windows.h>
#include <string>
#include <cstdarg>

#define LOGFILE "���ֻ��ݵ���ǧ����·.log"

using std::string;

string GetTimeStr();
void LogOn();
void LogOff();
void LogOutput(const char *szFormat, ...);

#endif