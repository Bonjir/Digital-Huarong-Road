#define _CRT_SECURE_NO_WARNINGS

#include "procedures.h"

void InitTerminal()
{
	HWND hwConsole = GetConsoleWindow();
	SetWindowLong(hwConsole, GWL_STYLE, (GetWindowLong(hwConsole, GWL_STYLE) & ~WS_SIZEBOX) & ~WS_MAXIMIZEBOX);
	SetConsoleTitle(PROJECTNAME);
	SendMessage(hwConsole, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));
	SendMessage(hwConsole, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)));

	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SMALL_RECT rcConsole = { 0, 0, 80, 20 }; // 设置控制台大小
	SetConsoleWindowInfo(hOutput, true, &rcConsole);
	CONSOLE_FONT_INFOEX ConsoleCurrentFontEx; // 设置字体
	ConsoleCurrentFontEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	ConsoleCurrentFontEx.nFont = 0;
	ConsoleCurrentFontEx.dwFontSize.X = _FontSize[_FontSizeID].x;
	ConsoleCurrentFontEx.dwFontSize.Y = _FontSize[_FontSizeID].y;
	ConsoleCurrentFontEx.FontFamily = TMPF_VECTOR;
	ConsoleCurrentFontEx.FontWeight = 200;
	wcscpy(ConsoleCurrentFontEx.FaceName, L"幼圆");
	SetCurrentConsoleFontEx(hOutput, FALSE, &ConsoleCurrentFontEx);

	extern void Endup();
	auto _pfCtrlHandler = [](DWORD dCtrlType) -> bool
	{
		if (dCtrlType == CTRL_CLOSE_EVENT || dCtrlType == CTRL_SHUTDOWN_EVENT)
		{
			Endup();
		}
		return true;
	};
	bool(*pfCtrlHandler)(DWORD) = _pfCtrlHandler;
	int dRet = SetConsoleCtrlHandler((PHANDLER_ROUTINE)pfCtrlHandler,TRUE);
	if (dRet == FALSE)
		LogOutput("Unable To SetControlHanler, with error %d", GetLastError());

	initscr();
	//raw();
	//cbreak();
	keypad(stdscr, true);
	noecho();
	curs_set(false);
	mousemask(ALL_MOUSE_EVENTS, NULL);
	resize_term(CONSOLELINE, CONSOLECOL);
	start_color();
	colorindex_init();

	wbkgd(stdscr, COLOR_PAIR(COLORINDEX_CHESSBOARD));
}

void Menu_Start_Preprint(INTERFACE *pInf)
{
	wstring sTitle = LPROJECTNAME;
	const char cFrameChar = '*';
	COORD sizeConsole;
	sizeConsole.X = CONSOLECOL;
	sizeConsole.Y = CONSOLELINE;

	const int dBlankWidth = 2, dBlankHeight = 1, dFrameWidth = 1;
	int xPosPrint = (sizeConsole.X - sTitle.length() - dBlankWidth * 2 - dFrameWidth * 2) / 2,
		len = sTitle.length();

	// frame
	attroff(A_BOLD);
	attron(COLOR_PAIR(COLORINDEX_STARTMENUFRAME));
	move(1, xPosPrint);
	for (int i = 0; i < len + dBlankWidth * 2 + dFrameWidth * 2; i++)
		addch(cFrameChar);
	for (int line = 2; line <= dBlankHeight * 2 + dFrameWidth + 1; line++)
	{
		move(line, xPosPrint);
		addch(cFrameChar);
		move(line, xPosPrint + len + dBlankWidth * 2 + dFrameWidth);
		addch(cFrameChar);
	}
	move(dBlankHeight * 2 + dFrameWidth + 2, xPosPrint);
	for (int i = 0; i < len + dBlankWidth * 2 + dFrameWidth * 2; i++)
		addch(cFrameChar);
	attroff(COLOR_PAIR(COLORINDEX_STARTMENUFRAME));

	// title
	attron(COLOR_PAIR(COLORINDEX_STARTMENUTEXT) | A_BOLD);
	move(dFrameWidth + dBlankHeight + 1, xPosPrint + dBlankWidth + dFrameWidth);
	addwstr(sTitle.c_str());
	attroff(COLOR_PAIR(COLORINDEX_STARTMENUTEXT) | A_BOLD);

	move(dBlankHeight * 2 + dFrameWidth + 2 + 2, sizeConsole.X / 2);
	return;
}

