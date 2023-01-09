#pragma once
#ifndef _PROCEDURES_H_
#define _PROCEDURES_H_

#include "warchess.h"
#include "interface.h"
#include "command.h"
#include "curses.h"
#include "colorindex.h"
#include "resource.h"

#define PROJECTNAME "数字华容道的千层套路"
#define LPROJECTNAME L"数 字 华 容 道 的 千 层 套 路 "


void Menu_Start_Preprint(INTERFACE *pInf);
PROCRESULT Menu_Start_Additional_Proc(INTERFACE *pInf, int chKey);

extern INTERFACE Menu_Help;
extern INTERFACE Display_Help_Basic;
PROCRESULT Display_Help_Basic_Proc(INTERFACE *pInf);
extern INTERFACE Display_Help_WarchessRules;
PROCRESULT Display_Help_WarchessRules_Proc(INTERFACE *pInf);

extern INTERFACE Menu_Settings;
extern INTERFACE Menu_Other_Options;
PROCRESULT Menu_Other_Options_AdditionalProc(INTERFACE *pInf, int chKey);
void Init_Menu_Other_Options();

extern INTERFACE Menu_About;
extern INTERFACE Display_About_Others;
PROCRESULT Display_About_Others_Proc(INTERFACE *pInf);
extern INTERFACE Display_About_Others_CastratedVersion;
PROCRESULT Display_About_Others_Proc_CastratedVersion(INTERFACE *pInf);

extern INTERFACE Display_Quit;
PROCRESULT Display_Quit_Proc(INTERFACE *pInf);

extern INTERFACE Display_Undeveloped;
PROCRESULT Display_Undeveloped_Proc(INTERFACE *pInf);

void InitTerminal();


#define SGM_NOANIMATION 0x1
#define SGM_NOCHANGECOLOR 0x2

bool IsGodModeOn();
bool IsShowInRedOnly();
PROCRESULT SetGodMode(bool _bGodModeOn, unsigned uMsk, INTERFACE *pCurrInf);
PROCRESULT GodModeNoChangeColor(bool bNoChangeColor, INTERFACE *pCurrInf);

extern COMMAND commGodModeOn;
extern COMMAND commGodModeOff; 
extern COMMAND commGodModeNoChangeColor;

extern int _FontSizeID;
extern POINT _FontSize[];

#endif