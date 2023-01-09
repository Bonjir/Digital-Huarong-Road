#include "chess.h"

ACTION MakeAction(int yMv, int xMv, int dir, int step)
{
	ACTION ac = { yMv, xMv, (DIRECTION)dir, step };
	return ac;
}

POINT PosAfterAction(ACTION ac)
{
	POINT pt = { ac.xmv, ac.ymv };
	if (ac.xmv < 0 || ac.xmv > 3 || ac.ymv < 0 || ac.ymv > 3)
	{
		pt.x = pt.y = -1;
		return pt;
	}
	int ydes[4] = { pt.y - ac.step , pt.y + ac.step , pt.y , pt.y },
		xdes[4] = { pt.x , pt.x , pt.x - ac.step , pt.x + ac.step };
	pt.x = xdes[ac.dir];
	pt.y = ydes[ac.dir];
	return pt;
}

/*----- ChessPiece Class -----*/

int CHESSPIECE::nChesspieces;

CHESSPIECE::CHESSPIECE()
{
	id = nChesspieces++;
}

void CHESSPIECE::Printw(WINDOW *wCB, int y, int x, int uShowMask)
{
	if (uShowMask & CPST_SELECTING)
	{
		wattron(wCB, A_BLINK);
	}
	if (uShowMask & CPST_SELECTED)
	{
		wattron(wCB, A_BOLD);
	}
	if (uShowMask & CPST_ENEMY)
	{
		wattron(wCB, COLOR_PAIR(COLORINDEX_CHESSBOARDENEMY));
	}

	if (this->id == -1 || (uShowMask & CPST_BLANK))
	{
		mvwprintw(wCB, y + 0, x, "      ");
		mvwprintw(wCB, y + 1, x, "      ");
		mvwprintw(wCB, y + 2, x, "      ");
	}
	else
	{
		mvwaddwstr(wCB, y + 0, x, L"┌───┐ ");
		mvwaddwstr(wCB, y + 1, x, L"│   │ ");
		mvwaddwstr(wCB, y + 2, x, L"└───┘ ");

		if ((uShowMask & CPST_ENEMY) && (uShowMask & CPST_SINGLECOLOR))
			wattron(wCB, A_REVERSE);

		if (uShowMask & CPST_SHOW)
			mvwprintw(wCB, y + 1, x + 2, this->id / 10 ? "%d" : " %d", this->id);
		if (uShowMask & CPST_HIDE)
			mvwprintw(wCB, y + 1, x + 2, " *");

		if ((uShowMask & CPST_ENEMY) && (uShowMask & CPST_SINGLECOLOR))
			wattroff(wCB, A_REVERSE);
	}
	wattroff(wCB, A_BLINK | A_BOLD | A_REVERSE);
	wattron(wCB, COLOR_PAIR(COLORINDEX_CHESSBOARD));

	return;
}

int CHESSPIECE::TrueID()
{
	return this->id / 2;
}

void CHESSPIECE::SetID(int id)
{
	this->id = id;
}

void CHESSPIECE::SetOwner(CHESSOWNER owner)
{
	this->owner = owner;
}

int ChessComp(CHESSPIECE a, CHESSPIECE b)
{
	int dTrueIDA = a.TrueID(), dTrueIDB = b.TrueID();

	if (dTrueIDA == dTrueIDB)
		return 0;

	if (dTrueIDA > dTrueIDB)
		return -1 * ChessComp(b, a);

	if (dTrueIDA == 0 && dTrueIDB == 7)
		return 1;

	return -1;
}

/*----- Chess Class -----*/

WINDOW *CHESS::wBoard;
CHESSPIECE CHESS::board[4][4];
POINT CHESS::posSelecting;
POINT CHESS::posSelected;
int CHESS::dBoardSize;
POINT CHESS::posPrint;

