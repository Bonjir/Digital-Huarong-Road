#pragma once
#ifndef _CHESS_H_
#define _CHESS_H_

#define _CRT_SECURE_NO_WARNINGS

#include "procedures.h"
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include "curses.h"
#include "debug.h"
#include "colorindex.h"
#include "command.h"
#include "interface.h"

const struct { int x = 5, y = 3; } szVisualPiece;
const struct { int x = szVisualPiece.x * 4 + 4, y = szVisualPiece.y * 4 + 2; } szVisualCB;

enum CHESSOWNER {OWNER_NULL, OWNER_ALLY, OWNER_ENEMY};

#define CPST_BLANK 0x0
#define CPST_HIDE 0x1
#define CPST_SHOW 0x2
#define CPST_SELECTING 0x10
#define CPST_SELECTED 0x20
#define CPST_ENEMY 0x40
#define CPST_SINGLECOLOR 0x80

typedef class CHESSPIECE
{
public:
	void Printw(WINDOW *wCB, int y, int x, int bHide);
	friend int ChessComp(CHESSPIECE a, CHESSPIECE b);
	CHESSPIECE();
	int TrueID();
	void SetID(int id);
	void SetOwner(CHESSOWNER);
	friend class CHESS;

protected:
	CHESSOWNER owner;
	int id;
	static int nChesspieces;
}CP;

int ChessComp(CHESSPIECE a, CHESSPIECE b);

#define UCB_SHOWMINEONLY 0x1
#define UCB_SHOWMINE UCB_SHOWMINEONLY
#define UCB_SHOWBOTH 0x2

#define UCB_NOREFRESH 0x100

#define UCB_DEFAULT() (IsGodModeOn() ? UCB_SHOWBOTH : UCB_SHOWMINE)

#define MP_SUCCESS 1
#define MP_FAIL -1
#define MP_ERROR 0

#define ALLOCMODE_PSEUDORAND 0
#define ALLOCMODE_RAND 1
#define ALLOCMODE_EQUAL 2
#define ALLOCMODE_SMART 3

enum DIRECTION { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

struct MOVEPIECEINFO
{
	int ymv, xmv, ydes, xdes;
};

struct ACTION
{
	int ymv, xmv;
	enum DIRECTION dir;
	int step;
};

ACTION MakeAction(int y, int x, int dir, int step);
POINT PosAfterAction(ACTION ac);

class CHESS
{
public:
	static void InitChessBoard(int y, int x);
	static void DestroyChessBoard();
	static void UpdateChessBoard(int uShowType);
	static void RandomizePieces();
	static POINT GetPiecePos(int id);
	static int GetPieceID(int y, int x);
	static int GetPieceID(POINT pos);
	static CHESSOWNER GetPieceOwner(int y, int x);
	static CHESSOWNER GetPieceOwner(POINT pos);
	static void SetPieceID(int y, int x, int id);
	static bool IsEvenCamp();
	static bool GetCamp();
	static void SetCamp(bool bCamp);
	static int MovePiece(int yMv, int xMv, int yDes, int xDes);
	static int MovePiece(int yMv, int xMv, DIRECTION dir, int step);
	static bool CanMovePiece(int yMv, int xMv, int yDes, int xDes);
	static bool CanMovePiece(int yMv, int xMv, DIRECTION dir, int step);
	static int IsGameOver();
	static void SelectMove(DIRECTION dir);
	static void SelectConfirm();
	static void SelectedCancel();
	static void SelectingShow(bool bShow);
	static POINT GetSelectingPos();
	static POINT GetSelectedPos();
	static int GetBoarderSize();
	static void SwapPieces(int y1, int x1, int y2, int x2);
	static int CoordConvert(int x);
	static DIRECTION DirConvert(DIRECTION dir);
	static void TurnAroundChessBoard();

	static void VisualSwap(int y1, int x1, int y2, int x2, int uShowType);
	static int VisualMove(int yMv, int xMv, DIRECTION dir, int step, int uShowType);
	static int VisualMove(int ymv, int xmv, int ydes, int xdes, int uShowType);

	static PROCRESULT Prepare_Proc(int uShowType, bool bMultipl = false);
	static void Move_Proc(MOVEPIECEINFO *pmpi, int uShowType);

	friend class WARCHESS;

protected:
	static WINDOW *wBoard;
	static CHESSPIECE board[4][4];
	static bool bCamp;
	static POINT posSelecting;
	static POINT posSelected;
	static int dBoardSize; // undone
	static bool bSelectingNotShow;
	static POINT posPrint;
	static int AllocMode;
	static wstring AllocModeDesc[];
};

extern const int CONSOLECOL;
extern const int CONSOLELINE;
extern bool IsGodModeOn();
extern bool IsShowInRedOnly();
extern PROCRESULT SetGodMode(bool _bGodModeOn, unsigned uMsk, INTERFACE *pCurrInf);
extern PROCRESULT GodModeNoChangeColor(bool bNoChangeColor, INTERFACE *pCurrInf);
extern COMMAND commGodModeOn;
extern COMMAND commGodModeOff;

#define CONVERTOWNER(ow) (((ow) == OWNER_ALLY ? OWNER_ENEMY : (ow) == OWNER_ENEMY ? OWNER_ALLY : (ow)))
#define DIRINTOSTR(d) (d == 0 ? "up" : d == 1 ? "dn" : d == 2 ? "lf" : "rt")


#endif