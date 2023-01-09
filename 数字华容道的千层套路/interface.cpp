#pragma once

#include <windows.h>
#include "interface.h"
#include "colorindex.h"
#include "debug.h"

/*----- MENUITEM struct -----*/

MENUITEM::MENUITEM()
{
	iItem = 0;
}

void MENUITEM::SetDescription(wstring sDescript)
{
	this->sDescription = sDescript;
}

void MENUITEM::SetName(wstring sName)
{
	this->sName = sName;
}

wstring MENUITEM::GetName()
{
	return this->sName;
}

void MENUITEM::Delete()
{
	this->argu = this->iItem = 0;
	this->sName = L"";
	this->sDescription = L"";
	this->pfEnter = NULL;
	this->pInfEnter = NULL;
}

wstring MENUITEM::GetDescription()
{
	return this->sDescription;
}

/*----- MENUINTERFACE class -----*/

extern COMMAND commGodModeOn;
extern COMMAND commGodModeOff;
extern COMMAND commGodModeNoChangeColor;

MENUINTERFACE::MENUINTERFACE()
{
	nItem = 0;
	iSelect = 0;
	bShowDescript = 0;
}

PROCRESULT MENUINTERFACE::ProcFunc(int dParam)
{
	if(pfPreprint != NULL)
		(*pfPreprint)(this->self); // 自定义preprint函数

	if (sTitle != L"") // 显示标题
	{
		attroff(A_BOLD);
		attron(COLOR_PAIR(COLORINDEX_STARTMENUTEXT));
		mvaddwstr(1, (CONSOLECOL - (this->bShowDescript ? DESCRWIN_WIDTH : 0) - sTitle.length()) / 2, sTitle.c_str());
		attroff(COLOR_PAIR(COLORINDEX_STARTMENUTEXT));
		move(3, (CONSOLECOL - (this->bShowDescript ? DESCRWIN_WIDTH : 0)) / 2);
	}
	getyx(stdscr, posPrintList.Y, posPrintList.X); // 记录开始输出列表的位置

	attron(COLOR_PAIR(COLORINDEX_NORMAL)); // 输出列表
	for (int i = 0; i < nItem; i++)
	{
		short xPosPrint = __MAX(2, short(posPrintList.X - ItemList[i].sName.length() / 2));
		move(posPrintList.Y + i, xPosPrint);
		addwstr(ItemList[i].sName.c_str());
	}

	if (this->bShowDescript == true) // 创建描述窗口
	{
		wDescript = subwin(stdscr, CONSOLELINE - 2, dDescWinWidth, 1, CONSOLECOL - dDescWinWidth - 2);
	}

	SetSelect(iSelect);

	if (pfPostprint != NULL)
		(*pfPostprint)(this->self); // 自定义postprint函数

	int chCurses;
	PROCRESULT dRet;

	loop
	{
		chCurses = kbgetch();

		if (chCurses == KEY_UP) // 通过按键选择item
		{
			if (iSelect > 0)
				SetSelect(iSelect - 1);
		}
		if (chCurses == KEY_DOWN)
		{
			if (iSelect < nItem - 1)
				SetSelect(iSelect + 1);
		}
		
		if (pfAdditionalProc != NULL) // 自定义 AdditionalProc 函数
		{
			dRet = (*pfAdditionalProc)(this->self, chCurses);
			if (dRet != PRORES_SUCCESS) // 返回PRORES_SUCCESS则不结束函数
				return dRet;
		}

		if (chCurses == KEY_ENTER) // 默认的enter item 过程
		{
			if (ItemList[iSelect].pfEnter != NULL) // 看起来没什么用
			{
				dRet = (*ItemList[iSelect].pfEnter)(this->self);
				if (LOWORD(dRet) != PRORES_SUCCESS)
					return dRet;
			}
			else if (ItemList[iSelect].pInfEnter != NULL)
			{
				return PRORES_CONVERTINF( *(ItemList[iSelect].pInfEnter) );
			}
		}

		if (chCurses == KEY_BACKSPACE) // 默认的后退函数
		{
			if (this->parent != NULL)
			{
				return PRORES_BACKWARD;
			}
		}

		if (chCurses == KEY_ESCAPE)
		{
			return PRORES_STARTMENU;
		}

		if (commGodModeOn.PassFunc(chCurses)
			|| commGodModeOff.PassFunc(chCurses)
			|| commGodModeNoChangeColor.PassFunc(chCurses))
			return PRORES_SHAREPARENT(*this->self);
	}

	if (this->bShowDescript == true)
	{
		delwin(wDescript);
	}
}

