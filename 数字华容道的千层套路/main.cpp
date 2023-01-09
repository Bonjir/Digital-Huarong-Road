
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <conio.h>
#include <string>
#include <cstring>
#include "chess.h"
#include "console.h"
#include "debug.cpp"

using std::string;

/*
* 棋盘类
* 棋子类
*
* 各个小proc 自循环，不刷新界面
* 单机pvp 法一 大窗口 法二 小窗口，findwindow，程序间通讯
*/

#define INTERFACE_STARTMENU	0
#define INTERFACE_JOINGAME	1
#define INTERFACE_PREPARE	2
#define INTERFACE_GAMING	3
#define INTERFACE_GAMEOVER	4
#define INTERFACE_HELP		100

#define PROCRET_ERROR		0x0
#define PROCRET_SUCCESS		0x1
#define PROCRET_INTERFACECONVERT	0x2
#define PROCRET_NOUPDATE	0x4

#define MAKERETURN(l, h) MAKEWPARAM(l, h)

typedef int PROCRESULT;

PROCRESULT MainProc(int dParam);
PROCRESULT StartMenuProc(int dParam);
PROCRESULT JoinGameProc(int dParam);
PROCRESULT PrepareProc(int dParam);
PROCRESULT GamingProc(int dParam);
PROCRESULT GameOverProc(int dParam);
PROCRESULT HelpProc(int dParam);

int main()
{
	Init();
	InitConsole();

	char chKey = 0;
	PROCRESULT dRet = 0;
	while (true)
	{
		ConsoleClear();
		dRet = MainProc(chKey);
		if (LOWORD(dRet) != PROCRET_INTERFACECONVERT)
			chKey = getch(); //
		else chKey = 0;
		Sleep(10);
	}
	return 0;
}

static unsigned uInterface;

static PROCRESULT _DealWithProcReturnValue(string sFuncName, PROCRESULT dRet)
{
	if (LOWORD(dRet) == PROCRET_ERROR)
	{
		ConsoleClear();

		SetConsoleTextColor(CTC_RED);
		printf(("Error In Func " + sFuncName + "\nwith return value %x\n").c_str(), dRet);
		SetConsoleTextColor(CTC_LIGHTWHITE);

		ConsolePause();
		exit(0);
		return dRet;
	}

	if (LOWORD(dRet) == PROCRET_SUCCESS)
		return dRet;

	if (LOWORD(dRet) == PROCRET_INTERFACECONVERT)
	{
		uInterface = HIWORD(dRet);
		return dRet;
	}
}

PROCRESULT MainProc(int dParam)
{
	switch (uInterface)
	{
	case INTERFACE_STARTMENU:
	{
		int dRet = StartMenuProc(dParam);
		return _DealWithProcReturnValue("StartMenuProc()", dRet);
	}
	case INTERFACE_JOINGAME:
	{
		int dRet = JoinGameProc(dParam);
		return _DealWithProcReturnValue("JoinGameProc()", dRet);
	}
	case INTERFACE_PREPARE:
	{
		int dRet = PrepareProc(dParam);
		return _DealWithProcReturnValue("PrepareProc()", dRet);
	}

	case INTERFACE_HELP:
	{
		int dRet = HelpProc(dParam);
		return _DealWithProcReturnValue("HelpProc()", dRet);
	}
	}
}

PROCRESULT StartMenuProc(int dParam)
{
#define SMP_NMENU 2
	static int SMP_dMenuChosen = 0;
	static string SWP_sMenu[SMP_NMENU] = { "开始", "帮助" };

	SetConsoleTextColor(CTC_AQUAMAINE);
	printf("\n\t");
	printf("数字华容道的千层套路");
	printf("\n\n");
	SetConsoleTextColor(CTC_LIGHTWHITE);

	if (dParam == KEYDOWN_UP || dParam == KEYDOWN_LEFT)
		SMP_dMenuChosen--, SMP_dMenuChosen = (SMP_dMenuChosen + SMP_NMENU) % SMP_NMENU;
	if (dParam == KEYDOWN_DOWN || dParam == KEYDOWN_RIGHT)
		SMP_dMenuChosen++, SMP_dMenuChosen %= SMP_NMENU;

	for (int i = 0; i < SMP_NMENU; i++)
	{
		if (SMP_dMenuChosen == i)
			SetConsoleTextColor(CTC_YELLOW),
			printf("\t      > ");
		else printf("\t\t");
		printf("%s\n", SWP_sMenu[i].c_str());
		if (SMP_dMenuChosen == i)
			SetConsoleTextColor(CTC_LIGHTWHITE);
	}

	if (dParam == KEYDOWN_RETURN)
	{
		if (SMP_dMenuChosen == 0)
			return MAKERETURN(PROCRET_INTERFACECONVERT, INTERFACE_JOINGAME);
		if (SMP_dMenuChosen == 1)
			return MAKERETURN(PROCRET_INTERFACECONVERT, INTERFACE_HELP);
	}


	return PROCRET_SUCCESS;
}