void CHESS::InitChessBoard(int y, int x)
{
	posPrint.y = y, posPrint.x = x;

	dBoardSize = 4;

	wBoard = subwin(stdscr, szVisualCB.y, szVisualCB.x, y, x);
	wborder(wBoard, L'┃', L'┃', L'━', L'━', L'┏', L'┓', L'┗', L'┛');
	wattron(wBoard, COLOR_PAIR(COLORINDEX_CHESSBOARD));
	wattron(stdscr, COLOR_PAIR(COLORINDEX_CHESSBOARD));
	for (int i = 0; i < szVisualCB.y; i++) // 填充空色块
	{
		for (int j = 0; j < szVisualCB.x; j++)
			mvwaddch(wBoard, i, j, ' ');
		mvwaddch(stdscr, i + y, x + szVisualCB.x, ' '); // 填充右边界色块
	}
	wattroff(stdscr, COLOR_PAIR(COLORINDEX_CHESSBOARD));

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			board[i][j].owner = (i >= 2 ? OWNER_ALLY : OWNER_ENEMY);
		}

	posSelecting.x = 0;
	posSelecting.y = 2;
	posSelected.x = posSelected.y = -1;
	bSelectingNotShow = 0;

}

void CHESS::DestroyChessBoard()
{
	wborder(wBoard, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wclear(wBoard);
	wrefresh(wBoard);
	delwin(wBoard);
}

void CHESS::UpdateChessBoard(int uShowType)
{
	wattron(wBoard, COLOR_PAIR(COLORINDEX_CHESSBOARD));
	wattron(stdscr, COLOR_PAIR(COLORINDEX_CHESSBOARD));
	for (int i = 0; i < szVisualCB.y; i++)
	{
		for (int j = 0; j < szVisualCB.x; j++)
			mvwaddch(wBoard, i, j, ' ');
		mvwaddch(stdscr, i + posPrint.y, posPrint.x + szVisualCB.x, ' ');
	}
	wattroff(stdscr, COLOR_PAIR(COLORINDEX_CHESSBOARD));

	int uMask = 0;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (posSelected.y == i && posSelected.x == j)
				uMask |= CPST_SELECTED;

			if (board[i][j].owner == OWNER_ENEMY)
				uMask |= CPST_ENEMY;

			if ((uShowType & UCB_SHOWBOTH) || board[i][j].owner == OWNER_ALLY)
				uMask |= CPST_SHOW;
			else if (uShowType & UCB_SHOWMINEONLY)
				uMask |= CPST_HIDE;

			if (IsGodModeOn() && IsShowInRedOnly())
				uMask |= CPST_SINGLECOLOR;

			board[i][j].Printw(wBoard, 1 + i * szVisualPiece.y, 2 + j * szVisualPiece.x, uMask);

			uMask = 0;
		}
	}
	if (!bSelectingNotShow) // mark selecting piece
	{
		int uMask = CPST_SELECTING;
		int ysel = posSelecting.y, xsel = posSelecting.x;

		if(posSelected.y == ysel && posSelected.x == xsel)
			uMask |= CPST_SELECTED;

		if (board[ysel][xsel].owner == OWNER_ENEMY)
			uMask |= CPST_ENEMY;

		if ((uShowType & UCB_SHOWBOTH) || board[ysel][xsel].owner == OWNER_ALLY)
			uMask |= CPST_SHOW;
		else if (uShowType & UCB_SHOWMINEONLY)
			uMask |= CPST_HIDE;

		if (IsGodModeOn() && IsShowInRedOnly())
			uMask |= CPST_SINGLECOLOR;

		board[ysel][xsel].Printw(wBoard, 1 + ysel * szVisualPiece.y, 2 + xsel * szVisualPiece.x, uMask);
	}

	wborder(wBoard, L'┃', L'┃', L'━', L'━', L'┏', L'┓', L'┗', L'┛');

	if (~uShowType & UCB_NOREFRESH)
	{
		wrefresh(wBoard);
		refresh();
	}
}

int CHESS::AllocMode;
wstring CHESS::AllocModeDesc[4] = { L"随 机 ", L"平 均 ", L"分 块 ", L"智 能 " };