void MENUINTERFACE::SetAdditionalProc(PROCRESULT(*pfProc)(INTERFACE *pInf, int chKey))
{
	this->pfAdditionalProc = pfProc;
	return;
}

void MENUINTERFACE::SetPreprint(void(*pfPreprint)(INTERFACE *pInf))
{
	this->pfPreprint = pfPreprint;
	return;
}

void MENUINTERFACE::SetPostprint(void(*pfPostprint)(INTERFACE *pInf))
{
	this->pfPostprint = pfPostprint;
	return;
}

void MENUINTERFACE::SetSelect(int iItem)
{
	short xPosCenter = this->posPrintList.X; // 将要输出的位置

	// 还原原选
	COORD sizeConsole, posToErase, posToHighLight;

	sizeConsole.X = CONSOLECOL, sizeConsole.Y = CONSOLELINE;

	wstring sOriSelName = this->ItemList[this->iSelect].sName;
	posToErase.X = __MAX(short(xPosCenter - (sOriSelName.length() / 2) - 4), 0);
	posToErase.Y = this->posPrintList.Y + this->iSelect;
	move(this->posPrintList.Y + this->iSelect, posToErase.X);
	attron(COLOR_PAIR(COLORINDEX_NORMAL));
	sOriSelName = L"    " + sOriSelName + L"    ";
	addwstr(sOriSelName.c_str());
	
	// 高亮新选
	wstring sSelName = this->ItemList[iItem].sName;
	attron(COLOR_PAIR(COLORINDEX_SELECTITEM) | A_BOLD);
	posToHighLight.X = __MAX(short(xPosCenter - (sSelName.length() / 2)), 0);
	posToHighLight.Y = this->posPrintList.Y + iItem;
	move(this->posPrintList.Y + iItem, __MAX(posToHighLight.X - 2, 0));
	sSelName = L"> " + sSelName + L" <";
	addwstr(sSelName.c_str());
	attroff(COLOR_PAIR(COLORINDEX_SELECTITEM) | A_BOLD);

	// 更新 description
	if (this->bShowDescript == true)
	{
		RECT rect = { 0, 0 };
		getmaxyx(this->wDescript, rect.bottom, rect.right);
		wdeepclear(this->wDescript, rect); // 处理curses清屏残留字符
		wattron(this->wDescript, COLOR_PAIR(COLORINDEX_ALLBLACK) | A_BOLD);
		drawframe(this->wDescript, L'│', L'│', L'─', L'─', L'┌', L'┐', L'└', L'┘', true);
		wattroff(this->wDescript, COLOR_PAIR(COLORINDEX_ALLBLACK) | A_BOLD);

		wstring sDesc = this->ItemList[iItem].sDescription;
		sSelName = this->ItemList[iItem].sName;
		sSelName = sSelName.substr(0, sSelName.find(L'：'));
		int xPosPrintDes = ((int)sDesc.length() > (int)this->dDescWinWidth - 2 ? 2 : ((int)this->dDescWinWidth - sDesc.length()) / 2);
		wattron(this->wDescript, COLOR_PAIR(COLORINDEX_TEXTBLUE) | A_BOLD);
		mvwaddwstr(this->wDescript, 1, (this->dDescWinWidth - sSelName.length()) / 2, sSelName.c_str());
		wattroff(this->wDescript, COLOR_PAIR(COLORINDEX_TEXTBLUE) | A_BOLD);
		mvwaddwstr(this->wDescript, 3, xPosPrintDes, sDesc.c_str());
		wrefresh(this->wDescript);
	}

	this->iSelect = iItem;
	refresh();
	return;
}

int MENUINTERFACE::GetSelect()
{
	return this->iSelect;
}