PROCRESULT JoinGameProc(int dParam)
{
INPUTPASSWORD:

	puts("请输入房间口令：\n(房间口令为一段字符串，如果房间不存在则将自动创建)");
	string sRoomPassword;
	char szRoomPw[100];
	scanf("%s", szRoomPw);
	sRoomPassword = szRoomPw;
	if (sRoomPassword == "q")
		return MAKERETURN(PROCRET_INTERFACECONVERT, INTERFACE_STARTMENU);

	string sDataPath = (DATAPATH + sRoomPassword + ".dat");
	if (IsPathExist(sDataPath.c_str()))
	{
		FILE *fp = fopen(sDataPath.c_str(), "r+");
		if (fp == NULL)
			goto NULLFILE;

		if (feof(fp))
		{
			fclose(fp);
			goto NULLFILE;
		}

		int nPlayers;
		fscanf(fp, "NPLAYERS: %d", &nPlayers);
		if (nPlayers == 1)
		{
			printf("Entering ...\n");
			fseek(fp, 0, SEEK_SET);
			fprintf(fp, "NPLAYERS: 2");
			fclose(fp);

			Sleep(500);
			return MAKERETURN(PROCRET_INTERFACECONVERT, INTERFACE_PREPARE);
		}
		if (nPlayers == 2)
		{
			SetConsoleTextColor(CTC_LIGHTRED);
			printf("The room is full, please try another room~\n");
			SetConsoleTextColor(CTC_LIGHTWHITE);
			fclose(fp);
			ConsolePause();
			ConsoleClear();
			goto INPUTPASSWORD;
		}
	}
	else {
	NULLFILE:

		FILE *fp = fopen(sDataPath.c_str(), "w+");
		fprintf(fp, "NPLAYERS: 1");
		printf("Waiting for another player ...\n");
		fclose(fp);

		int nPlayers = 0;
		while (nPlayers != 2)
		{
			Sleep(10);
			fp = fopen(sDataPath.c_str(), "r+");
			fscanf(fp, "NPLAYERS: %d", &nPlayers);
			fclose(fp);

			if (GetKeyState(VK_ESCAPE) < 0)
			{
				getch();//用以获取esc的键值 
				SetConsoleTextColor(CTC_LIGHTRED);
				printf("已退出房间\n");
				SetConsoleTextColor(CTC_LIGHTWHITE);
				fclose(fp);

				DeleteFile(sDataPath.c_str());

				ConsolePause();
				ConsoleClear();
				goto INPUTPASSWORD;
			}
		}
		Sleep(500);
		return MAKERETURN(PROCRET_INTERFACECONVERT, INTERFACE_PREPARE);
	}
	return PROCRET_SUCCESS;
}

PROCRESULT PrepareProc(int dParam)
{
	return 0;
}

PROCRESULT GamingProc(int dParam)
{
	return 0;
}

PROCRESULT GameOverProc(int dParam)
{
	return 0;
}

