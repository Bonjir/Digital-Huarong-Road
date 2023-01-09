#include <cstdio>
#include <conio.h>
#include "command.h"

int main()
{
	COMMAND comm1("comm1"),
		comm2("comm2"),
		comm3("eeeeeee");
	int ch, cp1 = 0, cp2 = 0, cp3 = 0;

	while (!cp1 || !cp2 || !cp3)
	{
		ch = _getch();
		if (!cp1) cp1 = comm1.Pass(ch);
		if (!cp2) cp2 = comm2.Pass(ch);
		if (!cp3) cp3 = comm3.Pass(ch);
		if (cp1) printf("comm1 passed");
		if (cp2) printf("comm2 passed");
		if (cp3) printf("comm3 passed");
		puts("");
	}

	return printf("all passed");

}