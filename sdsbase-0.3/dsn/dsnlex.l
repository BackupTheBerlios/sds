%{

#include "y.tab.h"
#include "dsn.h"
#include <string.h>

%}

%x DSH SRC

%option noyywrap
%option nounput

%%


(.*"/")+ { dsn_yylval.sval=strdup(yytext); return(T_PATH); }
[^:/]+   { dsn_yylval.sval=strdup(yytext); return(T_NAME); }
"::"     { BEGIN(DSH); }
":"      { BEGIN(SRC); } 


<DSH>[^:]+      { dsn_yylval.sval=strdup(yytext); return(T_VER); }
<DSH>":"[0-9]+  { sscanf(yytext,":%d",&dsn_yylval.ival); return(T_IND); }
<DSH>.

<SRC>"x="[0-9]+ { sscanf(yytext,"x=%d",&dsn_yylval.ival); return(T_X); }
<SRC>"y="[0-9]+ { sscanf(yytext,"y=%d",&dsn_yylval.ival); return(T_Y); }
<SRC>.

<*><<EOF>> { YY_FLUSH_BUFFER; BEGIN(INITIAL); yyterminate(); }
