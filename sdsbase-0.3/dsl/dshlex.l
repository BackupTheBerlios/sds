%{
#include <string.h>
#include <SharedData.h>
#include "dshyacc.h"

extern char *name;
extern int gotit;

%}

%x COMMENT VARIDS SKIP ENTRY COMMAND QUOTE

%option noyywrap
%option nounput

%%

[ \t\n]+
#.* 

"/*"		 BEGIN(COMMENT);
<COMMENT>[^*]*
<COMMENT>"*"+[^*/]*
<COMMENT>"*"+"/" BEGIN(INITIAL);

[^ :\t\n]+[ \t]*/:  {
	char *tmp, *label;
	label=strdup(dsh_yytext);
	if ((tmp=strchr(label,' '))) *tmp=0;
	if ((tmp=strchr(label,'\t'))) *tmp=0;
	if (!strcmp(name,label)) {
		gotit=1;
		BEGIN(VARIDS);
                return(T_GOTIT);
	} else 
	        BEGIN(SKIP); 
}

<SKIP>[^}]+ 
<SKIP>"}" { BEGIN(INITIAL); }

<VARIDS>[0-9]+ { dsh_yylval.ival=atoi(dsh_yytext); 
                 return(T_ID);  }
<VARIDS>[ \t\n] 
<VARIDS>[^{ \t\n] { return(dsh_yytext[0]); }
<VARIDS>"{" { BEGIN(ENTRY); return('{'); }

<ENTRY>\t[ \t]*/[^}#\n]  { BEGIN(COMMAND); return(T_COMMAND);  }
<ENTRY>#.* 
<ENTRY>[ \t\n]
<ENTRY>[0-9]+   { dsh_yylval.ival=atoi(dsh_yytext);  
                  return(T_ID); }
<ENTRY>[^ \t\n}] { return(dsh_yytext[0]); }
<ENTRY>"}"      { BEGIN(INITIAL); return('}'); }


<COMMAND>$[0-9]+ { 
	sscanf(dsh_yytext,"$%d",&dsh_yylval.ival); 
	return(T_IDVAR);
}
<COMMAND>\"    { BEGIN(QUOTE); return(dsh_yytext[0]) ; }
<COMMAND>[^"$\n \t\\]+ { 
	dsh_yylval.sval=strdup(dsh_yytext);
	return(T_STRING);
}
<COMMAND>[ \t] 
<COMMAND>\\[ \t]*\n 
<COMMAND>\n  { BEGIN(ENTRY); }

<QUOTE>\"   { BEGIN(COMMAND); return(dsh_yytext[0]) ; }
<QUOTE>[^"]+ {
	dsh_yylval.sval=strdup(dsh_yytext);
	return(T_STRING);
}


%%

