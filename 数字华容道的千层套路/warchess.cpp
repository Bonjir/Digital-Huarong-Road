#define _CRT_SECURE_NO_WARNINGS

#include "warchess.h"

using std::next_permutation;
using std::sort;

INTERFACE WARCHESS::Display_Interface(Display_Proc);
INTERFACE WARCHESS::Menu_GameMode(INTERFACE_MENU);

INTERFACE WARCHESS::Display_Win(Display_Win_Proc);
INTERFACE WARCHESS::Display_Lose(Display_Lose_Proc);
INTERFACE WARCHESS::Display_Tie(Display_Tie_Proc);

INTERFACE WARCHESS::Menu_OL_CreateOrJoinRoom(INTERFACE_MENU);
INTERFACE WARCHESS::Display_OL_Create(Display_OL_Create_Proc);
INTERFACE WARCHESS::Display_OL_Join(Display_OL_Join_Proc);
INTERFACE WARCHESS::Menu_OL_Join(INTERFACE_MENU);
INTERFACE WARCHESS::Display_OL_Prepare(Display_OL_Prepare_Proc);
INTERFACE WARCHESS::Display_OL_Move(Display_OL_Move_Proc);
INTERFACE WARCHESS::Display_OL_Wait(Display_OL_Wait_Proc);

INTERFACE WARCHESS::Menu_LM_CreateOrJoinRoom(INTERFACE_MENU);
INTERFACE WARCHESS::Display_LM_Create(Display_LM_Create_Proc);
INTERFACE WARCHESS::Display_LM_Join(Display_LM_Join_Proc);
INTERFACE WARCHESS::Display_LM_Prepare(Display_LM_Prepare_Proc);
INTERFACE WARCHESS::Display_LM_Move(Display_LM_Move_Proc);
INTERFACE WARCHESS::Display_LM_Wait(Display_LM_Wait_Proc);

INTERFACE WARCHESS::Menu_SP_DifficultySelect(INTERFACE_MENU);
INTERFACE WARCHESS::Display_SP_SBMode(Display_SP_SBMode_Proc);
INTERFACE WARCHESS::Display_SP_GBMode(Display_SP_GBMode_Proc);
INTERFACE WARCHESS::Display_SP(Display_SP_Proc);

INTERFACE WARCHESS::Menu_ML_DifficultySelect(INTERFACE_MENU);
INTERFACE WARCHESS::Display_ML_SBMode(Display_ML_SBMode_Proc);
INTERFACE WARCHESS::Display_ML_GBMode(Display_ML_GBMode_Proc);
INTERFACE WARCHESS::Display_ML(Display_ML_Proc);

INTERFACE WARCHESS::Menu_Settings(INTERFACE_MENU);

static NETWORK network;
extern HBCLIENT WCOLClnt;

void WARCHESS::InitWarChess()
{
	Menu_GameMode.SetTitle(L"模 式 选 择 ");
	Menu_GameMode.AddItem(L"网 络 联 机 ", L"", &Menu_OL_CreateOrJoinRoom);
	Menu_GameMode.AddItem(L"本 地 联 机 ", L"", &Menu_LM_CreateOrJoinRoom);
	Menu_GameMode.AddItem(L"单 人 模 式 ", L"", &Menu_SP_DifficultySelect);
	Menu_GameMode.AddItem(L"吃 瓜 模 式 ", L"", &Menu_ML_DifficultySelect);

	Menu_OL_CreateOrJoinRoom.SetTitle(L"游 戏 方 式 ");
	Menu_OL_CreateOrJoinRoom.AddItem(L"创 建 房 间 ", L"", &Display_OL_Create);
	Menu_OL_CreateOrJoinRoom.AddItem(L"加 入 房 间 ", L"", &Display_OL_Join);

	Menu_LM_CreateOrJoinRoom.SetTitle(L"游 戏 方 式 ");
	Menu_LM_CreateOrJoinRoom.AddItem(L"创 建 房 间 ", L"", &Display_LM_Create);
	Menu_LM_CreateOrJoinRoom.AddItem(L"加 入 房 间 ", L"", &Display_LM_Join);

	Menu_SP_DifficultySelect.SetTitle(L"难 度 选 择 ");
	Menu_SP_DifficultySelect.AddItem(L"沙 软 模 式 ", L"", &Display_SP_SBMode);
	Menu_SP_DifficultySelect.AddItem(L"诛 仙 模 式 ", L"", &Display_SP_GBMode);

	Menu_ML_DifficultySelect.SetTitle(L"难 度 选 择 ");
	Menu_ML_DifficultySelect.AddItem(L"沙 软 模 式 ", L"", &Display_ML_SBMode);
	Menu_ML_DifficultySelect.AddItem(L"诛 仙 模 式 ", L"", &Display_ML_GBMode);

	layer = 0;
	dLayerMaxPrepFir = 2;
	dLayerMaxPrepSec = 5;
	dLayerMaxMove = 7; 

	bShowDebugPanel = false;

	CHESS::AllocMode = ALLOCMODE_SMART;

	Menu_Settings.SetTitle(L"狭 处 相 逢 设 置 ");
	Menu_Settings.SetAdditionalProc(Menu_Settings_AddtionalProc);
	Menu_Settings.AddItem((L"棋 子 分 配 ： " + CHESS::AllocModeDesc[CHESS::AllocMode]).c_str(), L"调 整 棋 子 分 配 模 式 ", NULL);
	Menu_Settings.AddItem((L"AI思 考 时 间 限 制 ： " + IntToStringW(dLayerMaxMove)).c_str(), L"更 改 AI搜 索 层 数 ", NULL);

}

void WARCHESS::Endup()
{
	network.EndUpConnect();
	if(WCOLClnt.IsConnectAlive())
		WCOLClnt.CloseClient();
}

PROCRESULT WARCHESS::Display_Proc(INTERFACE *pInf)
{
	Menu_GameMode.SetParent(Display_Interface.GetParent());
	return Menu_GameMode.ProcFunc(0);
}

PROCRESULT WARCHESS::Display_Win_Proc(INTERFACE *pInf)
{
	/*CHESS::SelectingShow(false);
	CHESS::SelectedCancel();
	CHESS::UpdateChessBoard(UCB_DEFAULT());
	*/
	clear();
	int ret = scr_restore(IMGDAT_WIN);
	for (int i = 0; i < CONSOLELINE; i++)
		for (int j = 0; j < CONSOLECOL; j++)
			if(curscr->_y[i][j] != ' ')
				stdscr->_y[i][j] = curscr->_y[i][j];

	if (IsGodModeOn() == true)
		for (int i = 0; i < CONSOLELINE; i++)
			mvchgat(i, 0, -1, 0, COLORINDEX_ERRORCODE, NULL);
	else
		for (int i = 0; i < CONSOLELINE; i++)
			mvchgat(i, 0, -1,  A_BOLD, COLORINDEX_TEXTYELLOW, NULL);
	refresh();

	while (kbhit()) kbgetch(); // 清空kbgetch缓存
	kbgetch();

	CHESS::DestroyChessBoard();
	network.EndUpConnect();

	return PRORES_BACKWARD;
	//return PRORES_CONVERTINF(*INTERFACE::GetStartInterface());
}

PROCRESULT WARCHESS::Display_Lose_Proc(INTERFACE *pInf)
{
	/*CHESS::SelectingShow(false);
	CHESS::SelectedCancel();
	CHESS::UpdateChessBoard(UCB_DEFAULT());
*/
	clear();
	int ret = scr_restore(IMGDAT_LOSE);
	for (int i = 0; i < CONSOLELINE; i++)
		for (int j = 0; j < CONSOLECOL; j++)
			if (curscr->_y[i][j] != ' ')
				stdscr->_y[i][j] = curscr->_y[i][j];

	if (IsGodModeOn() == true)
		for (int i = 0; i < CONSOLELINE; i++)
			mvchgat(i, 0, -1, 0, COLORINDEX_ERRORCODE, NULL);
	else
		for (int i = 0; i < CONSOLELINE; i++)
			mvchgat(i, 0, -1, A_BOLD, COLORINDEX_ERRORCODE, NULL);
					

	refresh();

	while (kbhit()) kbgetch(); // 清空kbgetch缓存
	kbgetch();

	CHESS::DestroyChessBoard();
	network.EndUpConnect();

	return PRORES_BACKWARD;
}

PROCRESULT WARCHESS::Display_Tie_Proc(INTERFACE *pInf)
{
	/*CHESS::SelectingShow(false);
	CHESS::SelectedCancel();
	CHESS::UpdateChessBoard(UCB_DEFAULT());
*/

	clear();
	int ret = scr_restore(IMGDAT_TIE);
	for (int i = 0; i < CONSOLELINE; i++)
		for (int j = 0; j < CONSOLECOL; j++)
			if (curscr->_y[i][j] != ' ')
				stdscr->_y[i][j] = curscr->_y[i][j];
	if (IsGodModeOn() == true)
		for (int i = 0; i < CONSOLELINE; i++)
			mvchgat(i, 0, -1, 0, COLORINDEX_ERRORCODE, NULL);
	else
		for (int i = 0; i < CONSOLELINE; i++)
			mvchgat(i, 0, -1, A_BOLD, COLORINDEX_TEXTBLUE, NULL);
	refresh();

	while (kbhit()) kbgetch(); // 清空kbgetch缓存
	kbgetch();

	CHESS::DestroyChessBoard();
	network.EndUpConnect();

	return PRORES_BACKWARD;
}


wstring WARCHESS::brief_rule = L"\
两 个 玩 家 对 局 ， 分 为 奇 数 方 和 偶 数 方 ， 每 方 有 8 个 棋 子 ， 含 6 个 战 术 棋 子 、 1 个 战 术 核 心 棋 子 、 一 个 战 略 武 器 棋 子 。 \n\
单 击 Enter 以 查 看 更 多 ...";

