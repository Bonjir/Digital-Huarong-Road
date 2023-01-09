#pragma once
#ifndef _COMMAND_H_
#define _COMMAND_H_

#include <string>

using std::string;

class COMMAND
{
public:
	COMMAND(string keyword);
	COMMAND(string keyword, void (*pfPass)());
	bool Pass(int dKeyCurses);
	bool PassFunc(int dKeyCurses);

private:
	int dProgress;
	string sKeyword;
	void(*pfPass)();
};


#endif
