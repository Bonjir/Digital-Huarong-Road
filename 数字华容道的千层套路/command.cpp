#include "command.h"
#define Lowercase(ch) ((ch) >= 'A' && (ch) <= 'Z' ? (ch) - 'A' + 'a' : (ch))

COMMAND::COMMAND(string keyword)
{
	this->dProgress = 0;
	this->sKeyword = keyword;
	this->pfPass = NULL;
}

COMMAND::COMMAND(string keyword, void(*pfPass)())
{
	this->dProgress = 0;
	this->sKeyword = keyword;
	this->pfPass = pfPass;
}

bool COMMAND::Pass(int dKeyCurses)
{
	if (Lowercase(char(dKeyCurses)) == Lowercase(this->sKeyword[dProgress]))
	{
		this->dProgress++;
	}
	else this->dProgress = (char(dKeyCurses) == this->sKeyword[0] ? 1 : 0);

	if (this->dProgress >= this->sKeyword.length())
		return 1;
	else return 0;
}

bool COMMAND::PassFunc(int dKeyCurses)
{
	bool dRet = Pass(dKeyCurses);
	if (dRet)
		if (this->pfPass != NULL)
			(*pfPass)();
	return dRet;
}
