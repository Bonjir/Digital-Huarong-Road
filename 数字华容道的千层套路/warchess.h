
/* Êý×ÖÕ½Æå */

#pragma once
#ifndef _WARCHESS_H_
#define _WARCHESS_H_

#include <algorithm>
#include <conio.h>
#include <atomic>
#include <chrono>
#include "network.h"
#include "interface.h"
#include "chess.h"
#include "colorindex.h"
#include "command.h"

extern struct ACTION;

/* OM: Online-Multi-Player;  LM: Local-Multi-Player;  SP: Single-Player; ML: Melon Mode (No player) */

class WARCHESS
{
public:
	static void InitWarChess();
	static void Endup();
	static INTERFACE Display_Interface;
	static PROCRESULT Display_Proc(INTERFACE *pInf);

	static INTERFACE Display_OL_Create;
	static INTERFACE Display_OL_Join;

	static INTERFACE Display_LM_Create;
	static INTERFACE Display_LM_Join;

	static INTERFACE Display_SP_SBMode;
	static INTERFACE Display_SP_GBMode;

	static INTERFACE Display_ML_SBMode;
	static INTERFACE Display_ML_GBMode;

	static int bShowDebugPanel;

	/*-- profile --*/
	static wstring brief_rule;
	static wstring rules[];

	/*-- settings --*/
	static INTERFACE Menu_Settings;

protected: 
	static INTERFACE Menu_GameMode;

	static INTERFACE Display_Win;
	static PROCRESULT Display_Win_Proc(INTERFACE *pInf);

	static INTERFACE Display_Lose;
	static PROCRESULT Display_Lose_Proc(INTERFACE *pInf);

	static INTERFACE Display_Tie;
	static PROCRESULT Display_Tie_Proc(INTERFACE *pInf);

	static PROCRESULT Menu_Settings_AddtionalProc(INTERFACE *pInf, int chKey);

	/*-- AI --*/
	struct TempChessBoard;
	typedef struct TempChessBoard TCB;
	static TCB *RESULT(TCB *tcb, ACTION ac);
	static int MinValue(TCB *tcb, int prntLb, int layermax, ACTION acPrvAlly, ACTION acPrvEnemy);
	static int MaxValue(TCB *tcb, int prntUb, int layermax, ACTION acPrvAlly, ACTION acPrvEnemy);
	static ACTION Minimax_Decision(TCB *tcb, int yPrintProgress, int xPrintProgress);
	static int EnemyPrep(TCB *tcb, int prntLb);
	static int layer;
	static int dLayerMaxMove;
	static int dLayerMaxPrepFir;
	static int dLayerMaxPrepSec;

	static int dDiffMode;
	static void AIPrepare(int yPrntProg, int xPrntProg);
	static ACTION AIMove(int yPrntProg, int xPrntProg, int uShowType);

	/*-- online multi-player --*/
	static INTERFACE Menu_OL_CreateOrJoinRoom;

	static PROCRESULT Display_OL_Create_Proc(INTERFACE *pInf);

	static PROCRESULT Display_OL_Join_Proc(INTERFACE *pInf);
	static INTERFACE Menu_OL_Join;
	static PROCRESULT Menu_OL_Join_EnterProc(INTERFACE *pInf);

	static INTERFACE Display_OL_Prepare;
	static PROCRESULT Display_OL_Prepare_Proc(INTERFACE *pInf);

	static INTERFACE Display_OL_Move;
	static PROCRESULT Display_OL_Move_Proc(INTERFACE *pInf);

	static INTERFACE Display_OL_Wait;
	static PROCRESULT Display_OL_Wait_Proc(INTERFACE *pInf);

	/*-- local multi-player --*/
	static INTERFACE Menu_LM_CreateOrJoinRoom;

	static PROCRESULT Display_LM_Create_Proc(INTERFACE *pInf);

	static PROCRESULT Display_LM_Join_Proc(INTERFACE *pInf);

	static INTERFACE Display_LM_Prepare;
	static PROCRESULT Display_LM_Prepare_Proc(INTERFACE *pInf);

	static INTERFACE Display_LM_Move;
	static PROCRESULT Display_LM_Move_Proc(INTERFACE *pInf);

	static INTERFACE Display_LM_Wait;
	static PROCRESULT Display_LM_Wait_Proc(INTERFACE *pInf);

	/*-- single player --*/
	static INTERFACE Menu_SP_DifficultySelect;

	static INTERFACE Display_SP;
	static PROCRESULT Display_SP_Proc(INTERFACE *pInf);

	static PROCRESULT Display_SP_SBMode_Proc(INTERFACE *pInf);
	static void Prepare_SB_AI();
	static ACTION Move_SB_AI(int uShowType);

	static PROCRESULT Display_SP_GBMode_Proc(INTERFACE *pInf);
	static void Prepare_GB_AI(int yPrintProgress, int xPrintProgress);
	static ACTION Move_GB_AI(int yPrintProgress, int xPrintProgress, int uShowType);

	/*-- melon mode --*/
	static INTERFACE Menu_ML_DifficultySelect;

	static INTERFACE Display_ML;
	static PROCRESULT Display_ML_Proc(INTERFACE *pInf);

	static PROCRESULT Display_ML_SBMode_Proc(INTERFACE *pInf);
	static PROCRESULT Display_ML_GBMode_Proc(INTERFACE *pInf);

};

#define INF 114514
#define CANNOTWIN_VALUE (INF / 2)// 10 DEBUG
#define AIMOVE_RETURNONLY 0xfffffff


extern bool IsGodModeOn();
extern bool IsShowInRedOnly();
extern PROCRESULT SetGodMode(bool _bGodModeOn, unsigned uMsk, INTERFACE *pCurrInf);
extern PROCRESULT GodModeNoChangeColor(bool bNoChangeColor, INTERFACE *pCurrInf);

#define IMGDAT_WIN "imgwin.imgdat"
#define IMGDAT_LOSE "imglose.imgdat"
#define IMGDAT_TIE "imgtie.imgdat"

#define RS_CREATED 1
#define RS_JOINED 2
#define RS_ONEPREPARED 3
#define RS_GAMING 4
#define RS_ONEFINISHED 5
#define RS_NULL 0

#endif