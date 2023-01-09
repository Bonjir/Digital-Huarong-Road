/*
* Project Name:	���ֻ��ݵ���ǧ����·
* Created on:	2022/11/16
* Edited  on:	2023/01/06
* Author:		������
*/

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#define PROJECTNAME "���ֻ��ݵ���ǧ����·"

#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <string>
#include <conio.h>
#include <windows.h>
#include "chess.h"
#include "interface.h"
#include "debug.h"
#include "procedures.h"
#include "warchess.h"

/* Menu Interface */
INTERFACE Menu_Start(INTERFACE_MENU);

INTERFACE Menu_SelectGame(INTERFACE_MENU);

INTERFACE Menu_Settings(INTERFACE_MENU);

INTERFACE Menu_Help(INTERFACE_MENU);

INTERFACE Menu_About(INTERFACE_MENU);

void Init()
{
	InitTerminal();

	INTERFACE::SetStartInterface(&Menu_Start);

	WARCHESS::InitWarChess();

	/* Menu Interface */
	Menu_Start.SetPreprint(&Menu_Start_Preprint);
	Menu_Start.SetAdditionalProc(&Menu_Start_Additional_Proc);
	Menu_Start.AddItem(L"�� ʼ �� Ϸ ", L"", &Menu_SelectGame);
	Menu_Start.AddItem(L"�� �� ", L"", &Menu_Help);
	Menu_Start.AddItem(L"�� �� ", L"", &Menu_Settings);
	Menu_Start.AddItem(L"�� �� ", L"", &Menu_About);
	Menu_Start.AddItem(L"�� �� �� Ϸ ", L"", &Display_Quit);

	Menu_SelectGame.SetTitle(L"�� Ϸ ѡ �� ");
	Menu_SelectGame.AddItem(L"�� �� �� �� ", L"", &WARCHESS::Display_Interface);
	Menu_SelectGame.AddItem(L"ä �� �� ս ", L"", &Display_Undeveloped);
	Menu_SelectGame.AddItem(L"�� �� �� ", L"", &Display_Undeveloped);
	Menu_SelectGame.AddItem(L"�� �� �� �� ", L"", &Display_Undeveloped);
	
	Menu_Settings.SetTitle(L"�� �� ");
	Menu_Settings.AddItem(L"�� �� �� �� �� �� ", L"", &WARCHESS::Menu_Settings);
	Menu_Settings.AddItem(L"�� �� �� �� ", L"", &Menu_Other_Options);

	Menu_Help.SetTitle(L"�� �� ");
	Menu_Help.AddItem(L"�� �� �� �� ", L"�� �� �� �� �� �� �� �� �� ѡ �� �� �� �� �� �� �� �� �� �� �� Ϣ �� �� �� �� �� ѡ �� �� ", &Display_Help_Basic);
	Menu_Help.AddItem(L"�� �� �� �� �� �� ", WARCHESS::brief_rule.c_str(), &Display_Help_WarchessRules);

	Menu_About.SetTitle(L"�� �� ");
	Menu_About.AddItem(L"�� �� ", L"ƿ װ �� �� ", NULL);
	Menu_About.AddItem(L"�� �� �� Ϣ ", L"�� �� �� �� �� �� \n  ǧ �� �� · \n  Version  0.1", NULL);
	Menu_About.AddItem(L"�� �� ", L"Enter �� �� �� �� �� ...", &Display_About_Others);

	Init_Menu_Other_Options();

	LogOn();
}

void Endup()
{
	WARCHESS::Endup();
	endwin();
	LogOff();
}

int main()
{
	Init();

	PROCRESULT dRet = 0;
	INTERFACE *pInterface;
	while (true)
	{
		pInterface = INTERFACE::GetCurrentInterface();
		dRet = pInterface->ProcFunc(0);
		INTERFACE::DealWithProcReturnValue(dRet);

		Sleep(10);
	}
	Endup();
	return 0;
}