void CHESS::RandomizePieces()
{
	int Mine[8], Others[8];

	srand(int(time(0)));
	bCamp = (rand() % 100 < 50);
	if (IsEvenCamp())
		Mine[0] = 0, Others[0] = 1;
	else Mine[0] = 1, Others[0] = 0;

	int nMyPieces = 1, nOthersPieces = 1;

	int _dAllocModeRand = rand() % 100, _AllocMode;

#define ALLOCMODE_PSEUDORAND 0
#define ALLOCMODE_RAND 1
#define ALLOCMODE_EQUAL 2
#define ALLOCMODE_SMART 3

	// 分配模式 ： 60％概率伪随机（分块随机）20%纯随机， 20%平均
	if(CHESS::AllocMode == ALLOCMODE_SMART)
		_AllocMode = (_dAllocModeRand < 60 ? 0 : _dAllocModeRand < 80 ? 1 : 2);
	else _AllocMode = CHESS::AllocMode;

	switch (_AllocMode)
	{
	case ALLOCMODE_RAND:
	{
		for (int i = 2; i <= 13; i++)
		{
			if (nOthersPieces > 6)
			{
				Mine[nMyPieces++] = i;
				continue;
			}
			if (nMyPieces > 6)
			{
				Others[nOthersPieces++] = i;
				continue;
			}
			if (rand() % 100 < 50)
				Mine[nMyPieces++] = i;
			else
				Others[nOthersPieces++] = i;
		}
		break;
	}
	case ALLOCMODE_EQUAL:
	{
		for (int i = 2; i <= 13; i += 2)
		{
			if (rand() % 100 < 50)
				Mine[nMyPieces++] = i,
				Others[nOthersPieces++] = i + 1;
			else
				Mine[nMyPieces++] = i + 1,
				Others[nOthersPieces++] = i;
		}
		break;
	}
	case ALLOCMODE_PSEUDORAND:
	{
		for (int i = 2; i <= 7; i++)
		{
			if (nOthersPieces > 3)
			{
				Mine[nMyPieces++] = i;
				continue;
			}
			if (nMyPieces > 3)
			{
				Others[nOthersPieces++] = i;
				continue;
			}
			if (rand() % 100 < 50)
				Mine[nMyPieces++] = i;
			else
				Others[nOthersPieces++] = i;
		}
		for (int i = 8; i <= 13; i++)
		{
			if (nOthersPieces > 6)
			{
				Mine[nMyPieces++] = i;
				continue;
			}
			if (nMyPieces > 6)
			{
				Others[nOthersPieces++] = i;
				continue;
			}
			if (rand() % 100 < 50)
				Mine[nMyPieces++] = i;
			else
				Others[nOthersPieces++] = i;
		}
		break;
	}

	}
	if (IsEvenCamp())
		Mine[7] = 14, Others[7] = 15;
	else
		Mine[7] = 15, Others[7] = 14;

	for (int i = 0; i < 8; i++)
	{
		board[i / 4 + 2][i % 4].SetID(Mine[i]);
	}
	for (int i = 0; i < 8; i++)
	{
		board[3 - (i / 4 + 2)][3 - i % 4].SetID(Others[i]);
	}
}

POINT CHESS::GetPiecePos(int id)
{
	POINT p;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (board[i][j].id == id)
			{
				p.x = j;
				p.y = i;
				return p;
			}
		}
	}
	p.x = p.y = -1;
	return p;
}

void CHESS::SetCamp(bool bCamp)
{
	CHESS::bCamp = bCamp;
}

bool CHESS::IsEvenCamp()
{
	return bCamp == 0;
}
bool CHESS::GetCamp()
{
	return bCamp;
}

bool CHESS::bCamp;

