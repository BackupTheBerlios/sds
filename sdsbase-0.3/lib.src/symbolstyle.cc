#include <symbolstyle.h>

char *symbolstyle[]=
{
	"NoSymbol",
	"ClosedCircle",
	"OpenCircle",
	"ClosedSquare",
	"OpenSquare",
	"ClosedDiamond",
	"OpenDiamond"
};

int maxsymbolstyle(void)
{
	return sizeof(symbolstyle)/sizeof(char(*)[]);
}
