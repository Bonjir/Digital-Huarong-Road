#pragma once
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <windows.h>
#include "curses.h"	
#include "debug.h"

void SetConsoleCursorPos(short  x, short y);

void deepclear(RECT rctAbsolute);
void wdeepclear(WINDOW *wToClear);
void wdeepclear(WINDOW *wToClear, RECT rect);
void drawframe(WINDOW* win, chtype lft, chtype rht, chtype top, chtype btm, chtype tl, chtype tr, chtype bl, chtype br, bool LeftBorderLeftMove);

#endif