PROCRESULT HelpProc(int dParam)
{
#define HP_NMENU 4
	static int HP_dMenuChosen = 0;
	static string HP_sMenu[HP_NMENU] = { "规则", "版本信息", "作者", "其他" };
	static string HP_sContent[HP_NMENU] = {
"本游戏基于数字华容道魔改，用数字华容道进行游玩会有更好的体验~\n\
两个玩家对局，分为奇数方和偶数方，每方有8个棋子，含6个战术棋子、1个战术核心棋子、一个战略武器棋子。\n\
奇数方会固定有战术核心棋子1，以及战略武器棋子15；\
偶数方会固定有战术核心棋子0，以及战略武器棋子14；其余随机分配给两方。\n\
游戏规则：\n两方在游戏开始前可以自由摆放己方棋子，同时游戏进行中不可查看对方的棋子位置。\
游戏开始后，两方轮流移动棋子，在没有其他棋子阻挡的情况下可以横向或纵向移动任意格，如果该格上有对方棋子则将进行一次战斗判定，\
如果战斗判胜则可以将对方棋子吃掉，否则己方棋子被吃掉。\
将对方的战术核心棋子击败后即获得游戏胜利\n\
...",
"勇翔棋 v1.0.0",
"制作人：瓶装蛋糕",
"特别鸣谢：\n\
\tmyb\n\
\tct\n\
\tcct\n\
    以及所有为这个小游戏增添乐趣的同学们~"
	};
#define NTOTPAGE 3
	string sRules[3] = {
"本游戏基于数字华容道魔改，用数字华容道进行游玩会有更好的体验~\n\
两个玩家对局，分为奇数方和偶数方，每方有8个棋子，含6个战术棋子、1个战术核心棋子、一个战略武器棋子。\n\
奇数方会固定有战术核心棋子1，以及战略武器棋子15；\
偶数方会固定有战术核心棋子0，以及战略武器棋子14；其余随机分配给两方。\n",
"游戏规则：\n    两方在游戏开始前可以自由摆放己方棋子，同时游戏进行中不可查看对方的棋子位置。\
游戏开始后，两方轮流移动棋子，在没有其他棋子阻挡的情况下可以横向或纵向移动任意格，如果该格上有对方棋子则将进行一次战斗判定，\
如果战斗判胜则可以将对方棋子吃掉，否则己方棋子被吃掉。\
将对方的战术核心棋子击败后即获得游戏胜利\n",
"战斗判定：\n\
    奇数棋与比它小1的偶数棋是等价的，二者相遇会同归于尽；\n\
移动棋子的一方称为进攻方，如果进攻方移动的棋子与被进攻的棋子奇偶相同或被进攻棋子点数大于进攻棋子，则判负；反之判胜。\
特殊的，如果一方的战略武器棋子遇到另一方的战术核心棋子，则战略武器棋子落败。\
"
	};

	switch (dParam)
	{
	case KEYDOWN_BACKSPACE:
	case KEYDOWN_ESCAPE:
	{
		return MAKERETURN(PROCRET_INTERFACECONVERT, INTERFACE_STARTMENU);
	}

	case KEYDOWN_RETURN:
	{
		if (HP_dMenuChosen == 0)
		{
			int chKey = 0, iPage = 0;

			do
			{
				if (chKey && chKey ^ KEYDOWN_UP && chKey ^ KEYDOWN_LEFT && chKey ^ KEYDOWN_DOWN && chKey ^ KEYDOWN_RIGHT
					|| (chKey == KEYDOWN_UP || chKey == KEYDOWN_LEFT) && iPage == 0
					|| (chKey == KEYDOWN_DOWN || chKey == KEYDOWN_RIGHT) && iPage == NTOTPAGE - 1)
					continue;

				if (chKey == KEYDOWN_UP || chKey == KEYDOWN_LEFT)
					iPage--, iPage = max(0, iPage);
				if (chKey == KEYDOWN_DOWN || chKey == KEYDOWN_RIGHT)
					iPage++, iPage = min(iPage, NTOTPAGE - 1);

				ConsoleClear();
				printf("\t游戏规则 [page %d]\n\n    %s", iPage + 1, sRules[iPage].c_str());
				chKey = getch();
			} while ((chKey != KEYDOWN_ESCAPE && chKey != KEYDOWN_BACKSPACE));
			ConsoleClear();
		}
	}

	case KEYDOWN_UP:
	case KEYDOWN_LEFT:
	case KEYDOWN_DOWN:
	case KEYDOWN_RIGHT:
	{
		if (dParam == KEYDOWN_UP || dParam == KEYDOWN_LEFT)
			HP_dMenuChosen--, HP_dMenuChosen = max(HP_dMenuChosen, 0);
		if (dParam == KEYDOWN_DOWN || dParam == KEYDOWN_RIGHT)
			HP_dMenuChosen++, HP_dMenuChosen = min(HP_dMenuChosen, HP_NMENU - 1);

	case 0:
		SetConsoleTextColor(CTC_AQUAMAINE);
		printf("\n\t");
		printf("数字华容道的千层套路");
		printf("\n\n");
		SetConsoleTextColor(CTC_LIGHTWHITE);

		for (int i = 0; i < HP_NMENU; i++)
		{
			if (HP_dMenuChosen == i)
				SetConsoleTextColor(CTC_YELLOW),
				printf("\t      > ");
			else printf("\t\t");
			printf("%s\n", HP_sMenu[i].c_str());
			if (HP_dMenuChosen == i)
				SetConsoleTextColor(CTC_LIGHTWHITE);
		}
		printf("\n\n");
		string _showcontent = HP_sContent[HP_dMenuChosen];
		printf("    %s\n", (_showcontent.substr(0, 278) + (_showcontent.length() >= 278 ? " ..." : "")).c_str());
		if (HP_dMenuChosen == HP_NMENU - 1)
		{
			SetConsoleTextColor(CTC_LIGHTPURPLE);
			printf("\n\n");
			printf(
				"Dedicated to my high school classmates, who brought me joy, and to my most unforgettable high school life ~"
				//"谨以此作献给带给我欢乐的的高中同学们和那一段难忘的高三岁月。"
			);
			SetConsoleTextColor(CTC_LIGHTWHITE);
		}
		break;
	}
	default:
		break;
	}
	return PROCRET_SUCCESS;
}