PROCRESULT Menu_Start_Additional_Proc(INTERFACE *pInf, int chKey)
{
	static COMMAND commWC_SP_SB("wcspsb"); // war chess single player sb mode
	static COMMAND commWC_SP_GB("wcspgb"); // war chess single player gb mode
	static COMMAND commWC_ML_SB("wcmlsb"); // war chess melon mode sb mode
	static COMMAND commWC_ML_GB("wcmlgb"); // war chess melon mode gb mode
	static COMMAND commWC_LM_CT("wclmcr"); // war chess local multi create room
	static COMMAND commWC_LM_JN("wclmjr"); // war chess local multi join room
	//static COMMAND commSYSCOMM("syscomm"); // call out command window

	if (commWC_SP_SB.Pass(chKey))
		return PRORES_CONVERTINF(WARCHESS::Display_SP_SBMode);

	if (commWC_SP_GB.Pass(chKey))
		return PRORES_CONVERTINF(WARCHESS::Display_SP_GBMode);

	if (commWC_ML_SB.Pass(chKey))
		return PRORES_CONVERTINF(WARCHESS::Display_ML_SBMode);

	if (commWC_ML_GB.Pass(chKey))
		return PRORES_CONVERTINF(WARCHESS::Display_ML_GBMode);

	if (commWC_LM_CT.Pass(chKey))
		return PRORES_CONVERTINF(WARCHESS::Display_LM_Create);

	if (commWC_LM_JN.Pass(chKey))
		return PRORES_CONVERTINF(WARCHESS::Display_LM_Join);

	if (GetAsyncKeyState(VK_MENU) < 0 && GetKeyState(VK_F4) < 0)
		return PRORES_CONVERTINF(Display_Quit);

	return PRORES_SUCCESS;
}

#define DWKD_NOENDPAUSE 0x1 /* 最后一张时不停留，按继续键返回上一界面 */
#define DWKD_ENTERCONTINUE 0x2 /* enter 可以作为继续键 */

PROCRESULT Browser_DealWithKeyDown(int chKeyCurses, int &dCurrPage, int nPages, INTERFACE *pNextInf, INTERFACE *pPrvInf, int uMask)
{
	if (chKeyCurses == KEY_BACKSPACE || chKeyCurses == KEY_ESCAPE)
		return PRORES_BACKWARD;
	if (chKeyCurses == KEY_LEFT || chKeyCurses == KEY_UP)
		dCurrPage--;
	if (chKeyCurses == KEY_RIGHT || chKeyCurses == KEY_DOWN || ((uMask & DWKD_NOENDPAUSE) ? (chKeyCurses == KEY_ENTER) : 0))
		dCurrPage++;
	if (dCurrPage >= nPages)
	{
		if (pNextInf != NULL)
		{
			return PRORES_SHAREPARENT(*pNextInf);
		}
		else
		{
			dCurrPage = nPages - 1;
			if (uMask & DWKD_NOENDPAUSE)
				return PRORES_CONVERTINF(*INTERFACE::GetStartInterface());
		}
	}
	if (dCurrPage < 0)
	{
		if (pPrvInf != NULL)
		{
			return PRORES_SHAREPARENT(*pPrvInf);
		}
		else dCurrPage = 0;
	}
	return PRORES_SUCCESS;
}

/*----- Help Proc -----*/
INTERFACE Display_Help_Basic(Display_Help_Basic_Proc);

INTERFACE Display_Help_WarchessRules(Display_Help_WarchessRules_Proc);