int CHESS::MovePiece(int yMv, int xMv, int yDes, int xDes)
{
	if (CanMovePiece(yMv, xMv, yDes, xDes) == false)
		return MP_ERROR;

	if (board[yDes][xDes].id == -1)
	{
		board[yDes][xDes].id = board[yMv][xMv].id;
		board[yDes][xDes].owner = board[yMv][xMv].owner;
		board[yMv][xMv].id = -1;
		board[yMv][xMv].owner = OWNER_NULL;
		return MP_SUCCESS;
	}

	if ((board[yMv][xMv].id & 1) == (board[yDes][xDes].id & 1))
	{
		board[yMv][xMv].id = -1;
		board[yMv][xMv].owner = OWNER_NULL;
		return MP_FAIL;
	}
	int comp = ChessComp(board[yMv][xMv], board[yDes][xDes]);
	if (comp < 0)
	{
		board[yMv][xMv].id = -1;
		board[yMv][xMv].owner = OWNER_NULL;
		return MP_FAIL;
	}
	if (comp == 0)
	{
		board[yMv][xMv].id = board[yDes][xDes].id = -1;
		board[yMv][xMv].owner = board[yDes][xDes].owner = OWNER_NULL;
		return MP_FAIL;
	}

	if (comp > 0)
	{
		board[yDes][xDes].id = board[yMv][xMv].id;
		board[yDes][xDes].owner = board[yMv][xMv].owner;
		board[yMv][xMv].id = -1;
		board[yMv][xMv].owner = OWNER_NULL;
		return MP_SUCCESS;
	}
	return MP_SUCCESS;
}

int CHESS::MovePiece(int yMv, int xMv, DIRECTION dir, int step)
{
	int ydes[4] = { yMv - step , yMv + step , yMv , yMv },
		xdes[4] = { xMv , xMv , xMv - step , xMv + step };

	return CHESS::MovePiece(yMv, xMv, ydes[dir], xdes[dir]);
}

bool CHESS::CanMovePiece(int yMv, int xMv, int yDes, int xDes)
{
	if (yDes < 0 || xDes < 0)
		return 0;
	if ((yMv - yDes) * (xMv - xDes) != 0)
		return 0;
	if (board[yMv][xMv].id == -1 || board[yMv][xMv].owner == OWNER_NULL)
		return 0;
	for (int i = __MIN(yMv, yDes) + 1; i < __MAX(yMv, yDes); i++)
	{
		if (board[i][xMv].id != -1)
			return 0;
	}
	for (int j = __MIN(xMv, xDes) + 1; j < __MAX(xMv, xDes); j++)
	{
		if (board[yMv][j].id != -1)
			return 0;
	}
	if (board[yMv][xMv].owner == board[yDes][xDes].owner)
		return 0;
	return 1;
}

bool CHESS::CanMovePiece(int yMv, int xMv, DIRECTION dir, int step)
{
	int stepmax[4] = { yMv , 3 - yMv, xMv, 3 - xMv };
	if (step > stepmax[dir])
		return 0;

	int ydes[4] = { yMv - step , yMv + step , yMv , yMv },
		xdes[4] = { xMv , xMv , xMv - step , xMv + step };
	return CHESS::CanMovePiece(yMv, xMv, ydes[dir], xdes[dir]);
}

int CHESS::IsGameOver()
{
	int EvenAlive = 0, OddAlive = 0;
	for (int i = 0; i < dBoardSize; i++)
	{
		for (int j = 0; j < dBoardSize; j++)
		{
			if (board[i][j].id == 1) OddAlive = 1;
			if (board[i][j].id == 0) EvenAlive = 1;
		}
	}

	if (OddAlive && EvenAlive) return 0;
	if (!OddAlive && !EvenAlive) return 2;
	if (EvenAlive && IsEvenCamp() || OddAlive && !IsEvenCamp()) return 1;
	else return -1;
}

int CHESS::GetPieceID(int y, int x)
{
	return board[y][x].id;
}

int CHESS::GetPieceID(POINT pos)
{
	return board[pos.y][pos.x].id;
}

void CHESS::SetPieceID(int y, int x, int id)
{
	board[y][x].id = id;
}

void CHESS::SelectMove(DIRECTION dir)
{
	if (bSelectingNotShow)
		return;

	if (dir == DIR_UP)
		posSelecting.y--;
	if (dir == DIR_DOWN)
		posSelecting.y++;
	if (dir == DIR_LEFT)
		posSelecting.x--;
	if (dir == DIR_RIGHT)
		posSelecting.x++;

	posSelecting.y = __MAX(posSelecting.y, 0);
	posSelecting.y = __MIN(posSelecting.y, dBoardSize - 1);
	posSelecting.x = __MAX(posSelecting.x, 0);
	posSelecting.x = __MIN(posSelecting.x, dBoardSize - 1);
}

