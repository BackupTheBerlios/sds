%{
#include <string.h>
#include <SharedData.h>
#include "util.h"
#include "dsregyacc.h"

extern int mode;
/* if mode>=0:  look for dependecies, set mode to number of dependencies 
   if mode==-1: look for lines matching idset wanted
*/

%}

%x TAG SCAN COMMAND COMMENT

%option noyywrap
%option nounput

%%

^"<"  { BEGIN(TAG); return('<'); }
[ \t\n]
[ \t]*#.* 

<TAG>">"[ \t]*    {
	if (mode>=0) BEGIN(SCAN); 
	else BEGIN(COMMAND);
	return('>'); 
}
<TAG>[0-9]+ dsreg_yylval.ival=atoi(dsreg_yytext); return(T_TAGID);
<TAG>[ ]*
<TAG>.      return(dsreg_yytext[0]);

<SCAN>.*$ BEGIN(INITIAL); return(T_SCAN);

<COMMAND>$[0-9]+ {
	sscanf(dsreg_yytext,"$%d",&dsreg_yylval.ival); 
	return(T_IDVAR);
}
<COMMAND>[^$\n]+ {
	dsreg_yylval.sval=strdup(dsreg_yytext);
	return(T_COMMAND);
}
<COMMAND>$/([^0-9]) {
	dsreg_yylval.sval=strdup(dsreg_yytext);
	return(T_COMMAND);
}
<COMMAND>\n/[ \t]*# BEGIN(COMMENT);
<COMMAND>\n     BEGIN(INITIAL);

<COMMENT>^[ \t]*#.* {
	dsreg_yylval.sval=strdup(dsreg_yytext);
	return(T_COMMENT);
}
<COMMENT>\n/[ \t]*[^#] BEGIN(INITIAL);
<COMMENT>\n

%%