PROCRESULT Display_Help_Basic_Proc(INTERFACE *pInf)
{
	/*-- changeable part --*/
	const int nPages = 1;
	const int iParentMenuIndex = 0;
	INTERFACE *pNext = &Display_Help_WarchessRules,
		*pPrv = NULL;
	static wstring help_basic_text[1] = {
L"利 用 上 下 左 右 键 进 行 选 择 ， 按 回 车 键 查 看 具 体 信 息 或 者 进 入 该 选 项 。 \n\
   左 右 键 翻 页 ， Backspace 键 返 回 上 一 界 面 ， Esc 键 返 回 主 界 面 \
" };
	/*-- end --*/

	int dCurrPage = 0;
	loop
	{
		clear();

		switch (dCurrPage)
		{
		case 0:
			mvaddwstr(3, 3, help_basic_text[0].c_str());
			break;
		default:
			mvaddwstr(3, 3, L"这 里 什 么 也 没 有 哇 awa");
			break;
		}

		attron(COLOR_PAIR(COLORINDEX_TEXTCYAN));
		wstring sTitle = pInf->GetParent()->GetItem(iParentMenuIndex)->GetName();
		mvaddwstr(1, (CONSOLECOL - sTitle.length()) / 2, sTitle.c_str());
		attroff(COLOR_PAIR(COLORINDEX_TEXTCYAN));
		mvaddstr(CONSOLELINE - 1, CONSOLECOL - 12, (string("Page ") + IntToString(dCurrPage + 1) + " of " + IntToString(nPages)).c_str());
		refresh();

		int chKeyCurses = kbgetch();
		int dKeyDownRet = Browser_DealWithKeyDown(chKeyCurses, dCurrPage, nPages,
			pNext, pPrv, 0);
		if (LOWORD(dKeyDownRet) != PRORES_SUCCESS)
			return dKeyDownRet;
	}
}

PROCRESULT Display_Help_WarchessRules_Proc(INTERFACE *pInf)
{
	/*-- changeable part --*/
	const int nPages = 3; 
	const int iParentMenuIndex = 1;
	INTERFACE *pNext = NULL,
		*pPrv = &Display_Help_Basic;
	/*-- end --*/

	int dCurrPage = 0;
	loop
	{
		clear();
		switch (dCurrPage)
		{
		case 0:
			mvaddwstr(3, 3, WARCHESS::rules[0].c_str());
			break;
		case 1:
			mvaddwstr(3, 3, WARCHESS::rules[1].c_str());
			break;
		case 2:
			mvaddwstr(3, 3, WARCHESS::rules[2].c_str());
			break;
		default:
			mvaddwstr(3, 3, L"这 里 什 么 也 没 有 哇 awa");
			break;
		}

		attron(COLOR_PAIR(COLORINDEX_TEXTCYAN) | A_BOLD);
		wstring sTitle = pInf->GetParent()->GetItem(iParentMenuIndex)->GetName();
		mvaddwstr(1, (CONSOLECOL - sTitle.length()) / 2, sTitle.c_str());
		attroff(COLOR_PAIR(COLORINDEX_TEXTCYAN) | A_BOLD);
		mvaddstr(CONSOLELINE - 1, CONSOLECOL - 12, (string("Page ") + IntToString(dCurrPage + 1) + " of " + IntToString(nPages)).c_str());

		int chKeyCurses = kbgetch();
		int dKeyDownRet = Browser_DealWithKeyDown(chKeyCurses, dCurrPage, nPages,
			pNext, pPrv, 0);
		if (LOWORD(dKeyDownRet) != PRORES_SUCCESS)
			return dKeyDownRet;
	}
}

/*----- About Proc -----*/
INTERFACE Display_About_Others(Display_About_Others_Proc);

