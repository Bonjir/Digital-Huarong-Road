
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
* ������
* ������
*
* ����Сproc ��ѭ������ˢ�½���
* ����pvp ��һ �󴰿� ���� С���ڣ�findwindow�������ͨѶ
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
	static string SWP_sMenu[SMP_NMENU] = { "��ʼ", "����" };

	SetConsoleTextColor(CTC_AQUAMAINE);
	printf("\n\t");
	printf("���ֻ��ݵ���ǧ����·");
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

	puts("�����뷿����\n(�������Ϊһ���ַ�����������䲻�������Զ�����)");
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
				getch();//���Ի�ȡesc�ļ�ֵ 
				SetConsoleTextColor(CTC_LIGHTRED);
				printf("���˳�����\n");
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
	static string HP_sMenu[HP_NMENU] = { "����", "�汾��Ϣ", "����", "����" };
	static string HP_sContent[HP_NMENU] = {
"����Ϸ�������ֻ��ݵ�ħ�ģ������ֻ��ݵ�����������и��õ�����~\n\
������ҶԾ֣���Ϊ��������ż������ÿ����8�����ӣ���6��ս�����ӡ�1��ս���������ӡ�һ��ս���������ӡ�\n\
��������̶���ս����������1���Լ�ս����������15��\
ż������̶���ս����������0���Լ�ս����������14��������������������\n\
��Ϸ����\n��������Ϸ��ʼǰ�������ɰڷż������ӣ�ͬʱ��Ϸ�����в��ɲ鿴�Է�������λ�á�\
��Ϸ��ʼ�����������ƶ����ӣ���û�����������赲������¿��Ժ���������ƶ����������ø����жԷ������򽫽���һ��ս���ж���\
���ս����ʤ����Խ��Է����ӳԵ������򼺷����ӱ��Ե���\
���Է���ս���������ӻ��ܺ󼴻����Ϸʤ��\n\
...",
"������ v1.0.0",
"�����ˣ�ƿװ����",
"�ر���л��\n\
\tmyb\n\
\tct\n\
\tcct\n\
    �Լ�����Ϊ���С��Ϸ������Ȥ��ͬѧ��~"
	};
#define NTOTPAGE 3
	string sRules[3] = {
"����Ϸ�������ֻ��ݵ�ħ�ģ������ֻ��ݵ�����������и��õ�����~\n\
������ҶԾ֣���Ϊ��������ż������ÿ����8�����ӣ���6��ս�����ӡ�1��ս���������ӡ�һ��ս���������ӡ�\n\
��������̶���ս����������1���Լ�ս����������15��\
ż������̶���ս����������0���Լ�ս����������14��������������������\n",
"��Ϸ����\n    ��������Ϸ��ʼǰ�������ɰڷż������ӣ�ͬʱ��Ϸ�����в��ɲ鿴�Է�������λ�á�\
��Ϸ��ʼ�����������ƶ����ӣ���û�����������赲������¿��Ժ���������ƶ����������ø����жԷ������򽫽���һ��ս���ж���\
���ս����ʤ����Խ��Է����ӳԵ������򼺷����ӱ��Ե���\
���Է���ս���������ӻ��ܺ󼴻����Ϸʤ��\n",
"ս���ж���\n\
    �����������С1��ż�����ǵȼ۵ģ�����������ͬ���ھ���\n\
�ƶ����ӵ�һ����Ϊ������������������ƶ��������뱻������������ż��ͬ�򱻽������ӵ������ڽ������ӣ����и�����֮��ʤ��\
����ģ����һ����ս����������������һ����ս���������ӣ���ս������������ܡ�\
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
				printf("\t��Ϸ���� [page %d]\n\n    %s", iPage + 1, sRules[iPage].c_str());
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
		printf("���ֻ��ݵ���ǧ����·");
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
				//"���Դ����׸������һ��ֵĵĸ���ͬѧ�Ǻ���һ�������ĸ������¡�"
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

