%{
#include <string.h>
#include <SharedData.h>
#include "datyacc.h"


%}

%x COMMENT NUMOPT PLNOPT NAME IDENTNUM IDENTSTR

%option noyywrap
%option nounput

%%

[ \t\n]+
"/*"	{ BEGIN(COMMENT); }
.       { YY_FLUSH_BUFFER; yyterminate(); }


<COMMENT>data[ ]+of[ ]+id[ ]+[0-9]+[ ]*: {
	dat_yylval.ival=1;
	return(T_DATA);
}
<COMMENT>binary[ ]+data[ ]+of[ ]+id[ ]+[0-9]+[ ]*:  { 
	dat_yylval.ival=2;
	return(T_DATA);
}

<COMMENT>size[ \t]*=[ \t]*  { BEGIN(NUMOPT); return(T_SIZE); }
<COMMENT>ident\[      { BEGIN(IDENTNUM); return(T_IDENT); }
<COMMENT>linestyle[ \t]*=[ \t]*    { BEGIN(NAME); return(T_LSTYLE); }
<COMMENT>linecolor[ \t]*=[ \t]*    { BEGIN(NAME);  return(T_LCOLOR); }
<COMMENT>linewidth[ \t]*=[ \t]*    { BEGIN(NUMOPT); return(T_LWIDTH); }
<COMMENT>symbolstyle[ \t]*=[ \t]*  { BEGIN(NAME); return(T_SSTYLE); }
<COMMENT>symbolcolor[ \t]*=[ \t]*  { BEGIN(NAME);  return(T_SCOLOR); }
<COMMENT>symbolsize[ \t]*=[ \t]*   { BEGIN(NUMOPT); return(T_SSIZE); }
<COMMENT>plotlevel[ \t]*=[ \t]*    { BEGIN(PLNOPT); return(T_PLEV); }

<COMMENT>[^*] 
<COMMENT>"*"+[^*/] 
<COMMENT>"*"+"/" { BEGIN(INITIAL); }

<NUMOPT>[0-9]+        { dat_yylval.ival=atoi(dat_yytext); return(T_NUM); }
<NUMOPT>[^0-9]        { BEGIN(COMMENT); }

<PLNOPT>[0-9]+        { dat_yylval.ival=atoi(dat_yytext); return(T_PLNUM); }
<PLNOPT>"-1"          { dat_yylval.ival=-1; return(T_PLNUM); }
<PLNOPT>[^0-9-]       { BEGIN(COMMENT); }

<NAME>[^ \n\t]*       { dat_yylval.sval=strdup(dat_yytext); return(T_STRING); }
<NAME>[ \t\n]         { BEGIN(COMMENT); }    

<IDENTNUM>[0-9]+      { dat_yylval.ival=atoi(dat_yytext); return(T_NUM); }
<IDENTNUM>"]"[ \t]*=[ \t]*["] { BEGIN(IDENTSTR); }

<IDENTSTR>[^"]*       { dat_yylval.sval=strdup(dat_yytext); return(T_STRING); }
<IDENTSTR>["]         { BEGIN(COMMENT); }

%%