PROCRESULT Display_About_Others_Proc(INTERFACE *pInf)
{
	/* changeable part */
	const int nPages = 2;
	wstring sLines1[8] = {
		L"特 别 鸣 谢 ： ",
		L"    myb", L"    ct", L"    cct",
		L"    csgg和 lwjj",L"    毛 毛 兔 老 师 ",
		L"",
		L"以 及 所 有 为 这 个 小 游 戏 增 添 乐 趣 的 同 学 们 ~"
	};
	wstring sLines2[1] = {
		//L"Dedicated to my high school classmates, who brought me joy, and to my most unforgettable high school life ~"
		L"谨 以 此 作 献 给 带 给 我 欢 乐 的 高 中 同 学 们 和 那 一 段 难 忘 的 高 三 岁 月 。 "
	};

	static int dProgressOfEasterEgg = 0; // 彩蛋！
	const int dMaxProgressOfEasterEgg[100] = { 10 , 20 , 30 };
	const int nTotalColors = 9;
	/* end */

	int page = 0, currpage = -1;
	loop
	{
		if (currpage != page)
			clear(), currpage = page;
		switch (page)
		{
		case 0:
		{
			for (int i = 0, colorindex; i < sizeof(sLines1) / sizeof(wstring); i++)
			{
				do
					colorindex = rand() % nTotalColors;
				while (colorindex == COLORINDEX_SELECTITEM);
				attron(COLOR_PAIR(colorindex));
				mvaddwstr(i + 1, 10, sLines1[i].c_str());
				attroff(COLOR_PAIR(colorindex));
			}

			if (dProgressOfEasterEgg >= dMaxProgressOfEasterEgg[0] && dProgressOfEasterEgg < dMaxProgressOfEasterEgg[1])
			{
				attron(COLOR_PAIR(COLORINDEX_NORMAL));
				mvaddwstr(CONSOLELINE - 1, 10, L"在 菜 单 界 面 键 入 godmodeon 会 有 惊 喜 哦 ~ ");
			}

			if (dProgressOfEasterEgg >= dMaxProgressOfEasterEgg[1] && dProgressOfEasterEgg < dMaxProgressOfEasterEgg[2])
			{
				attron(COLOR_PAIR(COLORINDEX_NORMAL));
				mvaddwstr(CONSOLELINE - 1, 10, L"有 没 有 一 种 可 能 ， 有 些 字 和 背 景 色 是 一 样 的 所 以 你 看 不 到 ... ");
			}
			break;
		}
		case 1:
		{
			FILE *fp = fopen("campus.chpic", "r+"); // char pic
			char chget = 0;
			int y = 0, x = 0;
			while (!feof(fp))
			{
				chget = fgetc(fp);
				if (chget == '\n')
				{
					y++;
					x = 0;
					continue;
				}
				else
				{
					switch (chget)
					{
					case ' ':
						attron(COLOR_PAIR(COLORINDEX_BKGDWHITE) | A_BLINK);
						break;
					case '@':
						attron(COLOR_PAIR(COLORINDEX_BKGDBLUE));
						break;
					case 'L':
						attron(COLOR_PAIR(COLORINDEX_BKGDYELLOW));
						break;
					case 'i':
						attron(COLOR_PAIR(COLORINDEX_BKGDYELLOW) | A_BLINK);
						break;
					case 'G':
						attron(COLOR_PAIR(COLORINDEX_BKGDGREEN));
						break;
					}
					mvaddch(y, x, ' ');
					x++;
				}
				attroff(A_BLINK | A_BOLD);
			}
			fclose(fp);

			attron(COLOR_PAIR(COLORINDEX_ALLGREEN));
			mvaddwstr(CONSOLELINE - 1, 3, sLines2[0].c_str());
			attroff(COLOR_PAIR(COLORINDEX_ALLGREEN));
			break;
		}
		default:
		{
			mvaddwstr(3, 3, L"这 里 什 么 也 没 有 哇 awa");
			break;
		}
		}
		refresh();

		int chKeyCurses = kbgetch();
		int dKeyDownRet = Browser_DealWithKeyDown(chKeyCurses, page, nPages, NULL, NULL, DWKD_ENTERCONTINUE | DWKD_NOENDPAUSE);
		if (LOWORD(dKeyDownRet) != PRORES_SUCCESS)
			return dKeyDownRet;

		dProgressOfEasterEgg++;
	}
}