void MENUINTERFACE::SetItem(MENUITEM *pItem)
{
	this->ItemList[pItem->iItem] = *pItem;
	return;
}

void MENUINTERFACE::AddItem(MENUITEM *pItem)
{
	this->ItemList[this->nItem] = *pItem;
	this->ItemList[this->nItem].iItem = this->nItem;
	this->nItem++;

	if (pItem->sDescription.length() != 0)
		this->bShowDescript = 1;

	return;
}

void MENUINTERFACE::AddItem(wstring sName, wstring sDescription, INTERFACE *pInfEnter)
{
	this->ItemList[this->nItem].iItem = this->nItem;
	this->ItemList[this->nItem].sName = sName;
	this->ItemList[this->nItem].sDescription = sDescription;
	this->ItemList[this->nItem].pInfEnter = pInfEnter;
	this->nItem++;

	if (sDescription.length() != 0)
		this->bShowDescript = 1;

	return;
}

//void MENUINTERFACE::AddItem(wstring sName, wstring sDescription, INTERFACE *pInfEnter, PROCRESULT(*pfEnter)(INTERFACE *pInf))
//{
//	this->ItemList[this->nItem].pfEnter = pfEnter;
//	MENUINTERFACE::AddItem(sName, sDescription, pInfEnter);
//	return;
//}

void MENUINTERFACE::SetEnterFunc(MENUITEM *pItem, PROCRESULT(*pfEnter)(INTERFACE *pInf))
{
	pItem->pfEnter = pfEnter;
	return;
}

MENUITEM *MENUINTERFACE::GetItem(int iItem)
{
	return &(this->ItemList[iItem]);
}

int MENUINTERFACE::GetItemNum()
{
	return this->nItem;
}

MENUITEM *MENUINTERFACE::FindItem(wstring sName)
{
	for (int i = 0; i < nItem; i++)
	{
		if (this->ItemList[i].sName == sName)
			return &(this->ItemList[i]);
	}
	return NULL;
}

void MENUINTERFACE::DeleteItem(wstring sName)
{
	MENUITEM *ToDelete = this->FindItem(sName);
	ToDelete->Delete();
	for (int i = ToDelete->iItem + 1; i < nItem; i++)
	{
		this->ItemList[i - 1] = this->ItemList[i];
	}
	nItem--;
	return;
}

void MENUINTERFACE::ClearMenu()
{
	for (int i = 0; i < nItem; i++)
		this->ItemList[i].Delete();
	this->nItem = 0;
	this->iSelect = 0;
	this->bShowDescript = 0;
	this->pfPreprint = NULL;
	this->pfPostprint = NULL;
	this->pfAdditionalProc = NULL;
	this->sTitle = L"";
	this->posPrintList = { 0, 0 };
	return;
}

void MENUINTERFACE::ShowDescription(bool bShow)
{
	this->bShowDescript = bShow;
}

void MENUINTERFACE::SetTitle(wstring sTitle)
{
	this->sTitle = sTitle;
}



/*----- DISPLAYINTERFACE class -----*/

PROCRESULT DISPLAYINTERFACE::ProcFunc(int dParam)
{

	if (pfDisplay != NULL)
		return (*pfDisplay)(this->self);

	return PRORES_ERROR;
}

void DISPLAYINTERFACE::SetDisplay(PROCRESULT(*pfDisplay)(INTERFACE *pInf))
{
	this->pfDisplay = pfDisplay;
	return;
}


/*----- INTERFACE class -----*/

int INTERFACE::nInterface = 0;
INTERFACE *INTERFACE::pInfStart;
INTERFACE *INTERFACE::pCurrInterface;
INTERFACE *INTERFACE::InterfaceHeap[NMAX_INTERFACE];

INTERFACE::INTERFACE(INTERFACE_TYPE type)
{
	this->type = type;
	this->MNIF::self = this;
	this->DPIF::self = this;

	nInterface++;
	InterfaceID = nInterface;
	InterfaceHeap[InterfaceID] = this;

	if (pCurrInterface == NULL) pCurrInterface = this;
}

