/*
* Project Name:	数字华容道的千层套路
* Created on:	2022/11/16
* Edited  on:	2023/01/06
* Author:		王晨沣
*/

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#define PROJECTNAME "数字华容道的千层套路"

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
	Menu_Start.AddItem(L"开 始 游 戏 ", L"", &Menu_SelectGame);
	Menu_Start.AddItem(L"帮 助 ", L"", &Menu_Help);
	Menu_Start.AddItem(L"设 置 ", L"", &Menu_Settings);
	Menu_Start.AddItem(L"关 于 ", L"", &Menu_About);
	Menu_Start.AddItem(L"退 出 游 戏 ", L"", &Display_Quit);

	Menu_SelectGame.SetTitle(L"游 戏 选 择 ");
	Menu_SelectGame.AddItem(L"狭 处 相 逢 ", L"", &WARCHESS::Display_Interface);
	Menu_SelectGame.AddItem(L"盲 盒 大 战 ", L"", &Display_Undeveloped);
	Menu_SelectGame.AddItem(L"大 逃 亡 ", L"", &Display_Undeveloped);
	Menu_SelectGame.AddItem(L"数 字 麻 将 ", L"", &Display_Undeveloped);
	
	Menu_Settings.SetTitle(L"设 置 ");
	Menu_Settings.AddItem(L"狭 处 相 逢 设 置 ", L"", &WARCHESS::Menu_Settings);
	Menu_Settings.AddItem(L"其 他 设 置 ", L"", &Menu_Other_Options);

	Menu_Help.SetTitle(L"帮 助 ");
	Menu_Help.AddItem(L"基 本 操 作 ", L"利 用 上 下 左 右 键 进 行 选 择 ， 按 回 车 键 查 看 具 体 信 息 或 者 进 入 该 选 项 。 ", &Display_Help_Basic);
	Menu_Help.AddItem(L"狭 处 相 逢 规 则 ", WARCHESS::brief_rule.c_str(), &Display_Help_WarchessRules);

	Menu_About.SetTitle(L"关 于 ");
	Menu_About.AddItem(L"作 者 ", L"瓶 装 蛋 糕 ", NULL);
	Menu_About.AddItem(L"版 本 信 息 ", L"数 字 华 容 道 的 \n  千 层 套 路 \n  Version  0.1", NULL);
	Menu_About.AddItem(L"其 他 ", L"Enter 以 查 看 更 多 ...", &Display_About_Others);

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
