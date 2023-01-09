#pragma once
#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <string>
#include "curses.h"
#include "terminal.h"
#include "command.h"

//using std::string;
using std::wstring;

#ifndef loop
#define loop while(1)
#endif

const int CONSOLECOL = 80;
const int CONSOLELINE = 20;


// 程序返回值宏
#define PRORES_ERROR		0x0
#define PRORES_SUCCESS		0x1
#define PRORES_INTERFACECONVERT	0x2
#define PRORES_BACKWARD	0x4
#define PRORES_STARTMENU 0x8

#define PRORESMSK_SHAREPARENT 0x100
#define PRORESMSK_NOCLEAR 0x200
#define PRORESMSK_NOSETPARENT 0x400

#define MAKERETURN(l, h) MAKEWPARAM(l, h)
#define PRORES_CONVERTINF(inf) MAKERETURN(PRORES_INTERFACECONVERT, (inf).GetInterfaceID())
#define PRORES_SHAREPARENT(inf) MAKERETURN(PRORESMSK_SHAREPARENT | PRORES_INTERFACECONVERT, (inf).GetInterfaceID())
#define PRORES_ERRCODE(err) MAKERETURN(PRORES_ERROR, err)

typedef int PROCRESULT, PRORES;

#undef INTERFACE
#undef interface

class MENUINTERFACE;
class DISPLAYINTERFACE;
class INTERFACE;

/*----- 类：菜单界面类 -----*/

#define NMAX_MENU_ITEMS 10
#define NMAX_INTERFACE 100

#define DESCRWIN_WIDTH 16

typedef class MENUITEM
{
public:
	MENUITEM();
	void SetDescription(wstring sDescription);
	void SetName(wstring sName);
	wstring GetName();
	wstring GetDescription();
	friend class MENUINTERFACE;
private:
	void Delete();
	wstring sName;
	wstring sDescription;
	PROCRESULT(*pfEnter)(INTERFACE *pInf);
	INTERFACE *pInfEnter;
	int iItem;
	int argu;
}MNITEM;

typedef class MENUINTERFACE
{
public:
	MENUINTERFACE();
	virtual PROCRESULT ProcFunc(int dParam);
	virtual void SetSelect(int iItem);
	virtual int GetSelect();
	virtual void SetItem(MENUITEM *pItem);
	virtual void AddItem(MENUITEM *pItem);
	virtual void AddItem(wstring sName, wstring sDescription, INTERFACE *pInfEnter);
	virtual MENUITEM *GetItem(int iItem);
	virtual MENUITEM *FindItem(wstring sName);
	virtual void DeleteItem(wstring sName);
	virtual void ClearMenu();
	virtual int GetItemNum();
	virtual void SetPreprint(void(*pf)(INTERFACE *pInf));
	virtual void SetPostprint(void(*pf)(INTERFACE *pInf));
	virtual void SetAdditionalProc(PROCRESULT(*pfProc)(INTERFACE *pInf, int chKey));
	virtual void SetEnterFunc(MENUITEM *pItem, PROCRESULT(*pfEnter)(INTERFACE *pInf));
	virtual void ShowDescription(bool bShow);
	virtual void SetTitle(wstring sTitle);

protected:
	int nItem;
	int iSelect;
	PROCRESULT(*pfAdditionalProc)(INTERFACE *pInf, int chKey);
	MNITEM ItemList[NMAX_MENU_ITEMS];
	void(*pfPreprint)(INTERFACE *pInf);
	void(*pfPostprint)(INTERFACE *pInf);
	INTERFACE *parent;
	INTERFACE *self;
	COORD posPrintList;

	bool bShowDescript;
	const int dDescWinWidth = DESCRWIN_WIDTH;
	WINDOW *wDescript;
	wstring sTitle;

}MNIF, MENU;


/*----- 类：展示界面类 -----*/

typedef class DISPLAYINTERFACE
{
public:
	virtual PROCRESULT ProcFunc(int dParam);
	virtual void SetDisplay(PROCRESULT(*pfDisplay)(INTERFACE *pInf));

protected:
	INTERFACE *parent;
	INTERFACE *self;
	PROCRESULT(*pfDisplay)(INTERFACE *pInf);

}DPIF, DISPLAY;


/*----- 类：界面类 -----*/

enum INTERFACE_TYPE {INTERFACE_MENU, INTERFACE_DISPLAY};

class INTERFACE : public MENU, public DISPLAY
{
public:
	INTERFACE(INTERFACE_TYPE type);
	INTERFACE(PROCRESULT(*pfDisplay)(INTERFACE *pInf));
	PROCRESULT ProcFunc(int dParam);
	int GetInterfaceID();
	INTERFACE_TYPE GetInterFaceType();
	INTERFACE *GetParent();
	void SetParent(INTERFACE *parent);

	static int GetInterfaceNum();
	static int GetCurrentInterfaceID();
	static INTERFACE* GetCurrentInterface();
	static INTERFACE* GetInterfaceFromID(int iInterfaceID);
	static PROCRESULT DealWithProcReturnValue(PROCRESULT dRet);
	static void SetStartInterface(INTERFACE *pInfStart);
	static INTERFACE *GetStartInterface();
	static void EndInterfaceProc();

protected:
	INTERFACE_TYPE type;
	int InterfaceID;
	INTERFACE **GetParentHandle();

	static INTERFACE *pInfStart;
	static INTERFACE *pCurrInterface;
	static int nInterface;
	static INTERFACE *InterfaceHeap[NMAX_INTERFACE];
};

#endif