void CHESS::SelectConfirm()
{
	if (bSelectingNotShow)
		return;

	if (posSelected.x == posSelecting.x && posSelected.y == posSelecting.y)
		posSelected.x = posSelected.y = -1;
	else
	{
		posSelected = posSelecting;
	}
}

void CHESS::SelectedCancel()
{
	posSelected.x = posSelected.y = -1;
}

bool CHESS::bSelectingNotShow;

void CHESS::SelectingShow(bool bShow)
{
	bSelectingNotShow = !bShow;
}

POINT CHESS::GetSelectingPos()
{
	return posSelecting;
}

POINT CHESS::GetSelectedPos()
{
	return posSelected;
}

int CHESS::GetBoarderSize()
{
	return dBoardSize;
}

static void swap(CHESSPIECE &p1, CP &p2)
{
	CP p3 = p1;
	p1 = p2;
	p2 = p3;
}

void CHESS::SwapPieces(int y1, int x1, int y2, int x2)
{
	swap(board[y1][x1], board[y2][x2]);
}

CHESSOWNER CHESS::GetPieceOwner(int y, int x)
{
	return board[y][x].owner;
}

CHESSOWNER CHESS::GetPieceOwner(POINT pos)
{
	return board[pos.y][pos.x].owner;
}

int CHESS::CoordConvert(int x)
{
	return dBoardSize - 1 - x;
}

DIRECTION CHESS::DirConvert(DIRECTION dir)
{
	return DIRECTION(dir ^ 1);
}

void CHESS::TurnAroundChessBoard()
{
	for (int i = 0; i < 16; i++)
		CHESS::board[i / 4][i % 4].owner = CONVERTOWNER(CHESS::board[i / 4][i % 4].owner);
	for (int i = 0; i < 8; i++)
		CHESS::SwapPieces(i / 4, i % 4, 3 - i / 4, 3 - i % 4);
	CHESS::bCamp = !CHESS::bCamp;
}

PROCRESULT CHESS::Prepare_Proc(int uShowType, bool bMultipl)
{
	POINT posReadyBtn = { CHESS::posPrint.x + szVisualCB.x + 5, CHESS::posPrint.y + szVisualCB.y };

	attron(COLOR_PAIR(COLORINDEX_READYBTNFRAME));
	mvaddwstr(posReadyBtn.y - 5, posReadyBtn.x + 2, L"**********");
	mvaddwstr(posReadyBtn.y - 4, posReadyBtn.x + 2, L"*        *");
	mvaddwstr(posReadyBtn.y - 3, posReadyBtn.x + 2, L"*        *");
	mvaddwstr(posReadyBtn.y - 2, posReadyBtn.x + 2, L"*        *");
	mvaddwstr(posReadyBtn.y - 1, posReadyBtn.x + 2, L"**********");

	attron(COLOR_PAIR(COLORINDEX_READYBTNTEXT));
	mvaddwstr(posReadyBtn.y - 3, posReadyBtn.x + 5, L"Ready");
	attroff(COLOR_PAIR(COLORINDEX_READYBTNTEXT));

	wstring wsText = L"请 调 整 你 的 棋 子 布 局 ";
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	mvaddwstr(szVisualCB.y, 2, wsText.c_str());

	int chCursesKey;
	bool bFocusOnReadyBtn = 0;

	loop
	{
		chCursesKey = kbgetch();
		if (bFocusOnReadyBtn)
		{
			if (chCursesKey == KEY_ENTER)
			{
				CHESS::SelectedCancel();
				CHESS::SelectingShow(false);
				break;
			}
			if (chCursesKey == KEY_LEFT)
			{
				CHESS::SelectingShow(true);
				bFocusOnReadyBtn = 0;
			}

		}
		else
		{
			if (CHESS::GetSelectingPos().x == CHESS::GetBoarderSize() - 1 && chCursesKey == KEY_RIGHT)
			{
				CHESS::SelectingShow(false);
				bFocusOnReadyBtn = 1;
			}
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
				if (CHESS::GetSelectingPos().y >= 2)
				{
					if (CHESS::GetSelectedPos().y != -1)
					{
						CHESS::VisualSwap(CHESS::GetSelectingPos().y, CHESS::GetSelectingPos().x,
							CHESS::GetSelectedPos().y, CHESS::GetSelectedPos().x, UCB_DEFAULT());
						CHESS::SelectedCancel();
					}
					else
					{
						CHESS::SelectConfirm();
					}
				}
			}
		}

		if (bFocusOnReadyBtn)
		{
			attron(A_BLINK);
		}

		attron(COLOR_PAIR(COLORINDEX_READYBTNFRAME));
		mvaddwstr(posReadyBtn.y - 5, posReadyBtn.x + 2, L"**********");
		mvaddwstr(posReadyBtn.y - 4, posReadyBtn.x + 2, L"*        *");
		mvaddwstr(posReadyBtn.y - 3, posReadyBtn.x + 2, L"*        *");
		mvaddwstr(posReadyBtn.y - 2, posReadyBtn.x + 2, L"*        *");
		mvaddwstr(posReadyBtn.y - 1, posReadyBtn.x + 2, L"**********");

		attron(COLOR_PAIR(COLORINDEX_READYBTNTEXT));
		mvaddwstr(posReadyBtn.y - 3, posReadyBtn.x + 5, L"Ready");
		attroff(COLOR_PAIR(COLORINDEX_READYBTNTEXT));

		attroff(A_BLINK);

		if (!bMultipl)
		{
			commGodModeOn.PassFunc(chCursesKey);
			commGodModeOff.PassFunc(chCursesKey);
			commGodModeNoChangeColor.PassFunc(chCursesKey);
			uShowType = IsGodModeOn() ? UCB_SHOWBOTH : UCB_SHOWMINE;
		}
		CHESS::UpdateChessBoard(uShowType);

		if (!bMultipl)
		{
			if (chCursesKey == KEY_ESCAPE)
				return PRORES_STARTMENU;
			if (chCursesKey == KEY_BACKSPACE)
				return PRORES_BACKWARD;
		}
	}
	clear();
	return PRORES_SUCCESS;
}

