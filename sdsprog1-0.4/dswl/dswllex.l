%{

#include <math.h>
#include "y.tab.h"

%}

%x SIGMA

%option noyywrap
%option nounput

%%

[x]?min[ \t]*= {return (T_MIN);}
[x]?minimum[ \t]*= {return (T_MIN);}

[x]?max[ \t]*= {return (T_MAX);}
[x]?maximum[ \t]*= {return (T_MAX);}

N[ \t]*= {return (T_NOP);}
NoP[ \t]*= {return (T_NOP);}
Number[ \t]+of[ \t]+Points[ \t]*= {return (T_NOP);}

tol[ \t]*= {return (T_TOL);}
ftol[ \t]*= {return (T_FTO);}
xtol[ \t]*= {return (T_XTO);}
gtol[ \t]*= {return (T_GTO);}
sigma[ \t]*= {BEGIN(SIGMA);return (T_SIG);}
Sigma[ \t]*= {BEGIN(SIGMA);return (T_SIG);}
S[ \t]*= {return (T_SIG);}
maxitera?[ \t]*= {return (T_ITE);}
itera?[ \t]*= {return (T_ITE);}

A[ \t]*= {dswl_yylval.ival=0; return (T_PARS); }
a[ \t]*= {dswl_yylval.ival=0; return (T_PARS); }
B_si[ \t]*= {dswl_yylval.ival=1; return (T_PARS); }
b_si[ \t]*= {dswl_yylval.ival=1; return (T_PARS); }
bsi[ \t]*= {dswl_yylval.ival=1; return (T_PARS); }
Bsi[ \t]*= {dswl_yylval.ival=1; return (T_PARS); }
b[ \t]*= {dswl_yylval.ival=2; return (T_PARS); }
B0[ \t]*= {dswl_yylval.ival=3; return (T_PARS); }
b0[ \t]*= {dswl_yylval.ival=3; return (T_PARS); }
G0[ \t]*= {dswl_yylval.ival=4; return (T_PARS); }
g0[ \t]*= {dswl_yylval.ival=4; return (T_PARS); }

tr {return (T_TR);}
tog(gle)?_?res(iduum)? {return (T_TR);}

ok {return (T_OK);}
accept {return (T_OK);}

abort {return (T_ABORT);}

res(iduum)? {return (T_RES);}

rep(lace)? {return (T_REP);}

[+-]?[0-9]+ {
               sscanf(yytext,"%d",&dswl_yylval.ival); 
               return(T_INT); }

[+-]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][+-]?[0-9]{1,3})? {
               sscanf(yytext,"%lf",&dswl_yylval.fval); 
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
			sscanf(yytext,"%lf",&dswl_yylval.fval); 
			return(T_DOUBLE); }
	[[:blank:]]*
	";" { BEGIN(INITIAL); return ';';}
        <<EOF>>  { BEGIN(INITIAL); return 0;}
}
