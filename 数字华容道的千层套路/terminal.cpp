#include "terminal.h"

void SetConsoleCursorPos(short  x, short y)
{
	COORD point = { x, y };
	HANDLE _hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(_hConsole, point);
}

void deepclear(RECT rect)
{
	wdeepclear(stdscr, rect);
}

void wdeepclear(WINDOW *wToClear)
{
	RECT rect = { 0, 0 };
	getmaxyx(wToClear, rect.bottom, rect.right);
	wdeepclear(wToClear, rect);
}

void wdeepclear(WINDOW *wToClear, RECT rect)
{
	POINT pWin;
	getbegyx(wToClear, pWin.y, pWin.x);
	HANDLE _hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(_hConsole, 6 << 4); // #define CTC_YELLOW 6

	for (int i = rect.top; i < rect.bottom; i++)
	{
		SetConsoleCursorPos(rect.left + pWin.x, i + pWin.y);
		for (int j = rect.left; j < rect.right; j++)
		{
			putchar(' ');
			wToClear->_y[i][j] = '*';
			mvwaddch(wToClear, i, j, ' ');
		}
	}
	SetConsoleTextAttribute(_hConsole, 0 << 4 | 7); //	#define CTC_BLACK 0 #define CTC_WHITE 7
	wrefresh(wToClear);
}

void drawframe(WINDOW* win, chtype lft, chtype rht, chtype top, chtype btm, chtype tl, chtype tr, chtype bl, chtype br, bool LeftBorderLeftMove = 0)
{
	POINT pos, size;
	getbegyx(win, pos.y, pos.x);
	getmaxyx(win, size.y, size.x);

	if (LeftBorderLeftMove)
	{
		pos.x--, size.x++;
	}

	move(pos.y - 1, pos.x - 1);
	hline(top, size.x + 2);
	move(pos.y + size.y, pos.x - 1);
	hline(btm, size.x + 2);
	move(pos.y - 1, pos.x - 1);
	vline(lft, size.y + 2);
	move(pos.y - 1, pos.x + size.x);
	vline(rht, size.y + 2);
	mvaddch(pos.y - 1, pos.x - 1, tl);
	mvaddch(pos.y - 1, pos.x + size.x, tr);
	mvaddch(pos.y + size.y, pos.x - 1, bl);
	mvaddch(pos.y + size.y, pos.x + size.x, br);
}