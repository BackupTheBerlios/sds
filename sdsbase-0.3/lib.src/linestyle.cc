#include <linestyle.h>

char *linestyle[]=
{ 
	"NoPen",        
	"SolidLine",     
	"DashLine",     
	"DotLine",      
	"DashDotLine",   
	"DashDotDotLine"
};

int maxlinestyle(void)
{
	return sizeof(linestyle)/sizeof(char (*)[]);
}
