%{

#include <math.h>
#include "y.tab.h"

%}

%x SIGMA

%option noyywrap
%option nounput

%%

i[ \t]*= {return (T_MIN);}
[x]?min[ \t]*= {return (T_MIN);}
[x]?minimum[ \t]*= {return (T_MIN);}

a[ \t]*= {return (T_MAX);}
[x]?max[ \t]*= {return (T_MAX);}
[x]?maximum[ \t]*= {return (T_MAX);}

n[ \t]*= {return (T_NOP);}
NoP[ \t]*= {return (T_NOP);}
Number[ \t]+of[ \t]+Points[ \t]*= {return (T_NOP);}

f[ \t]*= {return (T_SFA);}
s-factor[ \t]*= {return (T_SFA);}
smoothing[ \t]+factor[ \t]*= {return (T_SFA);}

k[ \t]*= {return (T_ORD);}
spline[ \t]+order[ \t]*= {return (T_ORD);}
spline-order[ \t]*= {return (T_ORD);}

e[ \t]*= {return (T_DER);}
derivative-order[ \t]*= {return (T_DER);}
der-order[ \t]*= {return (T_DER);}
derivative[ \t]*= {return (T_DER);}
der[ \t]*= {return (T_DER);}
order[ \t]*= {return (T_DER);}

sigma[ \t]*= {BEGIN(SIGMA);return (T_SIG);}
Sigma[ \t]*= {BEGIN(SIGMA);return (T_SIG);}
S[ \t]*= {BEGIN(SIGMA);return (T_SIG);}

ok {return (T_OK);}
accept {return (T_OK);}

tr {return (T_TR);}
tog(gle)?_?res(iduum)? {return (T_TR);}

res(iduum)? {return (T_RES);}

rep(lace)? {return (T_REP);}

abort {return (T_ABORT);}

iopt[ \t]*= {return (T_IOPT);}

[+-]?[0-9]+ {
               sscanf(yytext,"%d",&dsdydx_yylval.ival); 
               return(T_INT); }

[+-]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][+-]?[0-9]{1,3})? {
               sscanf(yytext,"%lf",&dsdydx_yylval.fval); 
               return(T_DOUBLE); }

[[:blank:]]*
";" { return ';' ; }

<SIGMA>
{
	"(" { return '('; }
	")" { return ')'; }
	"|" { return '|'; }
	"," { return ','; }
	"\"" { return '"'; }
	[+-]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][+-]?[0-9]{1,3})? {
			sscanf(yytext,"%lf",&dsdydx_yylval.fval); 
			return(T_DOUBLE); }
	[[:blank:]]* 
	";" { BEGIN(INITIAL); return ';';}
        <<EOF>>  { BEGIN(INITIAL); return 0;}
}

