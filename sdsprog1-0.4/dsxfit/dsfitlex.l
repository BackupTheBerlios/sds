%{

#include <math.h>
#include <string.h>
#include "dsfityacc.h"

%}

%x SIGMA FUNC

%option noyywrap
%option nounput

%%

[x]?min[ \t]*= {return (T_MIN);}
[x]?minimum[ \t]*= {return (T_MIN);}

[x]?max[ \t]*= {return (T_MAX);}
[x]?maximum[ \t]*= {return (T_MAX);}

[Nn][Oo][Pp][ \t]*= {return (T_NOP);}
Number[ \t]+of[ \t]+Points[ \t]*= {return (T_NOP);}

tol[ \t]*= {return (T_TOL);}
ftol[ \t]*= {return (T_FTO);}
xtol[ \t]*= {return (T_XTO);}
gtol[ \t]*= {return (T_GTO);}
sigma[ \t]*= {BEGIN(SIGMA);return (T_SIG);}
Sigma[ \t]*= {BEGIN(SIGMA);return (T_SIG);}

maxitera?[ \t]*= {return (T_ITE);}
itera?[ \t]*= {return (T_ITE);}

func(tion)?[ \t]*= {BEGIN(FUNC); return(T_FNC);}

[a-z][ \t]*= {dsfit_yylval.ival=(int)*yytext-97; return (T_PARS); }
[A-Z][ \t]*= {dsfit_yylval.ival=(int)*yytext-65; return (T_PARS); }

tr {return (T_TR);}
tog(gle)?_?res(iduum)? {return (T_TR);}

ok {return (T_OK);}
accept {return (T_OK);}

res(iduum)? {return (T_RES);}

rep(lace)? {return (T_REP);}

abort {return (T_ABORT);}

[+-]?[0-9]+ {
               sscanf(yytext,"%d",&dsfit_yylval.ival); 
               return(T_INT); }

[+-]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][+-]?[0-9]{1,3})? {
               sscanf(yytext,"%lf",&dsfit_yylval.fval); 
               return(T_DOUBLE); }

[[:blank:]]*

";" { return ';' ; }
"\?" { return '?' ; }
"=" { return '=' ; } 

[^=;?[:blank:]]* { return *yytext; }

<SIGMA>
{
	"(" { return '('; }
	")" { return ')'; }
	"|" { return '|'; }
	"," { return ','; }
	"\"" { return '"'; }
	[+-]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][+-]?[0-9]{1,3})? {
			sscanf(yytext,"%lf",&dsfit_yylval.fval); 
			return(T_DOUBLE); }
	[[:blank:]]*
	";" { BEGIN(INITIAL); return ';';}
        <<EOF>>  { BEGIN(INITIAL); return 0;}
}

<FUNC>
{
        [^;]* { dsfit_yylval.sval=strdup(yytext); return(T_STRING); }
        ";"   { BEGIN(INITIAL); return ';';}
        <<EOF>>  { BEGIN(INITIAL); return 0;}
}
