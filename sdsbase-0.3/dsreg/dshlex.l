%{
#include <string.h>
#include <SharedData.h>
#include "dshyacc.h"

%}

%x COMMENT VARIDS ENTRY 

%option noyywrap
%option nounput

%%

[ \t\n]+
#.* 

"/*"		 BEGIN(COMMENT);
<COMMENT>[^*]*
<COMMENT>"*"+[^*/]*
<COMMENT>"*"+"/" BEGIN(INITIAL);

[^ :\t\n]+/[ \t]*:  {
         dsh_yylval.sval=strdup(dsh_yytext);
         BEGIN(VARIDS);
         return(T_NAME);
}


<VARIDS>([0-9]+[ ]*[,-][ ]*)?[0-9]+ { 
                    dsh_yylval.sval=strdup(dsh_yytext); 
                    return(T_IDS);  }
<VARIDS>[ \t\n] 
<VARIDS>[^{ \t\n] { return(dsh_yytext[0]); }
<VARIDS>"{"       { BEGIN(ENTRY); return('{'); }

<ENTRY>#.* 
<ENTRY>[^#}]*     { return(T_ENTRY); }
<ENTRY>"}"        { BEGIN(INITIAL); return('}'); }


%%