wstring WARCHESS::rules[] = {
L"简 介 ： \n\
     本 游 戏 基 于 数 字 华 容 道 魔 改 ， 用 数 字 华 容 道 进 行 游 玩 会 有 更 好 的 体 验 ~\n\
两 个 玩 家 对 局 ， 分 为 奇 数 方 和 偶 数 方 ， 每 方 有 8 个 棋 子 ， 含 6 个 战 术 棋 子 、 1 个 战 术 核 心 棋 子 、 一 个 战 略 武 器 棋 子 。 \n\
奇 数 方 会 固 定 有 战 术 核 心 棋 子 1 ， 以 及 战 略 武 器 棋 子 15 ； \
偶 数 方 会 固 定 有 战 术 核 心 棋 子 0 ， 以 及 战 略 武 器 棋 子 14 ； 其 余 随 机 分 配 给 两 方 。 \n",
L"游 戏 规 则 ： \n\
     两 方 在 游 戏 开 始 前 可 以 自 由 摆 放 己 方 棋 子 ， 同 时 游 戏 进 行 中 不 可 查 看 对 方 的 棋 子 位 置 。\
游 戏 开 始 后 ， 两 方 轮 流 移 动 棋 子 ， 在 没 有 其 他 棋 子 阻 挡 的 情 况 下 可 以 横 向 或 纵 向 移 动 任 意 格 ， 如 果 该 格 上 有 对 方 棋 子 则 将 进 行 一 次 战 斗 判 定 ， \
如 果 战 斗 判 胜 则 可 以 将 对 方 棋 子 吃 掉 ， 否 则 己 方 棋 子 被 吃 掉 。 \
将 对 方 的 战 术 核 心 棋 子 击 败 后 即 获 得 游 戏 胜 利 \n",
L"战 斗 判 定 ： \n\
     奇 数 棋 与 比 它 小 1 的 偶 数 棋 是 等 价 的 ， 二 者 相 遇 会 同 归 于 尽 ； \n\
移 动 棋 子 的 一 方 称 为 进 攻 方 ， 如 果 进 攻 方 移 动 的 棋 子 与 被 进 攻 的 棋 子 奇 偶 相 同 或 被 进 攻 棋 子 点 数 大 于 进 攻 棋 子 ， 则 判 负 ； 反 之 判 胜 。 \
特 殊 的 ， 如 果 一 方 的 战 略 武 器 棋 子 遇 到 另 一 方 的 战 术 核 心 棋 子 ， 则 战 略 武 器 棋 子 落 败 。 "
};

/*---------- AI (partly) ----------*/

typedef CHESSOWNER OWNER;
int WARCHESS::bShowDebugPanel;

struct WARCHESS::TempChessBoard
{
	int pieces[4][4];
	OWNER owner[4][4];
	int Judge(int ymv, int xmv, int ydes, int xdes);
	int FindFarthestAccessible(int y, int x, int dir);
	/*int Accessible(int ymv, int xmv, int dir, int step);
	int Accessible(int ymv, int xmv, int ydes, int xdes);*/
	void Move(int ymv, int xmv, DIRECTION dir, int step);
	int TerminalDetect();
	int Assess();

	void Print(int, int);
	void CopyFrom(TempChessBoard* src);
};

int WARCHESS::TempChessBoard::TerminalDetect()
{
	bool bHaveBaseCamp[3] = { 0, 0, 0 };
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			if (this->pieces[y][x] / 2 == 0)
				if (this->owner[y][x] == OWNER_ALLY)
					bHaveBaseCamp[OWNER_ALLY] = 1;
				else if (this->owner[y][x] == OWNER_ENEMY)
					bHaveBaseCamp[OWNER_ENEMY] = 1;
		}
	}
	if (!bHaveBaseCamp[OWNER_ALLY] && !bHaveBaseCamp[OWNER_ENEMY]) // tie
		return 2;
	if (!bHaveBaseCamp[OWNER_ALLY]) // lose
		return -1;
	if (!bHaveBaseCamp[OWNER_ENEMY]) // win
		return 1;
	return 0;
}

WARCHESS::TCB *WARCHESS::RESULT(TCB *tcb, ACTION ac)
{
	tcb->Move(ac.ymv, ac.xmv, ac.dir, ac.step);
	return tcb;
}

void WARCHESS::TempChessBoard::CopyFrom(TempChessBoard* src)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			this->pieces[i][j] = src->pieces[i][j];
			this->owner[i][j] = src->owner[i][j];
		}
	}
}

void WARCHESS::TempChessBoard::Print(int y, int x)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			if (owner[i][j] == OWNER_ENEMY)
				attron(COLOR_PAIR(COLORINDEX_ERRORCODE));
			if (owner[i][j] == OWNER_NULL)
				attron(A_BLINK);
			mvprintw(i + y, x + j * 3, "%d", pieces[i][j]);
			//mvprintw(i + 5, j * 3, "%d ", owner[i][j]);
			if (owner[i][j] == OWNER_NULL)
				attroff(A_BLINK);
			if (owner[i][j] == OWNER_ENEMY)
				attroff(COLOR_PAIR(COLORINDEX_ERRORCODE));
			printw(" ");
		}
}

int WARCHESS::TempChessBoard::Judge(int ymv, int xmv, int ydes, int xdes) // 对临时棋盘移动子胜负判断
{
	if (this->pieces[ydes][xdes] == -1)
		if (this->pieces[ymv][xmv] != -1)
			return 1;
		else return -1;

	if ((this->pieces[ymv][xmv] & 1) == (this->pieces[ydes][xdes] & 1))
		return -1;

	int dTrueIDMv = this->pieces[ymv][xmv] / 2, dTrueIDDes = this->pieces[ydes][xdes] / 2;

	if (dTrueIDMv == dTrueIDDes)
		return 0;
	if (dTrueIDMv > dTrueIDDes)
		return -1 * Judge(ydes, xdes, ymv, xmv);
	if (dTrueIDMv == 0 && dTrueIDDes == 7)
		return 1;
	return -1;
}

void WARCHESS::TempChessBoard::Move(int y, int x, DIRECTION dir, int step)
{
	int ydes[4] = { y - step , y + step , y , y },
		xdes[4] = { x , x , x - step , x + step };

	int dMove = Judge(y, x, ydes[dir], xdes[dir]);
	if (dMove == 1)
	{
		this->pieces[ydes[dir]][xdes[dir]] = this->pieces[y][x];
		this->owner[ydes[dir]][xdes[dir]] = this->owner[y][x];
		this->pieces[y][x] = -1;
		this->owner[y][x] = OWNER_NULL;
	}
	if (dMove == 0)
	{
		this->pieces[ydes[dir]][xdes[dir]] = this->pieces[y][x] = -1;
		this->owner[ydes[dir]][xdes[dir]] = this->owner[y][x] = OWNER_NULL;
	}
	if (dMove == -1)
	{
		this->pieces[y][x] = -1;
		this->owner[y][x] = OWNER_NULL;
	}
}

int WARCHESS::TempChessBoard::FindFarthestAccessible(int y, int x, int dir)
{
	int stepmax[4] = { y , 3 - y , x , 3 - x };
	for (int step = 1; step <= stepmax[dir]; step++)
	{
		int ydes[4] = { y - step , y + step , y , y },
			xdes[4] = { x , x , x - step , x + step };
		if (this->pieces[ydes[dir]][xdes[dir]] != -1)
			if (this->owner[ydes[dir]][xdes[dir]] == this->owner[y][x])
				return step - 1;
			else if (this->owner[ydes[dir]][xdes[dir]] == CONVERTOWNER(this->owner[y][x]))
				return step;
	}
	return stepmax[dir];
}

int WARCHESS::TempChessBoard::Assess()
{
	int ret = 0, dBaseCampID = -1;
	bool bHaveBaseCamp[3] = { 0, 0, 0 }, bHaveOddEven[3][2] = { {0} };
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			if (this->owner[y][x] == OWNER_NULL || this->pieces[y][x] < 0)
				continue;

			if (this->owner[y][x] == OWNER_ALLY)
				ret += this->pieces[y][x] / 2;
			if (this->owner[y][x] == OWNER_ENEMY)
				ret -= this->pieces[y][x] / 2;
			if (this->pieces[y][x] / 2 == 0)
				if (this->owner[y][x] == OWNER_ALLY)
					bHaveBaseCamp[OWNER_ALLY] = 1, dBaseCampID = this->pieces[y][x];
				else if (this->owner[y][x] == OWNER_ENEMY)
					bHaveBaseCamp[OWNER_ENEMY] = 1;

			if (this->pieces[y][x] / 2 != 0 && this->pieces[y][x] / 2 != 7)
				bHaveOddEven[this->owner[y][x]][this->pieces[y][x] & 1] = true;
		}
	}

	if (!bHaveBaseCamp[OWNER_ALLY] && !bHaveBaseCamp[OWNER_ENEMY]) // tie
		return 0;
	if (!bHaveBaseCamp[OWNER_ALLY]) // lose
		return -INF;
	if (!bHaveBaseCamp[OWNER_ENEMY]) // win 
		return INF + 1;

	if (bHaveOddEven[OWNER_ALLY][dBaseCampID] == false && bHaveOddEven[OWNER_ENEMY][!dBaseCampID] == false) // tie
		return 0;
	if (bHaveOddEven[OWNER_ALLY][dBaseCampID] == false) // 我方无能够吃掉对方军旗的子
		return -CANNOTWIN_VALUE + ret;
	if (bHaveOddEven[OWNER_ENEMY][!dBaseCampID] == false) // 敌方无能够吃掉我方军旗的子
		return CANNOTWIN_VALUE + ret;

	return ret;
}