INTERFACE::INTERFACE(PROCRESULT(*pfDisplay)(INTERFACE *pInf))
{
	this->type = INTERFACE_DISPLAY;
	this->DPIF::self = this;

	nInterface++;
	InterfaceID = nInterface;
	InterfaceHeap[InterfaceID] = this;

	if (pCurrInterface == NULL) pCurrInterface = this;

	this->SetDisplay(pfDisplay);
}

PROCRESULT INTERFACE::ProcFunc(int dParam)
{
	switch (this->type)
	{
	case INTERFACE_MENU:
	{
		return MENU::ProcFunc(dParam);
	}
	case INTERFACE_DISPLAY:
	{
		return DISPLAY::ProcFunc(dParam);
	}
	default:
	{
		return PRORES_ERROR;
	}
	}
}

int INTERFACE::GetInterfaceNum()
{
	return nInterface;
}


INTERFACE* INTERFACE::GetInterfaceFromID(int ID)
{
	return InterfaceHeap[ID];
}

int INTERFACE::GetInterfaceID()
{
	return this->InterfaceID;
}

INTERFACE_TYPE INTERFACE::GetInterFaceType()
{
	return this->type;
}

int INTERFACE::GetCurrentInterfaceID()
{
	return pCurrInterface->GetInterfaceID();
}

INTERFACE* INTERFACE::GetCurrentInterface()
{
	return pCurrInterface;
}

INTERFACE **INTERFACE::GetParentHandle()
{
	switch (this->type)
	{
	case INTERFACE_MENU:
		return &(this->MENUINTERFACE::parent);
	case INTERFACE_DISPLAY:
		return &(this->DISPLAYINTERFACE::parent);
	}
	return NULL;
}

INTERFACE *INTERFACE::GetParent()
{
	return *(this->GetParentHandle());
}

void INTERFACE::SetParent(INTERFACE *parent)
{
	*(this->GetParentHandle()) = parent;
	return;
}

void INTERFACE::SetStartInterface(INTERFACE *pInfStart)
{
	INTERFACE::pInfStart = pInfStart;
	pCurrInterface = pInfStart;
	return;
}

INTERFACE *INTERFACE::GetStartInterface()
{
	return pInfStart;
}

void INTERFACE::EndInterfaceProc()
{
	endwin();
}

PROCRESULT INTERFACE::DealWithProcReturnValue(PROCRESULT dRet)
{
	if (~dRet & PRORESMSK_NOCLEAR)
		clear();

	if (LOWORD(dRet) & PRORES_ERROR)
	{
		attron(COLOR_PAIR(COLORINDEX_ERRORCODE));
		printw("Error with error code %d\n", HIWORD(dRet));
		attroff(COLOR_PAIR(COLORINDEX_ERRORCODE));

		kbgetch();
		while (::GetKeyState(VK_RETURN) < 0)
			kbgetch();

		pCurrInterface = pInfStart;
		return dRet;
	}

	if (LOWORD(dRet) & PRORES_SUCCESS)
		return dRet;

	if (LOWORD(dRet) & PRORES_INTERFACECONVERT)
	{
		if (dRet & PRORESMSK_SHAREPARENT)
		{
			GetInterfaceFromID(HIWORD(dRet))->SetParent(pCurrInterface->GetParent());
		}
		else
		{
			if ((~dRet & PRORESMSK_NOSETPARENT) && (HIWORD(dRet) != pInfStart->GetInterfaceID()))
				GetInterfaceFromID(HIWORD(dRet))->SetParent(pCurrInterface);
		}
		pCurrInterface = GetInterfaceFromID(HIWORD(dRet));
	}

	if (LOWORD(dRet) & PRORES_BACKWARD)
	{
		if (HIWORD(dRet) == 0)
		{
			if (pCurrInterface->GetParent() != NULL)
				pCurrInterface = pCurrInterface->GetParent();
		}
		else
		{
			pCurrInterface = GetInterfaceFromID(HIWORD(dRet));
		}
		return dRet;
	}

	if (LOWORD(dRet) & PRORES_STARTMENU)
	{
		pCurrInterface = INTERFACE::GetStartInterface();
	}

	return dRet;
}