void CHESS::Move_Proc(MOVEPIECEINFO *pmpi, int uShowType)
{
	int chCursesKey;
	int dMoveRet;

	bool bNeedFdBk = 1;
	if (pmpi == NULL)
	{
		bNeedFdBk = 0;
		pmpi = new MOVEPIECEINFO;
	}

	bSelectingNotShow = false;

	loop
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

				dMoveRet = CHESS::VisualMove(pmpi->ymv, pmpi->xmv, pmpi->ydes, pmpi->xdes, uShowType);
				if (dMoveRet != MP_ERROR)
					break;
			}
			if (CHESS::GetPieceOwner(pmpi->ydes, pmpi->xdes) == OWNER_ALLY)
			{
				CHESS::SelectConfirm();
			}
		}

		commGodModeOn.PassFunc(chCursesKey);
		commGodModeOff.PassFunc(chCursesKey);
		commGodModeNoChangeColor.PassFunc(chCursesKey);
		uShowType = IsGodModeOn() ? UCB_SHOWBOTH : UCB_SHOWMINE;

		CHESS::UpdateChessBoard(uShowType);
	}

	if (bNeedFdBk == 0)
		delete pmpi;

	CHESS::SelectingShow(false);
	CHESS::SelectedCancel();
}

#define VC_DURATION 500
#define VC_REFRESHDELAY 50
#define VC_FRAMERATE (1000 / VC_REFRESHDELAY) // 20
#define VC_REFRESHTIMES 10 //(VC_DURATION / VC_REFRESHDELAY) // 10 