ACTION WARCHESS::Minimax_Decision(TCB *tcb, int yPrintProgress, int xPrintProgress)
{
	int ProgressMax = 4 * 4 * WARCHESS::dLayerMaxMove, dProgress = 0;
	ACTION acRet = { -1, -1, (DIRECTION)0, 0 };
	char chCursesKeydownPanel = 0;

	for (int layermax = 1; layermax <= WARCHESS::dLayerMaxMove; layermax++)
	{
		int mxvl = -INF - 1;

		for (int y = 0; y < 4; y++)
		{
		LOOP_Y:
			for (int x = 0; x < 4; x++)
			{
			LOOP_X:
				attron(COLOR_PAIR(COLORINDEX_SELECTITEM) | A_BOLD);
				dProgress++;
				mvprintw(yPrintProgress, xPrintProgress, "Progress : %.1f%%", (float)dProgress * 100 / ProgressMax);
				attroff(COLOR_PAIR(COLORINDEX_SELECTITEM) | A_BOLD);
				if (dProgress == ProgressMax || rand() % 3 == 0)
					refresh();

				if (tcb->owner[y][x] != OWNER_ALLY)
					continue;
				for (int dir = 0; dir < 4; dir++)
				{
				LOOP_DIR:
					int stepm = tcb->FindFarthestAccessible(y, x, dir); // 先遍历所有 action
					if (stepm == 0)
						continue;

					for (int step = stepm; step > 0; step--)
					{
					LOOP_STEP:
						ACTION trial = { y, x, (DIRECTION)dir, step };

						TCB bkup;
						bkup.CopyFrom(tcb);

						RESULT(tcb, trial);

						if (bShowDebugPanel)
						{
							tcb->Print(5, 0);
							//attron(COLOR_PAIR(COLORINDEX_NORMAL));
							mvprintw(9, 0, "trial on %d %d %s %d", y, x, DIRINTOSTR(dir), step);
							refresh();
							kbgetch();
							mvaddwstr(9, 0, L"                                            ");
							mvaddwstr(10, 0, L"                                            ");
						}

						int trialvl = MinValue(tcb, -INF, layermax, MakeAction(y, x, (DIRECTION)dir, step), MakeAction(-1, -1, (DIRECTION)0, 0));
						if (trialvl > mxvl)
						{
							mxvl = trialvl;
							acRet = trial;
						}

						if (bShowDebugPanel)
						{
							tcb->Print(5, 0);
							mvprintw(9, 0, "trial on %d %d %s %d", y, x, DIRINTOSTR(dir), step);
							mvprintw(10, 0, "with value %d, and mxvl %d", trialvl, mxvl);
							refresh();
							chCursesKeydownPanel = kbgetch();
							mvaddwstr(9, 0, L"                                            ");
							mvaddwstr(10, 0, L"                                            ");
						}

						tcb->CopyFrom(&bkup);

						if (bShowDebugPanel && (chCursesKeydownPanel == KEY_LEFT || chCursesKeydownPanel == KEY_BACKSPACE))
						{
							step++;
							if (step <= stepm)
								goto LOOP_STEP;
							dir--;
							if (dir >= 0)
								goto LOOP_DIR;
							x--;
							if (x >= 0)
								goto LOOP_X;
							y--;
							if (y >= 0)
								goto LOOP_Y;
							dir++;
							x++;
							y++;
						}
					}
				}
			}
		}

		if (mxvl >= INF)
			return acRet;
	}
	return acRet;
}

/*---------- online multi-player proc ----------*/
static HBCLIENT WCOLClnt;

PROCRESULT DetectQuitMsg()
{
	int dKey;
	while (kbhit())
	{
		dKey = kbgetch();
		if (dKey == KEY_ESCAPE)
		{
			WCOLClnt.SendMsg("[WCOL](quitroom)");
			return PRORES_STARTMENU;
		}
		if (dKey == KEY_BACKSPACE)
		{
			WCOLClnt.SendMsg("[WCOL](quitroom)");
			return PRORES_BACKWARD;
		}
	}
	return PRORES_SUCCESS;
}

void InvalidCommand(const char *szState, const char *szRecv)
{
	LogOutput("[WCOL] %s 无效的命令 %s", szState, szRecv);
	clear();
	attron(COLOR_PAIR(COLORINDEX_ERRORCODE));
	wstring wsText = L"指 令 无 效 ， 请 检 查 版 本 信 息 ";
	mvaddwstr(2, 3, wsText.c_str());
	//WCOLClnt.CloseClient();
	WCOLClnt.SendMsg("[WCOL](quitroom)");
	refresh();
	kbgetch();
}