INTERFACE Display_About_Others_CastratedVersion(Display_About_Others_Proc_CastratedVersion);
PROCRESULT Display_About_Others_Proc_CastratedVersion(INTERFACE *pInf)
{
	/* changeable part */
	const int nPages = 2;
	wstring sLines1[1] = {
		L"老 师 们 好 厉 害 讲 课 好 棒 棒 ， 助 教 们 好 帅 好 漂 亮 （ "};
	wstring sLines2[1] = {
		//L"Dedicated to my high school classmates, who brought me joy, and to my most unforgettable high school life ~"
		L"谨 以 此 作 献 给 带 给 我 欢 乐 的 高 中 同 学 们 和 那 一 段 难 忘 的 高 三 岁 月 。 "
	};

	static int dProgressOfEasterEgg = 0; // 彩蛋！
	const int dMaxProgressOfEasterEgg[100] = { 10 , 20 , 30 };
	const int nTotalColors = 9;
	/* end */

	int page = 0, currpage = -1;
	loop
	{
		if (currpage != page)
			clear(), currpage = page;
		switch (page)
		{
		case 0:
		{
			for (int i = 0, colorindex; i < sizeof(sLines1) / sizeof(wstring); i++)
			{
				do
					colorindex = rand() % nTotalColors;
				while (colorindex == COLORINDEX_SELECTITEM);
				attron(COLOR_PAIR(colorindex));
				mvaddwstr(i + 1, 10, sLines1[i].c_str());
				attroff(COLOR_PAIR(colorindex));
			}

			if (dProgressOfEasterEgg >= dMaxProgressOfEasterEgg[0] && dProgressOfEasterEgg < dMaxProgressOfEasterEgg[1])
			{
				attron(COLOR_PAIR(COLORINDEX_NORMAL));
				mvaddwstr(CONSOLELINE - 1, 10, L"在 菜 单 界 面 键 入 godmodeon 会 有 惊 喜 哦 ~ ");
			}

			if (dProgressOfEasterEgg >= dMaxProgressOfEasterEgg[1] && dProgressOfEasterEgg < dMaxProgressOfEasterEgg[2])
			{
				attron(COLOR_PAIR(COLORINDEX_NORMAL));
				mvaddwstr(CONSOLELINE - 1, 10, L"有 没 有 一 种 可 能 ， 有 些 字 和 背 景 色 是 一 样 的 所 以 你 看 不 到 ... ");
			}
			break;
		}
		case 1:
		{
			FILE *fp = fopen("campus.chpic", "r+"); // char pic
			char chget = 0;
			int y = 0, x = 0;
			while (!feof(fp))
			{
				chget = fgetc(fp);
				if (chget == '\n')
				{
					y++;
					x = 0;
					continue;
				}
				else
				{
					switch (chget)
					{
					case ' ':
						attron(COLOR_PAIR(COLORINDEX_BKGDWHITE) | A_BLINK);
						break;
					case '@':
						attron(COLOR_PAIR(COLORINDEX_BKGDBLUE));
						break;
					case 'L':
						attron(COLOR_PAIR(COLORINDEX_BKGDYELLOW));
						break;
					case 'i':
						attron(COLOR_PAIR(COLORINDEX_BKGDYELLOW) | A_BLINK);
						break;
					case 'G':
						attron(COLOR_PAIR(COLORINDEX_BKGDGREEN));
						break;
					}
					mvaddch(y, x, ' ');
					x++;
				}
				attroff(A_BLINK | A_BOLD);
			}
			fclose(fp);

			attron(COLOR_PAIR(COLORINDEX_ALLGREEN));
			mvaddwstr(CONSOLELINE - 1, 3, sLines2[0].c_str());
			attroff(COLOR_PAIR(COLORINDEX_ALLGREEN));
			break;
		}
		default:
		{
			mvaddwstr(3, 3, L"这 里 什 么 也 没 有 哇 awa");
			break;
		}
		}
		refresh();

		int chKeyCurses = kbgetch();
		int dKeyDownRet = Browser_DealWithKeyDown(chKeyCurses, page, nPages, NULL, NULL, DWKD_ENTERCONTINUE | DWKD_NOENDPAUSE);
		if (LOWORD(dKeyDownRet) != PRORES_SUCCESS)
			return dKeyDownRet;

		dProgressOfEasterEgg++;
	}
}

/*----- Quit Proc -----*/
INTERFACE Display_Quit(Display_Quit_Proc);