void CHESS::VisualSwap(int y1, int x1, int y2, int x2, int uShowType)
{
	if (y1 == y2 && x1 == x2)
		return;

	CHESSPIECE cp1 = board[y1][x1], cp2 = board[y2][x2];

	CHESS::SelectingShow(false);

	board[y1][x1].id = board[y2][x2].id = -1;
	board[y1][x1].owner = board[y2][x2].owner = OWNER_NULL;

	int uMsk1 = 0, uMsk2 = 0;

	if (cp1.owner == OWNER_ENEMY) // uMsk move
		uMsk1 |= CPST_ENEMY;
	if (uShowType == UCB_SHOWBOTH || cp1.owner == OWNER_ALLY)
		uMsk1 |= CPST_SHOW;
	else if (uShowType == UCB_SHOWMINEONLY || cp1.owner == OWNER_ENEMY)
		uMsk1 |= CPST_HIDE;
	if (IsGodModeOn() && IsShowInRedOnly())
		uMsk1 |= CPST_SINGLECOLOR;

	if (cp2.owner == OWNER_ENEMY) // uMsk 2t
		uMsk2 |= CPST_ENEMY;
	if (uShowType == UCB_SHOWBOTH || cp2.owner == OWNER_ALLY)
		uMsk2 |= CPST_SHOW;
	else if (uShowType == UCB_SHOWMINEONLY || cp2.owner == OWNER_ENEMY)
		uMsk2 |= CPST_HIDE;
	if (IsGodModeOn() && IsShowInRedOnly())
		uMsk2 |= CPST_SINGLECOLOR;

	int yst1 = 1 + y1 * szVisualPiece.y, xst1 = 2 + x1 * szVisualPiece.x,
		yst2 = 1 + y2 * szVisualPiece.y, xst2 = 2 + x2 * szVisualPiece.x;

	for (int i = 0, ycur1 = 0, xcur1 = 0, ycur2 = 0, xcur2 = 0; i < VC_REFRESHTIMES / 2; i++)
	{
		if (!(yst1 + i * (y2 - y1) * szVisualPiece.y / VC_REFRESHTIMES - ycur1)
			&& !(xst1 + int(i * (x2 - x1) * szVisualPiece.x / VC_REFRESHTIMES / 2) * 2 - xcur1))
			continue;

		ycur1 = yst1 + i * (yst2 - yst1) / VC_REFRESHTIMES;
		xcur1 = xst1 + int(i * (xst2 - xst1) / VC_REFRESHTIMES / 2) * 2;
		ycur2 = yst2 + i * (yst1 - yst2) / VC_REFRESHTIMES;
		xcur2 = xst2 + int(i * (xst1 - xst2) / VC_REFRESHTIMES / 2) * 2;

		UpdateChessBoard(uShowType | UCB_NOREFRESH);

		cp1.Printw(wBoard, ycur1, xcur1, uMsk1);
		cp2.Printw(wBoard, ycur2, xcur2, uMsk2);

		wrefresh(wBoard);
		Sleep(VC_REFRESHDELAY);
	}

	board[y1][x1] = cp2;
	board[y2][x2] = cp1;

	CHESS::SelectingShow(true);
}

int CHESS::VisualMove(int ymv, int xmv, DIRECTION dir, int step, int uShowType)
{
	int yDesWithDir[4] = { ymv - step , ymv + step , ymv , ymv },
		xDesWithDir[4] = { xmv , xmv , xmv - step , xmv + step },
		ydes = yDesWithDir[dir], xdes = xDesWithDir[dir];

	if (dir < 0 || dir > 3 || step > 3)
		return MessageBox(NULL, string("In VisualMove, ymv xmv dir step = " + 
			IntToString(ymv) + " " + IntToString(xmv) + " " + IntToString(dir) + " " + IntToString(step)).c_str(),
			"ERROR", MB_ICONHAND); DEBUG
	return VisualMove(ymv, xmv, ydes, xdes, uShowType);
}