PROCRESULT WaitForCommand(char *szRecv, int len, const char *stdCmd, bool bQuitable = 1)
{
	memset(szRecv, 0, len);
	while (!CutPrefix(szRecv, stdCmd))
	{
		while (!WCOLClnt.HaveCommandBuff())
		{
			if (!WCOLClnt.IsConnectAlive()) // 连接断开
			{
				clear();
				attron(COLOR_PAIR(COLORINDEX_ERRORCODE));
				wstring wsText = L"网 络 连 接 已 断 开 ， 请 检 查 网 络 ";
				mvaddwstr(2, 3, wsText.c_str());
				refresh();
				kbgetch();
				WCOLClnt.CloseClient();
				return PRORES_BACKWARD;
			}

			if (bQuitable)
			{
				int dRet = DetectQuitMsg();
				if (!(dRet & PRORES_SUCCESS))
					return dRet;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
		}
		
		string sCmd = WCOLClnt.GetCommand();
		if (sCmd.length() < len)
		{
			strcpy(szRecv, sCmd.c_str());
			szRecv[sCmd.length()] = '\0';
		}
		else
		{
			InvalidCommand("Buff过短无法容纳指令", sCmd.c_str());
			strcpy(szRecv, sCmd.substr(0, len - 1).c_str());
			szRecv[len - 1] = '\0';
			return PRORES_STARTMENU;
		}
	}
	return PRORES_SUCCESS;
}

PROCRESULT WARCHESS::Display_OL_Create_Proc(INTERFACE *pInf)
{
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	wstring wsText = L"请 稍 后 ...";
	mvaddwstr(2, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
	refresh();

	if (!WCOLClnt.IsConnectAlive())
	{
		if (WCOLClnt.InitClient() == false)
		{
			clear();
			attron(COLOR_PAIR(COLORINDEX_ERRORCODE));
			wsText = L"请 检 查 网 络 连 接 ";
			mvaddwstr(2, 3, wsText.c_str());
			refresh();
			kbgetch();
			WCOLClnt.CloseClient();
			return PRORES_BACKWARD;
		}
		if (WCOLClnt.Connect(SERVER_ADDR) == false)
		{
			clear();
			attron(COLOR_PAIR(COLORINDEX_ERRORCODE));
			wsText = L"服 务 器 已 停 止 运 行 ";
			mvaddwstr(2, 3, wsText.c_str());
			refresh();
			kbgetch();
			WCOLClnt.CloseClient();
			return PRORES_BACKWARD;
		}
	}
	clear();
	wsText = L"请 输 入 四 位 数 字 作 为 房 间 号 ";
	mvaddwstr(2, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
	refresh();

	int dKey;
	int roomid = 0;
	char szSend[257], szRecv[257] = { 0 };
	while (true)
	{
		roomid = 0;
		for (int i = 0; i < 4; i++)
		{
			while (true)
			{
				dKey = kbgetch();
				if (dKey == KEY_ESCAPE)
				{
					return PRORES_STARTMENU;
				}
				if (dKey == KEY_BACKSPACE)
				{
					return PRORES_BACKWARD;
				}

				if (dKey >= '0' && dKey <= '9')
				{
					roomid = roomid * 10 + dKey - '0';
					mvprintw(4, (CONSOLECOL - 7) / 2 + i * 2, "%d", dKey - '0');
					refresh();
					break;
				}
			}
		}
		sprintf(szSend, "[WCOL](create)%d", roomid);
		WCOLClnt.SendMsg(szSend);

		int dRet = WaitForCommand(szRecv, sizeof(szRecv),"[WCOL]");
		if (!(dRet & PRORES_SUCCESS))
		{
			WCOLClnt.SendMsg("[WCOL](quitroom)");
			return dRet;
		}

		if (CutPrefix(szRecv, "(success)"))
			break;
		else if(CutPrefix(szRecv, "(error)101"))
		{
			wsText = L"房 间 号 被 占 用 ， 请 重 新 输 入 ";
			mvprintw(4, (CONSOLECOL - 7) / 2, "       ");
			mvaddwstr(6, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
			refresh();
		}
		else
		{
			InvalidCommand("RS_NULL", szRecv);
			return PRORES_BACKWARD;
		}
	}
	clear();
	wsText = L"下 面 是 你 的 房 间 号 ";
	mvaddwstr(2, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
	for (int i = 3, roomid_digit = roomid; i >= 0; i --, roomid_digit /= 10)
		mvprintw(4, (CONSOLECOL - 7) / 2 + i * 2, "%d", roomid_digit % 10);
	wsText = L"在 等 待 玩 家 加 入 ...";
	mvaddwstr(6, 3, wsText.c_str());
	refresh();
	
	while (true)
	{
		int dRet = WaitForCommand(szRecv, sizeof(szRecv), "[WCOL]");
		if (!(dRet & PRORES_SUCCESS))
			return dRet;

		if (CutPrefix(szRecv, "(allocated)"))
			break;
		else
		{
			InvalidCommand("RS_CREATED", szRecv);
			return PRORES_BACKWARD;
		}
	}

	if (MsgComp(szRecv, "015")) // 对方是 1
	{
		CHESS::SetCamp(0);
	}
	else if (MsgComp(szRecv, "014"))
	{
		CHESS::SetCamp(1);
	}
	else
	{
		InvalidCommand("RS_JOINED", szRecv);
		return PRORES_BACKWARD;
	}

	for (int i = 0; i < 16; i++)
	{
		int dPieceID;
		sscanf(szRecv + i * 4, "%d", &dPieceID);
		CHESS::SetPieceID(i / 4, i % 4, dPieceID);
	}

	return PRORES_SHAREPARENT(Display_OL_Prepare);
}

static int roomid[100], nJoinableRoom = 0;

PROCRESULT UpdateRoomTable()
{
	WCOLClnt.SendMsg("[WCOL](roominquire)");

	char szRecv[512];
	while (TRUE)
	{
		int dRet = WaitForCommand(szRecv, sizeof(szRecv), "[WCOL]");
		if (!(dRet & PRORES_SUCCESS))
			return dRet;

		if (CutPrefix(szRecv, "(roominfo)"))
			break;
		else
		{
			InvalidCommand("RS_CREATED", szRecv);
			return PRORES_BACKWARD;
		}
	}

	int nRoom, len = strlen(szRecv), roomsta;
	nJoinableRoom = 0;

	sscanf(szRecv, "%d", &nRoom);
	char *pReadRecv = szRecv;
	for (; pReadRecv < szRecv + len; pReadRecv++)
		if (*pReadRecv == '@')
			break;
	for (int i = 0; i < nRoom; i++)
	{
		sscanf(pReadRecv, "@%d:%d", roomid + nJoinableRoom, &roomsta);
		for (pReadRecv++; pReadRecv < szRecv + len; pReadRecv++)
			if (*pReadRecv == '@')
				break;
		if (roomsta == RS_CREATED)
		{
			nJoinableRoom++;
		}
	}
	return PRORES_SUCCESS;
}

PROCRESULT WARCHESS::Display_OL_Join_Proc(INTERFACE *pInf)
{
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	wstring wsText = L"请 稍 后 ...";
	mvaddwstr(2, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
	refresh();

	if (!WCOLClnt.IsConnectAlive())
	{
		if (WCOLClnt.InitClient() == false)
		{
			clear();
			attron(COLOR_PAIR(COLORINDEX_ERRORCODE));
			wsText = L"请 检 查 网 络 连 接 ";
			mvaddwstr(2, 3, wsText.c_str());
			refresh();
			kbgetch();
			WCOLClnt.CloseClient();
			return PRORES_BACKWARD;
		}
		if (WCOLClnt.Connect(SERVER_ADDR) == false)
		{
			clear();
			attron(COLOR_PAIR(COLORINDEX_ERRORCODE));
			wsText = L"服 务 器 已 停 止 运 行 ";
			mvaddwstr(2, 3, wsText.c_str());
			refresh();
			kbgetch();
			WCOLClnt.CloseClient();
			return PRORES_BACKWARD;
		}
	}
	
	int dRet = UpdateRoomTable();
	if (dRet != PRORES_SUCCESS)
		return dRet;

	Menu_OL_Join.ClearMenu();
	Menu_OL_Join.SetTitle(L"加 入 房 间 ");
	for (int i = 0; i < nJoinableRoom; i++)
	{
		Menu_OL_Join.AddItem(IntToStringW(roomid[i]).c_str(), L"", NULL);
		Menu_OL_Join.SetEnterFunc(Menu_OL_Join.GetItem(i), Menu_OL_Join_EnterProc);
	}

	return PRORES_SHAREPARENT(Menu_OL_Join);
}

PROCRESULT WARCHESS::Menu_OL_Join_EnterProc(INTERFACE *pInf)
{
	int iSel = pInf->GetSelect();
	char szSend[40];
	sprintf(szSend, "[WCOL](join)%d", roomid[iSel]);
	WCOLClnt.SendMsg(szSend);

	char szRecv[512];
	while (TRUE)
	{
		int dRet = WaitForCommand(szRecv, sizeof(szRecv), "[WCOL]");
		if (!(dRet & PRORES_SUCCESS))
			return dRet;

		bool bNullRoom, bFullRoom;
		if (CutPrefix(szRecv, "(allocated)"))
			break;
		else 
		if (bNullRoom = (CutPrefix(szRecv, "(error)201") || CutPrefix(szRecv, "(error)203"))
			|| (bFullRoom = CutPrefix(szRecv, "(error)202")))
		{
			if (bNullRoom)
			{
				clear();
				wstring wsText = L"该 房 间 不 存 在 ";
				mvaddwstr(2, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
				refresh();
				kbgetch();
			}
			else if (bFullRoom)
			{
				clear();
				wstring wsText = L"房 间 已 满 ";
				mvaddwstr(2, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
				refresh();
				kbgetch();
			}

			int dRet = UpdateRoomTable();
			if (dRet != PRORES_SUCCESS)
				return dRet;

			Menu_OL_Join.ClearMenu();
			Menu_OL_Join.SetTitle(L"加 入 房 间 ");
			for (int i = 0; i < nJoinableRoom; i++)
			{
				Menu_OL_Join.AddItem(IntToStringW(roomid[i]).c_str(), L"", NULL);
				Menu_OL_Join.SetEnterFunc(Menu_OL_Join.GetItem(i), Menu_OL_Join_EnterProc);
			}

			return PRORES_SHAREPARENT(*pInf);
		}
		else
		{
			InvalidCommand("RS_CREATED", szRecv);
			return PRORES_BACKWARD;
		}
	}


	if (MsgComp(szRecv, "015")) // 对方是 1
	{
		CHESS::SetCamp(0);
	}
	else if (MsgComp(szRecv, "014"))
	{
		CHESS::SetCamp(1);
	}
	else
	{
		InvalidCommand("RS_JOINED", szRecv);
		return PRORES_BACKWARD;
	}

	for (int i = 0; i < 16; i++)
	{
		int dPieceID;
		sscanf(szRecv + i * 4, "%d", &dPieceID);
		CHESS::SetPieceID(i / 4, i % 4, dPieceID);
	}

	return PRORES_SHAREPARENT(Display_OL_Prepare);
}

void OppoQuit()
{
	clear();
	mvaddwstr(2, (CONSOLECOL - 14) / 2, L"对 方 已 退 出 房 间 ");
	refresh();
	kbgetch();
}

PROCRESULT WARCHESS::Display_OL_Prepare_Proc(INTERFACE *pInf)
{
	int dRet = SetGodMode(0, SGM_NOANIMATION, pInf);
	if (dRet != PRORES_SUCCESS)
		return dRet;

	CHESS::InitChessBoard(2, (CONSOLECOL - szVisualCB.x) / 2);
	CHESS::UpdateChessBoard(UCB_DEFAULT());
	std::atomic <bool> bThdFinished (false);
	thread thdPrepare;
	while (thdPrepare.joinable() == 0)
		thdPrepare = thread([&bThdFinished]
			{
				CHESS::Prepare_Proc(UCB_DEFAULT(), true); 
				bThdFinished = true; 
			}
		);

	wstring wsText;

	while (TRUE)
	{
		if (bThdFinished) //结束准备
		{
			string sSend = "[WCOL](prepare)";
			char pcs[5] = { 0 };
			for (int i = 8; i < 16; i++)
			{
				sprintf(pcs, "%03d_", CHESS::GetPieceID(i / 4, i % 4));
				pcs[4] = 0;
				sSend += pcs;
			}
			WCOLClnt.SendMsg(sSend.c_str());
			TerminateThread(thdPrepare.native_handle(), 0);
			thdPrepare.join();
			break;
		}
		if (WCOLClnt.HaveCommandBuff())
		{
			string sCmd = WCOLClnt.GetCommand();
			if (sCmd.find("[WCOL](error)001", 0) != -1)
			{
				OppoQuit();
				TerminateThread(thdPrepare.native_handle(), 0);
				thdPrepare.join();
				return PRORES_BACKWARD;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	mvaddwstr(szVisualCB.y, 2, L"请 等 待 另 一 方 完 成 准 备 ");
	CHESS::UpdateChessBoard(UCB_SHOWMINE);

	char szRecv[257];
	while (TRUE)
	{
		WaitForCommand(szRecv, sizeof(szRecv), "[WCOL]", false);
		if (CutPrefix(szRecv, "(prepared)"))
			break;
		else if (CutPrefix(szRecv, "(error)001"))
		{
			OppoQuit();
			return PRORES_BACKWARD;
		}
		else
		{
			InvalidCommand("RS_ONEPREPARED", szRecv);
			return PRORES_BACKWARD;
		}
	}

	for (int i = 0; i < 16; i++)
	{
		int dPieceID = -1;
		if (sscanf(szRecv + i * 4, "%d", &dPieceID) < 0)
		{
			InvalidCommand("RS_ONEPREPARED", szRecv);
			return PRORES_BACKWARD;
		}
		CHESS::SetPieceID(i / 4, i % 4, dPieceID);
	}

	if(CHESS::GetCamp() == 0)
		return PRORES_SHAREPARENT(Display_OL_Move);
	else return PRORES_SHAREPARENT(Display_OL_Wait);
}

bool NewMoveProc(MOVEPIECEINFO *pmpi)
{
	int chCursesKey;
	int dMoveRet;

	while(kbhit())
	{
		chCursesKey = kbgetch();

		if (chCursesKey == KEY_UP)
			CHESS::SelectMove(DIR_UP);
		if (chCursesKey == KEY_DOWN)
			CHESS::SelectMove(DIR_DOWN);
		if (chCursesKey == KEY_LEFT)
			CHESS::SelectMove(DIR_LEFT);
		if (chCursesKey == KEY_RIGHT)
			CHESS::SelectMove(DIR_RIGHT);
		if (chCursesKey == KEY_ENTER)
		{
			pmpi->ymv = CHESS::GetSelectedPos().y, pmpi->xmv = CHESS::GetSelectedPos().x;
			pmpi->ydes = CHESS::GetSelectingPos().y, pmpi->xdes = CHESS::GetSelectingPos().x;
			if (pmpi->ymv != -1)
			{
				//POINT pos PosAfterAction(acPrv) undone

				dMoveRet = CHESS::VisualMove(pmpi->ymv, pmpi->xmv, pmpi->ydes, pmpi->xdes, UCB_DEFAULT());
				if (dMoveRet != MP_ERROR)
					return true;
			}
			if (CHESS::GetPieceOwner(pmpi->ydes, pmpi->xdes) == OWNER_ALLY)
			{
				CHESS::SelectConfirm();
			}
		}

		CHESS::UpdateChessBoard(UCB_DEFAULT());
	}
	return false;
}

PROCRESULT WARCHESS::Display_OL_Move_Proc(INTERFACE *pInf)
{
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	mvaddwstr(szVisualCB.y, 2, L"现 在 是 你 的 回 合 ");
	CHESS::SelectingShow(true);
	CHESS::UpdateChessBoard(UCB_DEFAULT());
	MOVEPIECEINFO mpi;

	/*std::atomic <bool> bThdFinished(false);
	thread thdMove;
	while(thdMove.joinable() == 0)
		thdMove = thread([&bThdFinished, &mpi]
			{
				CHESS::Move_Proc(&mpi, UCB_DEFAULT());
				bThdFinished = true;
			}
		);
*/
	while (TRUE)
	{
		if (NewMoveProc(&mpi)) // 结束移动
		{
			CHESS::SelectingShow(false);
			CHESS::SelectedCancel();

			char szSend[50] ;
			sprintf(szSend, "[WCOL](move)%03d_%03d_%03d_%03d", mpi.ymv, mpi.xmv, mpi.ydes, mpi.xdes);
			WCOLClnt.SendMsg(szSend);
			/*TerminateThread(thdMove.native_handle(), 0);
			if (thdMove.joinable())
				thdMove.join();*/
			break;
		}
		if (WCOLClnt.HaveCommandBuff())
		{
			string sCmd = WCOLClnt.GetCommand();
			if (sCmd.find("[WCOL](error)001", 0) != -1)
			{
				OppoQuit();
				/*TerminateThread(thdMove.native_handle(), 0);
				thdMove.join();*/
				return PRORES_BACKWARD;
			}
			else
			{
				InvalidCommand("RS_GAMING", sCmd.c_str());
				/*TerminateThread(thdMove.native_handle(), 0);
				thdMove.join();*/
				return PRORES_BACKWARD;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	int dIsGameOver = CHESS::IsGameOver();
	if (dIsGameOver)
	{
		WCOLClnt.SendMsg("[WCOL](terminal)");
	}
	switch (dIsGameOver)
	{
	case 1:
		return PRORES_SHAREPARENT(Display_Win);
	case -1:
		return PRORES_SHAREPARENT(Display_Lose);
	case 2:
		return PRORES_SHAREPARENT(Display_Tie);
	default:
		return PRORES_SHAREPARENT(Display_OL_Wait);
	}
}

PROCRESULT WARCHESS::Display_OL_Wait_Proc(INTERFACE *pInf)
{
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	mvaddwstr(szVisualCB.y, 2, L"现 在 是 对 方 的 回 合 ");
	CHESS::SelectingShow(false);
	CHESS::UpdateChessBoard(UCB_DEFAULT());

	char szRecv[257] = { 0 };
	while (TRUE)
	{
		WaitForCommand(szRecv, sizeof(szRecv), "[WCOL]", false);
		if (CutPrefix(szRecv, "(opmove)"))
			break;
		else if (CutPrefix(szRecv, "(error)001"))
		{
			OppoQuit();
			return PRORES_BACKWARD;
		}
		else
		{
			InvalidCommand("RS_GAMING", szRecv);
			return PRORES_BACKWARD;
		}
	} 

	int ymv, xmv, ydes, xdes;
	if (sscanf(szRecv, "%d_%d_%d_%d", &ymv, &xmv, &ydes, &xdes) < 0)
	{
		InvalidCommand("RS_GAMING", szRecv);
		return PRORES_BACKWARD;
	}
	ymv = CHESS::CoordConvert(ymv);
	xmv = CHESS::CoordConvert(xmv);
	ydes = CHESS::CoordConvert(ydes);
	xdes = CHESS::CoordConvert(xdes);
	CHESS::VisualMove(ymv, xmv, ydes, xdes, UCB_DEFAULT());

	int dIsGameOver = CHESS::IsGameOver();
	if (dIsGameOver)
	{
		WCOLClnt.SendMsg("[WCOL](terminal)");
	}
	switch (dIsGameOver)
	{
	case 1:
		return PRORES_SHAREPARENT(Display_Win);
	case -1:
		return PRORES_SHAREPARENT(Display_Lose);
	case 2:
		return PRORES_SHAREPARENT(Display_Tie);
	default:
		return PRORES_SHAREPARENT(Display_OL_Move);
	}
}


/*---------- local multi-player proc ----------*/
PROCRESULT WARCHESS::Display_LM_Create_Proc(INTERFACE *pInf)
{
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	wstring wsText = L"下 面 是 你 的 房 间 号 ： ";
	mvaddwstr(1, (CONSOLECOL - wsText.length()) / 2, wsText.c_str());
	string sText = EncryptIP(GetLocalIPAddress());
	mvaddstr(2, (CONSOLECOL - sText.length()) / 2, sText.c_str());
	wsText = L"在 等 待 玩 家 加 入 ...";
	mvaddwstr(5, 3, wsText.c_str());
	refresh();

	if (network.InitNetWork(NETWORKTYPE_SERVER) == 0)
	{
		clear();
		attron(COLOR_PAIR(COLORINDEX_ERRORCODE));
		printw("Network error, please try again.");
		refresh();
		while (GetKeyState(VK_RETURN) < 0);
		kbgetch();
		return PRORES_CONVERTINF(*INTERFACE::GetStartInterface());
	}

	char szMessage[100];
	wstring sMsg;
	network.WaitForClient();

	clear();
	wsText = L"正在加入房间...";
	mvaddwstr(1, 3, wsText.c_str());
	refresh();

	if (network.SendMsg("@wclm:cr") == 0)
	{
		mvprintw(1, 3, "SendMsg error");
		refresh();
		clear();
	}

	do
	{
		if (network.ReceiveMsg(szMessage) == 0)
		{
			mvprintw(1, 3, "ReceiveMsg error");
			refresh();
			clear();
		}
	} while (!MsgComp(szMessage, "@wclm"));

	if (!MsgComp(szMessage, "@wclm:jr"))
	{
		mvprintw(1, 3, "get message from client error: %s", szMessage);
		refresh();
		clear();
		kbgetch();
		return PRORES_ERROR;
	}

	srand((short)time(0));
	CHESS::SetCamp((rand() % 100 < 50));
	if (CHESS::GetCamp() == 0)
	{
		network.SendMsg("@wclm:camp0");
	}
	else
	{
		network.SendMsg("@wclm:camp1");
	}

	do
	{
		if (network.ReceiveMsg(szMessage) == 0)
		{
			mvprintw(1, 3, "ReceiveMsg error");
			refresh();
			clear();
		}
	} while (!MsgComp(szMessage, "@wclm:camp"));
	if (MsgComp(szMessage, "@wclm:camp0"))
	{
		if (CHESS::GetCamp() == 0)
		{
			MessageBox(NULL, "Both 0", "error", 0);
			return PRORES_ERRCODE(1000);
		}
	}
	else if (MsgComp(szMessage, "@wclm:camp1"))
	{
		if (CHESS::GetCamp() == 1)
		{
			MessageBox(NULL, "Both 1", "error", 0);
			return PRORES_ERRCODE(1001);
		}
	}

	CHESS::RandomizePieces();
	memset(szMessage, 0, sizeof(szMessage));
	for (int i = 0; i < 16; i++)
	{
		sprintf(szMessage + i * 3, "%02d_", CHESS::GetPieceID(i / 4, i % 4));
	}
	network.SendMsg(szMessage);

	return PRORES_SHAREPARENT(Display_LM_Prepare);
}

PROCRESULT WARCHESS::Display_LM_Join_Proc(INTERFACE *pInf)
{
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	mvaddwstr(1, 3, L"请 输 入 房 间 号 ： ");
	refresh();
	string sIPAddr;
	char szIPAddrBuff[10];
	char szMessage[100];
	wstring sMsg;
	do
	{
		echo();
		scanw("%s", szIPAddrBuff);
		sIPAddr = szIPAddrBuff;
	}
	while (!DecodeIP(&sIPAddr) && (clear(), mvaddwstr(1, 3, L"房 间 号 输 入 错 误 ， 请 重 新 输 入 : "), refresh(), 1));

	noecho();

	clear();
	mvaddwstr(1, 3, L"正 在 加 入 房 间 ...");
	refresh();
	
	network.InitNetWork(NETWORKTYPE_CLIENT);
	network.Connect(sIPAddr.c_str());

	do
	{
		if (network.ReceiveMsg(szMessage) == 0)
		{
			mvprintw(1, 3, "ReceiveMsg error");
			refresh();
			clear();
		}
	} while (!MsgComp(szMessage, "@wclm"));

	if (!MsgComp(szMessage, "@wclm:cr"))
	{
		mvprintw(1, 3, "get message from server error: %s", szMessage);
		refresh();
		clear();
		kbgetch();
		return PRORES_ERROR;
	}
	network.SendMsg("@wclm:jr");

	do
	{
		if (network.ReceiveMsg(szMessage) == 0)
		{
			mvprintw(1, 3, "ReceiveMsg error");
			refresh();
			clear();
		}
	} while (!MsgComp(szMessage, "@wclm:camp"));

	if (MsgComp(szMessage, "@wclm:camp0"))
	{
		CHESS::SetCamp(1);
		network.SendMsg("@wclm:camp1");
	}
	else if (MsgComp(szMessage, "@wclm:camp1"))
	{
		CHESS::SetCamp(0);
		network.SendMsg("@wclm:camp0");
	}
	else
		return PRORES_ERROR;

	string sPieceID;
	network.ReceiveMsg(szMessage);
	for (int i = 0; i < 16; i ++) 
	{
		sPieceID = szMessage + i * 3;
		sPieceID = sPieceID.substr(0, 2);
		CHESS::SetPieceID(3 - (i / 4), 3 - (i % 4), StringToInt(sPieceID));
	}

	return PRORES_SHAREPARENT(Display_LM_Prepare);
}

PROCRESULT WARCHESS::Display_LM_Prepare_Proc(INTERFACE* pInf)
{
	CHESS::InitChessBoard(2, (CONSOLECOL - szVisualCB.x) / 2);
	CHESS::UpdateChessBoard(UCB_DEFAULT());
	CHESS::Prepare_Proc(UCB_DEFAULT());
	
	mvaddwstr(szVisualCB.y, 2, L"请 等 待 另 一 方 完 成 准 备 ");
	CHESS::UpdateChessBoard(UCB_SHOWMINE);

	char szMsg[100];
	if (CHESS::GetCamp() == 0)
	{
		network.SendMsg("@wclm:prp0");
		network.ReceiveMsg(szMsg);
		if (!MsgComp(szMsg, "@wclm:prp1")) 
		{
			return PRORES_ERROR;
		}

		memset(szMsg, 0, sizeof(szMsg));
		strcpy(szMsg, "@wclm:");
		for (int i = 0; i < 8; i++)
		{
			sprintf(szMsg + i * 3 + 6, "%02d_", CHESS::GetPieceID(i / 4 + 2, i % 4));
		}
		network.SendMsg(szMsg);

		do
			network.ReceiveMsg(szMsg);
		while (!MsgComp(szMsg, "@wclm"));
		string sPieceID = szMsg;
		for (int i = 0; i < 8; i++)
		{
			sPieceID = szMsg + i * 3 + 6;
			sPieceID = sPieceID.substr(0, 2);
			CHESS::SetPieceID(3 - (i / 4 + 2), 3 - (i % 4), StringToInt(sPieceID));
		}

		return PRORES_SHAREPARENT(Display_LM_Move);
	}
	else
	{
		network.ReceiveMsg(szMsg);
		if (!MsgComp(szMsg, "@wclm:prp0"))
		{
			return PRORES_ERROR;
		}
		network.SendMsg("@wclm:prp1");
		
		do
			network.ReceiveMsg(szMsg);
		while (!MsgComp(szMsg, "@wclm"));

		string sPieceID = szMsg;
		for (int i = 0; i < 8; i++)
		{
			sPieceID = szMsg + i * 3 + 6;
			sPieceID = sPieceID.substr(0, 2);
			CHESS::SetPieceID(3 - (i / 4 + 2), 3 - (i % 4), StringToInt(sPieceID));
		}

		memset(szMsg, 0, sizeof(szMsg));
		strcpy(szMsg, "@wclm:");
		for (int i = 0; i < 8; i++)
		{
			sprintf(szMsg + i * 3 + 6, "%02d_", CHESS::GetPieceID(i / 4 + 2, i % 4));
		}
		network.SendMsg(szMsg);

		return PRORES_SHAREPARENT(Display_LM_Wait);
	}
}

PROCRESULT WARCHESS::Display_LM_Move_Proc(INTERFACE *pInf)
{
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	mvaddwstr(szVisualCB.y, 2, L"现 在 是 你 的 回 合 ");
	CHESS::SelectingShow(true);
	CHESS::UpdateChessBoard(UCB_DEFAULT());
	MOVEPIECEINFO mpi;

	CHESS::Move_Proc(&mpi, UCB_DEFAULT());

	char szMsg[100];
	memset(szMsg, 0, sizeof(szMsg));
	sprintf(szMsg, "Mv_%02d_%02d_%02d_%02d", mpi.ymv, mpi.xmv, mpi.ydes, mpi.xdes);
	network.SendMsg(szMsg);

	int bIsGameOver = CHESS::IsGameOver();
	switch (bIsGameOver)
	{
	case 1:
		return PRORES_SHAREPARENT(Display_Win);
	case -1:
		return PRORES_SHAREPARENT(Display_Lose);
	case 2:
		return PRORES_SHAREPARENT(Display_Tie);
	default:
		return PRORES_SHAREPARENT(Display_LM_Wait);
	}
}

PROCRESULT WARCHESS::Display_LM_Wait_Proc(INTERFACE *pInf)
{
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	mvaddwstr(szVisualCB.y, 2, L"现 在 是 对 方 的 回 合 ");
	CHESS::SelectingShow(false);
	CHESS::UpdateChessBoard(UCB_DEFAULT());

	int ydes, xdes, ymv, xmv;
	char szMsg[100];
	network.ReceiveMsg(szMsg);
	if (!MsgComp(szMsg, "Mv"))
	{
		return PRORES_ERRCODE(pInf->GetInterfaceID());
	}

	sscanf(szMsg, "Mv_%d_%d_%d_%d", &ymv, &xmv, &ydes, &xdes);
	ymv = CHESS::CoordConvert(ymv);
	xmv = CHESS::CoordConvert(xmv);
	ydes = CHESS::CoordConvert(ydes);
	xdes = CHESS::CoordConvert(xdes);
	CHESS::VisualMove(ymv, xmv, ydes, xdes, UCB_DEFAULT());

	int dIsGameOver = CHESS::IsGameOver();
	switch (dIsGameOver)
	{
	case 1:
		return PRORES_SHAREPARENT(Display_Win);
	case -1:
		return PRORES_SHAREPARENT(Display_Lose);
	case 2:
		return PRORES_SHAREPARENT(Display_Tie);
	default:
		return PRORES_SHAREPARENT(Display_LM_Move);
	}
}


/*---------- single player proc ----------*/
int WARCHESS::dDiffMode;
#define __WARCHESS_DIFFMODE_SB 0
#define __WARCHESS_DIFFMODE_GB 1

PROCRESULT WARCHESS::Display_SP_Proc(INTERFACE *pInf)
{
	CHESS::InitChessBoard(2, (CONSOLECOL - szVisualCB.x) / 2);
	CHESS::RandomizePieces();

	/*
	Mannal Init Chessboard	
	int cb[4][4] = { {14, 10, 9, 8},{5, 4, 2, 0},{1, 3, 6, 7},{11,12,13,15} };
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			CHESS::SetPieceID(i, j, cb[i][j]);
	*/

	for (int PrepareTurn = CHESS::IsEvenCamp(); PrepareTurn <= CHESS::IsEvenCamp() + 01; PrepareTurn++)
	{
		if (PrepareTurn & 1)
		{
			CHESS::UpdateChessBoard(UCB_DEFAULT());
			int dPrepareProcRet = CHESS::Prepare_Proc(UCB_DEFAULT());
			if (!(dPrepareProcRet & PRORES_SUCCESS))
				return dPrepareProcRet;
		}
		else
		{
			attron(COLOR_PAIR(COLORINDEX_NORMAL));
			mvaddstr(szVisualCB.y, 2, "                        ");
			mvaddwstr(szVisualCB.y, 2, (L"现 在 是 " + wstring(L"AI") + L"在 调 整 棋 子 ").c_str());
			CHESS::UpdateChessBoard(UCB_DEFAULT());
			AIPrepare(szVisualCB.y + 1, 2);
		}
	}

	int dIsGameOver;
	bool bPlayersTurn = CHESS::IsEvenCamp();
	ACTION acPrvAlly, acPrvEnemy;
	loop
	{
		if (bPlayersTurn)
			CHESS::SelectingShow(true);
		CHESS::UpdateChessBoard(UCB_DEFAULT());

		attron(COLOR_PAIR(COLORINDEX_NORMAL));
		if (bPlayersTurn)
		{
			mvaddstr(szVisualCB.y, 2, "                        ");
			mvaddwstr(szVisualCB.y, 2, L"现 在 是 你 的 回 合   ");
			refresh();
			//acPrvAlly = 
				CHESS::Move_Proc(NULL, UCB_DEFAULT());
		}
		else
		{
			mvaddstr(szVisualCB.y, 2, "                        ");
			mvaddwstr(szVisualCB.y, 2, (L"现 在 是 " + wstring(L"AI") + L"的 回 合   ").c_str());
			refresh();
			acPrvEnemy = AIMove(szVisualCB.y + 1, 2, UCB_DEFAULT());
		}

		dIsGameOver = CHESS::IsGameOver();
		switch (dIsGameOver)
		{
		case 1:
			return PRORES_SHAREPARENT(Display_Win);
		case -1:
			return PRORES_SHAREPARENT(Display_Lose);
		case 2:
			return PRORES_SHAREPARENT(Display_Tie);
		}

		bPlayersTurn = !bPlayersTurn;
	}
}

void WARCHESS::AIPrepare(int yPrntProg, int xPrntProg)
{
	switch (dDiffMode)
	{
	case __WARCHESS_DIFFMODE_SB:
		Prepare_SB_AI();
		return;
	case __WARCHESS_DIFFMODE_GB:
		Prepare_GB_AI(yPrntProg, xPrntProg);
		return;
	}
}

ACTION WARCHESS::AIMove(int yPrntProg, int xPrntProg, int uShowType )
{
	switch (dDiffMode)
	{
	case __WARCHESS_DIFFMODE_SB:
		Sleep(1000);
		return Move_SB_AI(uShowType);
	case __WARCHESS_DIFFMODE_GB:
		return Move_GB_AI(yPrntProg, xPrntProg, uShowType);
	}
}

PROCRESULT WARCHESS::Display_SP_SBMode_Proc(INTERFACE *pInf)
{
	dDiffMode = __WARCHESS_DIFFMODE_SB;

	return PRORES_SHAREPARENT(Display_SP);
}

void WARCHESS::Prepare_SB_AI()
{
	for (int i = 0; i < 100; i++)
	{
		int iSel1 = rand() % 8, iSel2 = rand() % 8;
		CHESS::SwapPieces(iSel1 / 4, iSel1 % 4, iSel2 / 4, iSel2 % 4);
	}
}

ACTION WARCHESS::Move_SB_AI(int uShowType )
{
	int OWNER_AI = OWNER_ENEMY;
	int iMv = rand() % 16, _init_rand_dir;

LOOP:
	bool bHaveOtherPcs = 0;
	for (int i = 0; i < 16; i++)
		if (CHESS::GetPieceID(i / 4, i % 4) / 2 && CHESS::GetPieceOwner(i / 4, i % 4) == OWNER_AI)
			bHaveOtherPcs = 1;

	while (CHESS::GetPieceOwner(iMv / 4, iMv % 4) != OWNER_AI)
		iMv = rand() % 16;

	int yMv = iMv / 4, xMv = iMv % 4;

	_init_rand_dir = rand() % 4;
	for (int _unmod_dir = _init_rand_dir, dir = _unmod_dir; _unmod_dir < _init_rand_dir + 4; _unmod_dir++, dir = _unmod_dir % 4)
	{
		int stepmax[4] = { yMv , 3 - yMv, xMv, 3 - xMv };
		if (!stepmax[dir]) continue;
		for (int step = rand() % stepmax[dir]; step > 0; step--)
		{
			int ydes[4] = { yMv - step , yMv + step , yMv , yMv },
				xdes[4] = { xMv , xMv , xMv - step , xMv + step };

			if (CHESS::CanMovePiece(yMv, xMv, (DIRECTION)dir, step))
			{
				if (CHESS::GetPieceID(yMv, xMv) / 2 == 0 && CHESS::GetPieceOwner(ydes[dir], xdes[dir]) != OWNER_NULL && bHaveOtherPcs)
					continue;

				if(~uShowType & AIMOVE_RETURNONLY)
					CHESS::VisualMove(yMv, xMv, (DIRECTION)dir, step, uShowType);
				ACTION acRet = { yMv, xMv, (DIRECTION)dir, step };
				return acRet;
			}
		}
	}
	iMv = rand() % 16;
	goto LOOP;
}

PROCRESULT WARCHESS::Display_SP_GBMode_Proc(INTERFACE *pInf)
{
	dDiffMode = __WARCHESS_DIFFMODE_GB;

	return PRORES_SHAREPARENT(Display_SP);
}

int WARCHESS::layer;
int WARCHESS::dLayerMaxMove;
int WARCHESS::dLayerMaxPrepFir;
int WARCHESS::dLayerMaxPrepSec;

int WARCHESS::MaxValue(TCB *tcb, int prntUb, int layermax, ACTION acPrvAlly, ACTION acPrvEnemy)
{
	int dTerminal = tcb->TerminalDetect();
	switch (dTerminal)
	{
	case 1:
		return INF;
	case 2:
		return 0;
	case -1:
		return -INF;
	}
	if (layer >= layermax)
		return tcb->Assess();

	int mxvl = -INF - 1;
	char chCursesKeydownPanel = 0;

	for (int y = 0; y < 4; y++)
	{
	LOOP_Y:
		for (int x = 0; x < 4; x++)
		{
		LOOP_X:
			if (tcb->owner[y][x] != OWNER_ALLY)
				continue;
			for (int dir = 0; dir < 4; dir++)
			{
			LOOP_DIR:
				int stepm = tcb->FindFarthestAccessible(y, x, dir);
				if (stepm == 0)
					continue;

				for (int step = stepm; step > 0; step--)
				{
					//POINT posPrv = { acPrvAlly.xmv, acPrvAlly.ymv }, undone
					//	posAfterThis = PosAfterAction(MakeAction(y, x, (DIRECTION)dir, step));
					//if (posPrv.x == posAfterThis.x && posPrv.y == posAfterThis.y) // avoid endless repetition
					//	continue;

				LOOP_STEP:
					ACTION trial = { y, x, (DIRECTION)dir, step };

					TCB bkup;
					bkup.CopyFrom(tcb);

					RESULT(tcb, trial);

					if (bShowDebugPanel)
					{
						tcb->Print(5, 0);
						mvprintw(9, 0, "AI will try %d %d %s %d", y, x, DIRINTOSTR(dir), step);
						mvaddwstr(10, 0, L"AI 将 行 动 ");
						mvaddwstr(11, 0, L"上 一 步 是 ");
						printw("%d %d %s %d", acPrvAlly.ymv, acPrvAlly.xmv, DIRINTOSTR(acPrvAlly.dir), acPrvAlly.step);
						printw("%d %d %s %d", acPrvEnemy.ymv, acPrvEnemy.xmv, DIRINTOSTR(acPrvEnemy.dir), acPrvEnemy.step);
						refresh();
						kbgetch();
						mvaddwstr(9, 0, L"                                            ");
						mvaddwstr(10, 0, L"                                            ");
						mvaddwstr(11, 0, L"                                            ");
					}

					layer++; // count the depth of searching tree
					int trialvl = MinValue(tcb, mxvl, layermax, MakeAction(y, x, (DIRECTION)dir, step), acPrvEnemy);
					layer--;

					mxvl = __MAX(trialvl, mxvl);

					if (mxvl >= prntUb) // alpha-beta pruning
						return mxvl;

					if (bShowDebugPanel)
					{
						tcb->Print(5, 0);
						mvprintw(9, 0, "AI tries %d %d %s %d", y, x, DIRINTOSTR(dir), step);
						mvaddwstr(10, 0,L"AI行 动 后 :  玩 家 会 行 动 的 最 小 值 为 trial mn vl = ");
						printw("%d", trialvl);
						mvaddwstr(11, 0, L"当 前 对 方 可 能 行 动 最 小 值 的 极 大 值 为 ");
						printw("%d", mxvl);
						refresh();
						kbgetch();
						mvaddwstr(9, 0, L"                                            ");
						mvaddwstr(10, 0, L"                                                            ");
						mvaddwstr(11, 0, L"                                            ");
					}

					tcb->CopyFrom(&bkup);

					if (bShowDebugPanel && (chCursesKeydownPanel == KEY_LEFT || chCursesKeydownPanel == KEY_BACKSPACE))
					{
						step++;
						if (step <= stepm)
							goto LOOP_STEP;
						dir--;
						if (dir >= 0)
							goto LOOP_DIR;
						x--;
						if (x >= 0)
							goto LOOP_X;
						y--;
						if (y >= 0)
							goto LOOP_Y;
						dir++;
						x++;
						y++;
					}
				}
			}
		}
	}

	return mxvl;
}

int WARCHESS::MinValue(TCB *tcb, int prntLb, int layermax, ACTION acPrvAlly, ACTION acPrvEnemy)
{
	int dTerminal = tcb->TerminalDetect();
	switch (dTerminal)
	{
	case 1:
		return INF;
	case 2:
		return 0;
	case -1:
		return -INF;
	}
	if (layer >= layermax)
		return tcb->Assess();

	int mnvl = INF;
	char chCursesKeydownPanel = 0;

	for (int y = 0; y < 4; y++)
	{
	LOOP_Y:
		for (int x = 0; x < 4; x++)
		{
		LOOP_X:
			if (tcb->owner[y][x] != OWNER_ENEMY)
				continue;
			for (int dir = 0; dir < 4; dir++)
			{
				int stepm = tcb->FindFarthestAccessible(y, x, dir);
				if (stepm == 0)
					continue;
			LOOP_DIR:
				for (int step = stepm; step > 0; step--)
				{
					//POINT posPrv = { acPrvEnemy.xmv, acPrvEnemy.ymv }, undone
					//	posAfterThis = PosAfterAction(MakeAction(y, x, (DIRECTION)dir, step));
					//if (posPrv.x == posAfterThis.x && posPrv.y == posAfterThis.y) // avoid endless repetition
					//	continue;

				LOOP_STEP:
					ACTION trial = { y, x, (DIRECTION)dir, step };

					TCB bkup;
					bkup.CopyFrom(tcb);

					RESULT(tcb, trial);

					if (bShowDebugPanel)
					{
						tcb->Print(5, 0);
						mvaddwstr(9, 0, L"玩 家 将 行 动 ");
						mvaddwstr(10, 0, L"玩 家 将 要 走 ");
						printw("%d %d %s %d", y, x, DIRINTOSTR(dir), step);
						mvaddwstr(11, 0, L"上 一 步 是 ");
						printw("%d %d %s %d", acPrvAlly.ymv, acPrvAlly.xmv, DIRINTOSTR(acPrvAlly.dir), acPrvAlly.step);
						printw("%d %d %s %d", acPrvEnemy.ymv, acPrvEnemy.xmv, DIRINTOSTR(acPrvEnemy.dir), acPrvEnemy.step);
						refresh();
						kbgetch();
						mvaddwstr(10, 0, L"                                            ");
						mvaddwstr(11, 0, L"                                            ");
					}

					layer++; // count the depth of searching tree
					int trialvl = MaxValue(tcb, mnvl, layermax, acPrvAlly, MakeAction(y, x, (DIRECTION)dir, step));
					layer--;

					mnvl = __MIN(trialvl, mnvl);

					if (mnvl <= prntLb) // alpha-beta pruning
					{
						return mnvl;
					}

					if (bShowDebugPanel)
					{
						tcb->Print(5, 0);
						mvaddwstr(9, 0, L"玩 家 走 了 ");
						printw("%d %d %s %d", y, x, DIRINTOSTR(dir), step);
						mvaddwstr(10, 0, L"玩 家 行 动 后 :  AI 会 行 动 的 最 大 值 为 trial mx vl = ");
						printw("%d", trialvl);
						mvaddwstr(11, 0, L"当 前 A I 可 能 行 动 最 大 值 的 极 小 值 为 ");
						printw("%d", mnvl);
						refresh();
						chCursesKeydownPanel = kbgetch();
						mvaddwstr(9, 0, L"                                            ");
						mvaddwstr(10, 0, L"                                                            ");
						mvaddwstr(11, 0, L"                                            ");
					}

					tcb->CopyFrom(&bkup);

					if (bShowDebugPanel && (chCursesKeydownPanel == KEY_LEFT || chCursesKeydownPanel == KEY_BACKSPACE))
					{
						step++;
						if (step <= stepm)
							goto LOOP_STEP;
						dir--;
						if (dir >= 0)
							goto LOOP_DIR;
						x--;
						if (x >= 0)
							goto LOOP_X;
						y--;
						if (y >= 0)
							goto LOOP_Y;
						dir++;
						x++;
						y++;
					}
				}
			}
		}
	}

	return mnvl;
}

int fact(int x)
{
	int sum = 1;
	while(x)
		sum *= x --;
	return sum;
}

int WARCHESS::EnemyPrep(TCB *tcbret, int prntLb)
{
	int AllyPcs[8], mxvl = -INF;
	bool bIsEvenCamp = CHESS::IsEvenCamp();
	for (int i = 0; i < 8; i++)
		AllyPcs[i] = tcbret->pieces[i / 4 + 2][i % 4];

	sort(AllyPcs, AllyPcs + 8);

	TCB tcb;
	tcb.CopyFrom(tcbret);
	for (int i = 0; i < 16; i++)
		tcb.owner[i / 4][i % 4] = (i < 8 ? OWNER_ENEMY : OWNER_ALLY);

	do
	{
		for (int i = 0; i < 8; i++)
			tcb.pieces[i / 4 + 2][i % 4] = AllyPcs[i];

		int trialval = MinValue(&tcb, -INF, dLayerMaxPrepFir, MakeAction(-1, -1, (DIRECTION)0, 0), MakeAction(-1, -1, (DIRECTION)0, 0));
		if (trialval > mxvl)
		{
			for (int i = 0; i < 8; i++)
				tcbret->pieces[i / 4 + 2][i % 4] = AllyPcs[i];
			mxvl = trialval;
		}

		if (prntLb >= -mxvl)
			return -mxvl;

	} while (next_permutation(AllyPcs, AllyPcs + 8));

	return -mxvl;

}

void WARCHESS::Prepare_GB_AI(int yPrintProg, int xPrintProg)
{
	bool bPanelOn = bShowDebugPanel;
	bShowDebugPanel = 0;
	int ProgressMax = 4 * 2 * fact(6), dProgress = 0;
	int AllyPcs[8], BestArrange[8], mxvl = -INF;
	bool bIsEvenCamp = !CHESS::IsEvenCamp();
	for (int i = 0; i < 8; i++)
		AllyPcs[i] = CHESS::GetPieceID(i / 4, i % 4);

	sort(AllyPcs, AllyPcs + 8);
		
	TCB tcb;
	for (int i = 0; i < 8; i++)
		tcb.owner[i / 4][i % 4] = OWNER_ALLY;
	for (int i = 8; i < 16; i++)
	{
		tcb.pieces[i / 4][i % 4] = CHESS::GetPieceID(i / 4, i % 4);
		tcb.owner[i / 4][i % 4] = OWNER_ENEMY;
	}
	do
	{
		int posCore;
		for (int i = 0; i < 8; i++)
			if (AllyPcs[i] == (int)CHESS::IsEvenCamp())
				posCore = i;
		if (posCore > 3) // 不让军旗在前排
			break;

		attron(COLOR_PAIR(COLORINDEX_SELECTITEM) | A_BOLD);
		if(dProgress < ProgressMax)
			dProgress++;
		mvprintw(yPrintProg, xPrintProg, "Progress : %.1f%%", (float)dProgress * 100 / ProgressMax);
		attroff(COLOR_PAIR(COLORINDEX_SELECTITEM) | A_BOLD);
		if (dProgress == ProgressMax || rand() % 3 == 0)
			refresh();

		if((AllyPcs[posCore + 4] & 1) == CHESS::IsEvenCamp() && AllyPcs[posCore + 4] / 2 != 7)
			continue;

		for (int i = 0; i < 8; i++)
			tcb.pieces[i / 4][i % 4] = AllyPcs[i];

		if (bIsEvenCamp)
		{
			int trialval = EnemyPrep(&tcb, mxvl);
			if (trialval > mxvl)
			{
				for (int i = 0; i < 8; i++)
					BestArrange[i] = AllyPcs[i];
				mxvl = trialval;
			}
		}
		else
		{
			int trialval = MinValue(&tcb, -INF, dLayerMaxPrepSec, MakeAction(-1, -1, (DIRECTION)0, 0), MakeAction(-1, -1, (DIRECTION)0, 0));
			if (trialval > mxvl)
			{
				for (int i = 0; i < 8; i++)
					BestArrange[i] = AllyPcs[i];
				mxvl = trialval;
			}
		}
	} while (next_permutation(AllyPcs, AllyPcs + 8));

	for (int i = 0; i < 8; i++)
		CHESS::SetPieceID(i / 4, i % 4, BestArrange[i]);

	mvaddstr(yPrintProg, xPrintProg, "                    ");
	bShowDebugPanel = bPanelOn;
}

ACTION WARCHESS::Move_GB_AI(int yPrintProgress, int xPrintProgress, int uShowType)
{
	TempChessBoard tcb;
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			tcb.pieces[y][x] = CHESS::GetPieceID(y, x);
			tcb.owner[y][x] = CHESS::GetPieceOwner(y, x);
			tcb.owner[y][x] = CONVERTOWNER(tcb.owner[y][x]);
		}
	}
	
	if (bShowDebugPanel)
	{
		tcb.Print(0, 0);
		addstr(" prev state");
	}

	ACTION acBest = Minimax_Decision(&tcb, yPrintProgress, xPrintProgress);
	if (__SQR(acBest.xmv + 1) + __SQR(acBest.ymv + 1) == 0) // xmv == ymv == -1
	{
		////MessageBox(NULL, "no more steps!", "error", MB_ICONHAND);
		//FILE *ferror = fopen(ERRORLOGPATH, "a+");
		//SYSTEMTIME st;
		//GetLocalTime(&st);
		//fprintf(ferror, "%04d %02d %02d %02d %02d %02d\n\t", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		FILE *ferror;
		ErrorLogOn(ferror);
		fprintf(ferror, "NOMORESTEPS");
		for (int i = 0; i < 16; i++)
		{
			fprintf(ferror, "_%d%03d", int(tcb.owner[i / 4][i % 4] == OWNER_ALLY), tcb.pieces[i / 4][i % 4]);
		}
		ErrorLogOff(ferror);

		acBest = Move_SB_AI(AIMOVE_RETURNONLY);
	}

	if(~uShowType & AIMOVE_RETURNONLY)
		CHESS::VisualMove(acBest.ymv, acBest.xmv, (DIRECTION)acBest.dir, acBest.step, uShowType);

	if (bShowDebugPanel)
	{
		mvprintw(4, 0, "action : %d %d %s %d", acBest.ymv, acBest.xmv, DIRINTOSTR((int)acBest.dir), acBest.step);
	}
	mvaddstr(yPrintProgress, xPrintProgress, "                    ");

	ACTION acRet = { acBest.ymv, acBest.xmv, (DIRECTION)acBest.dir, acBest.step };
	return acRet;
}

/*---------- none player proc ----------*/

PROCRESULT _DealWithKeyDown_ML(int chKeyCurses, bool &bAuto)
{
	if (chKeyCurses == CTL_ENTER)
		bAuto = !bAuto;
	if (chKeyCurses == KEY_ESCAPE)
		return PRORES_STARTMENU;
	if (chKeyCurses == KEY_BACKSPACE)
		return PRORES_BACKWARD;
	return PRORES_SUCCESS;
}

PROCRESULT WARCHESS::Display_ML_Proc(INTERFACE *pInf)
{
	CHESS::InitChessBoard(2, (CONSOLECOL - szVisualCB.x) / 2);
	CHESS::RandomizePieces();
	CHESS::SelectingShow(false);
	int dKeyDownRet = 0;
	bool bAuto = 0;

	for (int PrepareTurn = CHESS::IsEvenCamp(); PrepareTurn <= CHESS::IsEvenCamp() + 01; PrepareTurn++)
	{
		attron(COLOR_PAIR(COLORINDEX_NORMAL));
		mvaddstr(szVisualCB.y, 2, "                        ");

		if (PrepareTurn & 1)
		{
			mvaddwstr(szVisualCB.y, 2, (L"现 在 是 " + wstring(L"AI A") + L"在 调 整 棋 子 ").c_str());
			CHESS::UpdateChessBoard(UCB_SHOWBOTH);
			AIPrepare(szVisualCB.y + 1, 2);
		}
		else
		{
			mvaddwstr(szVisualCB.y, 2, (L"现 在 是 " + wstring(L"AI B") + L"在 调 整 棋 子 ").c_str());
			CHESS::UpdateChessBoard(UCB_SHOWBOTH);
			CHESS::TurnAroundChessBoard();
			AIPrepare(szVisualCB.y + 1, 2);
			CHESS::TurnAroundChessBoard();
		}

		CHESS::UpdateChessBoard(UCB_SHOWBOTH);
		mvaddstr(szVisualCB.y, 2, "                    ");
		if (!bAuto)
			mvaddwstr(szVisualCB.y, 2, L"按 任 意 键 以 继 续 ...");
		refresh();

		while (kbhit())
		{
			dKeyDownRet = _DealWithKeyDown_ML(kbgetch(), bAuto);
			if (!(dKeyDownRet & PRORES_SUCCESS))
				return dKeyDownRet;
		}
		if (!bAuto)
		{
			dKeyDownRet = _DealWithKeyDown_ML(kbgetch(), bAuto);
			if (!(dKeyDownRet & PRORES_SUCCESS))
				return dKeyDownRet;
		}
		else
			Sleep(500);
		mvaddstr(szVisualCB.y, 2, "                    ");
	}

	int dIsGameOver;
	bool bPlayersTurn = CHESS::IsEvenCamp();
	loop
	{
		CHESS::UpdateChessBoard(UCB_SHOWBOTH);

		attron(COLOR_PAIR(COLORINDEX_NORMAL));
		if (bPlayersTurn)
		{
			mvaddstr(szVisualCB.y, 2, "                        ");
			mvaddwstr(szVisualCB.y, 2, (L"现 在 是 " + wstring(L"AI A") + L"的 回 合   ").c_str());
			refresh();
			AIMove(szVisualCB.y + 1, 2, UCB_SHOWBOTH);
		}
		else
		{
			mvaddstr(szVisualCB.y, 2, "                        ");
			mvaddwstr(szVisualCB.y, 2, (L"现 在 是 " + wstring(L"AI B") + L"的 回 合   ").c_str());
			refresh();

			CHESS::TurnAroundChessBoard();
			ACTION actionB = AIMove(szVisualCB.y + 1, 2, AIMOVE_RETURNONLY);
			CHESS::TurnAroundChessBoard();
			actionB.xmv = CHESS::CoordConvert(actionB.xmv);
			actionB.ymv = CHESS::CoordConvert(actionB.ymv);
			actionB.dir = CHESS::DirConvert(actionB.dir);

			CHESS::VisualMove(actionB.ymv, actionB.xmv, (DIRECTION)actionB.dir, actionB.step, UCB_SHOWBOTH);
		}
		mvaddstr(szVisualCB.y, 2, "                    ");
		if (!bAuto)
			mvaddwstr(szVisualCB.y, 2, L"按 任 意 键 以 继 续 ...");
		refresh();
		while (kbhit())
		{
			dKeyDownRet = _DealWithKeyDown_ML(kbgetch(), bAuto);
			if (dKeyDownRet ^ PRORES_SUCCESS)
				return dKeyDownRet;
		}
		if (!bAuto)
		{
			dKeyDownRet = _DealWithKeyDown_ML(kbgetch(), bAuto);
			if (dKeyDownRet ^ PRORES_SUCCESS)
				return dKeyDownRet;
		}
		else
			Sleep(500);
		mvaddstr(szVisualCB.y, 2, "                    ");

		dIsGameOver = CHESS::IsGameOver();
		switch (dIsGameOver)
		{
		case 1:
			return PRORES_CONVERTINF(Display_Win) | PRORESMSK_NOCLEAR;
		case -1:
			return PRORES_CONVERTINF(Display_Lose) | PRORESMSK_NOCLEAR;
		case 2:
			return PRORES_CONVERTINF(Display_Tie) | PRORESMSK_NOCLEAR;
		}

		bPlayersTurn = !bPlayersTurn;
	}
}

PROCRESULT WARCHESS::Display_ML_SBMode_Proc(INTERFACE *pInf)
{
	dDiffMode = __WARCHESS_DIFFMODE_SB;

	return PRORES_SHAREPARENT(Display_ML);
}

PROCRESULT WARCHESS::Display_ML_GBMode_Proc(INTERFACE *pInf)
{
	dDiffMode = __WARCHESS_DIFFMODE_GB;

	return PRORES_SHAREPARENT(Display_ML);
}

PROCRESULT WARCHESS::Menu_Settings_AddtionalProc(INTERFACE *pInf, int chKey)
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
			if (CHESS::AllocMode > 0)
				CHESS::AllocMode--;
			else return PRORES_SUCCESS;
		}
		if (chKey == KEY_RIGHT)
		{
			if (CHESS::AllocMode < 4 - 1)
				CHESS::AllocMode++;
			else return PRORES_SUCCESS;
		}
		pmiSel->SetName((L"棋 子 分 配 ： " + CHESS::AllocModeDesc[CHESS::AllocMode]).c_str());
		pInf->SetSelect(iSel);
		break;
	}
	case 1:
	{
		if (chKey == KEY_LEFT)
		{
			if (WARCHESS::dLayerMaxMove > 1)
				WARCHESS::dLayerMaxMove--;
			else return PRORES_SUCCESS;
		}
		if (chKey == KEY_RIGHT)
		{
			if (WARCHESS::dLayerMaxMove < 8)
				WARCHESS::dLayerMaxMove++;
			else return PRORES_SUCCESS;
		}

		switch (dLayerMaxMove)
		{
		case 1:
		case 2:
			dLayerMaxPrepFir = 1;
			dLayerMaxPrepSec = 1;
			break;
		case 3:
			dLayerMaxPrepFir = 1;
			dLayerMaxPrepSec = 2;
			break;
		case 4:
			dLayerMaxPrepFir = 1;
			dLayerMaxPrepSec = 3;
			break;
		case 5:
			dLayerMaxPrepFir = 2;
			dLayerMaxPrepSec = 4;
			break;
		case 6:
		case 7:
		case 8:
			dLayerMaxPrepFir = 2;
			dLayerMaxPrepSec = 5;
			break;
		}
		pmiSel->SetName((L"AI思 考 时 间 限 制 ： " + IntToStringW(WARCHESS::dLayerMaxMove)).c_str());
		pInf->SetSelect(iSel);
		break;
	}
	}
	return PRORES_SUCCESS;
}
/*
PROCRESULT WARCHESS::Menu_Settings_EnterProc(INTERFACE *pInf)
{
	int iSelect = pInf->GetSelect();
	MENUITEM *pmiSelect = pInf->GetItem(iSelect);
	if (pmiSelect->GetName().substr(0, 8) == L"棋 子 分 配 ")
	{
		pmiSelect->SetName(L"棋 子 分 配 ");
	}
	if (pmiSelect->GetName().substr(0, 15) == L"AI 思 考 时 间 限 制 ")
	{
		pmiSelect->SetName(L"AI 思 考 时 间 限 制 ");
	}
}*/