PROCRESULT Display_Quit_Proc(INTERFACE *pInf) // undone
{
	loop
	{
		attron(COLOR_PAIR(COLORINDEX_STARTMENUTEXT) | A_BOLD);
		wstring wsText = L"Sure To Quit ?";
		mvaddwstr(CONSOLELINE / 2 - 2, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
		wsText = L"Yes";
		wsText = L"No";
		refresh();
		int chCurses = kbgetch();
		if (chCurses == KEY_ENTER)
			exit(0);
		if (chCurses == KEY_ESCAPE || chCurses == KEY_BACKSPACE)
			return PRORES_BACKWARD;
	}
}

/*----- god mode -----*/

void SecretlyGMOn();
void SecretlyGMOff();
void SecretlyGMNoClr();

COMMAND commGodModeOn("godmodeon", SecretlyGMOn); // GOD mode on
COMMAND commGodModeOff("godmodeoff", SecretlyGMOff); // GOD mode off
COMMAND commGodModeNoChangeColor("godmodeclr", SecretlyGMNoClr); // GOD mode no change color switch

bool bGodModeOn = 0;
bool bShowInRedOnly = 0;

bool IsGodModeOn()
{
	return bGodModeOn;
}

bool IsShowInRedOnly()
{
	return bShowInRedOnly;
}

PROCRESULT SetGodMode(bool _bGodModeOn, unsigned uMsk, INTERFACE *pCurrInf)
{
	bool bPrvState = bGodModeOn;
	if (_bGodModeOn)
	{
		bGodModeOn = true;
		if (~uMsk & SGM_NOCHANGECOLOR)
		{
			godmode_color_init();
			bShowInRedOnly = true;
		}
		if (~uMsk & SGM_NOANIMATION)
		{
			clear();
			refresh();
			Sleep(1500);
			attron(COLOR_PAIR(COLORINDEX_ERRORCODE) | A_REVERSE);
			for (int j = 0; j < CONSOLECOL + CONSOLELINE; j++)
			{
				for (int i = 0; i < CONSOLELINE; i++)
				{
					for (int len = 0; len < j - i; len++)
						mvaddch(i, len, '*');
				}
				Sleep(10);
				refresh();
			}
			string sText = "  G o d   M o d e   O n  ";
			mvprintw(CONSOLELINE / 2, (CONSOLECOL - sText.length()) / 2, sText.c_str());
			refresh();
			Sleep(500);
			for (int i = 0; i < CONSOLELINE; i++)
				mvchgat(i, 0, -1, 0, COLORINDEX_ERRORCODE, NULL);
			refresh();
			Sleep(500);
			attroff(COLOR_PAIR(COLORINDEX_ERRORCODE) | A_REVERSE);

		}
	}
	else
	{
		bGodModeOn = false;
		if (bShowInRedOnly)
		{
			colorindex_init();
			bShowInRedOnly = false;
		}
		if (~uMsk & SGM_NOANIMATION)
		{
			clear();
			string sText = "  G o d   M o d e   O f f  ";
			attron(COLOR_PAIR(COLORINDEX_STARTMENUTEXT));
			mvprintw(CONSOLELINE / 2, (CONSOLECOL - sText.length()) / 2, sText.c_str());
			refresh();
			Sleep(1000);
		}
	}
	if (bPrvState != _bGodModeOn)
		if (pCurrInf != NULL)
			return PRORES_SHAREPARENT(*pCurrInf);
	return PRORES_SUCCESS;
}

PROCRESULT GodModeNoChangeColor(bool bNoChangeColor, INTERFACE *pCurrInf)
{
	if (!bGodModeOn)
		return PRORES_SUCCESS;
	if (bNoChangeColor)
	{
		bShowInRedOnly = false;
		colorindex_init();
		if (pCurrInf != NULL)
			return PRORES_SHAREPARENT(*pCurrInf);
	}
	else
	{
		bShowInRedOnly = true;
		for (int i = 0; i < COLORINDEX_NTOTAL; i++)
			init_pair(i, COLOR_RED, COLOR_BLACK);
		if (pCurrInf != NULL)
			return PRORES_SHAREPARENT(*pCurrInf);
	}
}

void SecretlyGMOn()
{
	SetGodMode(true, SGM_NOANIMATION, NULL);
}

void SecretlyGMOff()
{
	SetGodMode(false, SGM_NOANIMATION, NULL);
}

void SecretlyGMNoClr()
{
	GodModeNoChangeColor(IsShowInRedOnly(), NULL);
}


/*----- undeveloped -----*/
INTERFACE Display_Undeveloped(Display_Undeveloped_Proc);
PROCRESULT Display_Undeveloped_Proc(INTERFACE *pInf)
{
	wstring wsText = L"这 里 除 了 还 什 么 都 没 有 诶 （ 悲 ", wsGrass = L"wwwwwwww";

	attron(COLOR_PAIR(COLORINDEX_TEXTGREEN));
	for (int i = rand() % 6 + 4; i >= 0; i--)
	{
		int bBold = (rand() % 100 < 50);
		if(bBold)
			attron(A_BOLD);
		mvaddwstr(rand() % CONSOLELINE, rand() % (CONSOLECOL - wsGrass.length()) + wsGrass.length(), wsGrass.c_str());
		if (bBold)
			attroff(A_BOLD);
	}
	attroff(COLOR_PAIR(COLORINDEX_TEXTGREEN) | A_BOLD);

	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	mvaddwstr(CONSOLELINE / 2, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
	attroff(COLOR_PAIR(COLORINDEX_NORMAL));

	while (kbhit())kbgetch();
	kbgetch();
	return PRORES_BACKWARD;
}

/*----- settings -----*/
INTERFACE Menu_Other_Options(INTERFACE_MENU);

int _FontSizeID = 1;
POINT _FontSize[3] = { { 8 , 16 } , { 12 , 24 } , { 16 , 32 } };
static wstring wsWindowSizeDesc[3] = { L"小", L"中", L"大" };

void Init_Menu_Other_Options()
{
	Menu_Other_Options.SetTitle(L"其 他 设 置 ");
	Menu_Other_Options.AddItem(L"窗 口 大 小 ： 中 ", L"设 置 窗 口 大 小 ", NULL);
	Menu_Other_Options.SetAdditionalProc(Menu_Other_Options_AdditionalProc);
}

PROCRESULT Menu_Other_Options_AdditionalProc(INTERFACE *pInf, int chKey)
{
	if (chKey - KEY_LEFT && chKey - KEY_RIGHT)
		return PRORES_SUCCESS;

	MENUITEM *pmiSel = pInf->GetItem(pInf->GetSelect());
	int iSel = pInf->GetSelect();



	switch (iSel)
	{
	case 0:
	{
		if (chKey == KEY_LEFT)
		{
			if (_FontSizeID > 0)
				_FontSizeID--;
			else return PRORES_SUCCESS;
		}
		if (chKey == KEY_RIGHT)
		{
			if (_FontSizeID < sizeof(_FontSize) / sizeof(POINT) - 1)
				_FontSizeID++;
			else return PRORES_SUCCESS;
		}
		pmiSel->SetName((L"窗 口 大 小 ： " + wsWindowSizeDesc[_FontSizeID]).c_str());
		pInf->SetSelect(iSel);

		CONSOLE_FONT_INFOEX ConsoleCurrentFontEx;
		ConsoleCurrentFontEx.cbSize = sizeof(CONSOLE_FONT_INFOEX);
		ConsoleCurrentFontEx.nFont = 0;
		ConsoleCurrentFontEx.dwFontSize.X = _FontSize[_FontSizeID].x;
		ConsoleCurrentFontEx.dwFontSize.Y = _FontSize[_FontSizeID].y;
		ConsoleCurrentFontEx.FontFamily = TMPF_VECTOR;
		ConsoleCurrentFontEx.FontWeight = 200;
		wcscpy(ConsoleCurrentFontEx.FaceName, L"幼圆");
		SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &ConsoleCurrentFontEx);

		break;
	}
	}
	return PRORES_SUCCESS;
}