int CHESS::VisualMove(int ymv, int xmv, int ydes, int xdes, int uShowType)
{
	if (ymv > 3 || xmv > 3 || ydes > 3 || xdes > 3)
		return MessageBox(NULL, 
			string("ymv, xmv, ydes, xdes" + 
				IntToString(ymv) + " " + IntToString(xmv) + " " + IntToString(ydes) + " " + IntToString(xdes)).c_str(),
			"ERROR", MB_ICONHAND);

	CHESSPIECE cpmv = board[ymv][xmv], cpdes = board[ydes][xdes];

	int dMoveRet = MovePiece(ymv, xmv, ydes, xdes);

	if (dMoveRet == MP_ERROR)
		return MP_ERROR;

	CHESS::SelectingShow(false);

	bool bTie = (board[ydes][xdes].owner == OWNER_NULL);

	board[ymv][xmv].id = board[ydes][xdes].id = -1;
	board[ymv][xmv].owner = board[ydes][xdes].owner = OWNER_NULL;

	int uMskMv = 0, uMskDes = 0;

	if (cpdes.owner == OWNER_ENEMY) // uMsk dest
		uMskDes |= CPST_ENEMY;
	if (uShowType == UCB_SHOWBOTH || cpdes.owner == OWNER_ALLY)
		uMskDes |= CPST_SHOW;
	else if (uShowType == UCB_SHOWMINEONLY || cpdes.owner == OWNER_ENEMY)
		uMskDes |= CPST_HIDE;
	if (IsGodModeOn() && IsShowInRedOnly())
		uMskDes |= CPST_SINGLECOLOR;


	if (cpmv.owner == OWNER_ENEMY) // uMsk move
		uMskMv |= CPST_ENEMY;
	if (uShowType == UCB_SHOWBOTH || cpmv.owner == OWNER_ALLY)
		uMskMv |= CPST_SHOW;
	else if (uShowType == UCB_SHOWMINEONLY || cpmv.owner == OWNER_ENEMY)
		uMskMv |= CPST_HIDE;
	if (IsGodModeOn() && IsShowInRedOnly())
		uMskMv |= CPST_SINGLECOLOR;

	int yStart = 1 + ymv * szVisualPiece.y, xStart = 2 + xmv * szVisualPiece.x, 
		yEnd = 1 + ydes * szVisualPiece.y, xEnd = 2 + xdes * szVisualPiece.x;
	for (int i = 0, y = 0, x = 0; i <= VC_REFRESHTIMES; i ++)
	{
		if (!(yStart + i * (ydes - ymv) * szVisualPiece.y / VC_REFRESHTIMES - y) 
			&& !(xStart + int(i * (xdes - xmv) * szVisualPiece.x / VC_REFRESHTIMES / 2 ) * 2 - x))
			continue;

		y = yStart + i * (ydes - ymv) * szVisualPiece.y / VC_REFRESHTIMES;
		x = xStart + int(i * (xdes - xmv) * szVisualPiece.x / VC_REFRESHTIMES / 2) * 2;

		UpdateChessBoard(uShowType | UCB_NOREFRESH);
			
		if (dMoveRet == MP_SUCCESS)
		{
			cpdes.Printw(wBoard, 1 + ydes * szVisualPiece.y, 2 + xdes * szVisualPiece.x, uMskDes);
			cpmv.Printw(wBoard, y, x, uMskMv);
		}
		if (dMoveRet == MP_FAIL)
		{
			cpmv.Printw(wBoard, y, x, uMskMv);
			cpdes.Printw(wBoard, 1 + ydes * szVisualPiece.y, 2 + xdes * szVisualPiece.x, uMskDes);
		}
		wrefresh(wBoard);
		Sleep(VC_REFRESHDELAY);
	}

	UpdateChessBoard(uShowType | UCB_NOREFRESH);
	if (!bTie)
	{
		if (dMoveRet == MP_SUCCESS)
		{
			cpdes.Printw(wBoard, 1 + ydes * szVisualPiece.y, 2 + xdes * szVisualPiece.x, uMskDes);
			cpmv.Printw(wBoard, yEnd, xEnd, uMskMv);
		}
		if (dMoveRet == MP_FAIL)
		{
			cpmv.Printw(wBoard, yEnd, xEnd, uMskMv);
			cpdes.Printw(wBoard, 1 + ydes * szVisualPiece.y, 2 + xdes * szVisualPiece.x, uMskDes);
		}
	}
	wrefresh(wBoard);

	if (dMoveRet == MP_SUCCESS)
		board[ydes][xdes] = cpmv;
	if (dMoveRet == MP_FAIL)
	{
		if(cpmv.id / 2 - cpdes.id / 2)
			board[ydes][xdes] = cpdes;
	}

	return dMoveRet;
}
