%{

#include <math.h>
#include "y.tab.h"

%}

%x INDEX

%option noyywrap
%option nounput

%%

"x["   { BEGIN(INDEX); return (T_XARR); }
"y["   { BEGIN(INDEX); return (T_YARR); }

"x" { return (T_X); }
"y" { return (T_Y); }

"e"   { dssc_yylval.fval=1.60217733e-19;
               return(T_DOUBLE); }

"hbar"  { dssc_yylval.fval=1.0545726691251018e-34;
               return(T_DOUBLE); }

"kb"  { dssc_yylval.fval=1.3806579149888776E-23;
               return(T_DOUBLE); }

"pi"  { dssc_yylval.fval=M_PI;
               return(T_DOUBLE); }

"ee/h"  { dssc_yylval.fval=3.874046102803882E-5;
               return(T_DOUBLE); }

"h/ee"  { dssc_yylval.fval=25812.805874360638;
               return(T_DOUBLE); }
"l00"  { dssc_yylval.fval=1.2331471740542616E-5;
               return(T_DOUBLE); }

"phi0"  { dssc_yylval.fval=14.135669239559144E-15;
               return(T_DOUBLE); }


"h"   { dssc_yylval.fval=6.6260755e-34;
               return(T_DOUBLE); }

"i"   { return(T_IDOUBLE); }
"N"   { return(T_NDOUBLE); }


"sin" { return(T_SIN); } 
"cos" { return(T_COS); } 
"tan" { return(T_TAN); } 
"asin" { return(T_ASIN); } 
"acos" { return(T_ACOS); } 
"atan" { return(T_ATAN); } 
"atan2" { return(T_ATAN2); } 
"exp" { return(T_EXP); } 
"log" { return(T_LOG); } 
"log10" { return(T_LOG10); } 
"pow" { return(T_POW); } 
"sqrt" { return(T_SQRT); } 
"cbrt" { return(T_CBRT); } 
"hypot" { return(T_HYPOT); } 
"expm1" { return(T_EXPM1); } 
"log1p" { return(T_LOG1P); } 
"sinh" { return(T_SINH); } 
"cosh" { return(T_COSH); } 
"tanh" { return(T_TANH); } 
"asinh" { return(T_ASINH); } 
"acosh" { return(T_ACOSH); } 
"atanh" { return(T_ATANH); } 
"fabs" { return(T_FABS); } 
"psi" { return(T_PSI); } 
"psifn" { return(T_PSIFN); } 
"rand" { return(T_RAND); }
"theta" { return(T_THETA); }
"min"   { return(T_MIN); }
"max"   { return(T_MAX); }
"xmin"   { return(T_XMIN); }
"xmax"   { return(T_XMAX); }
"ymin"   { return(T_YMIN); }
"ymax"   { return(T_YMAX); }


<INDEX>"]" { BEGIN(INITIAL); return(']'); }

<INDEX>[0-9]+ {
		sscanf(yytext,"%d",&dssc_yylval.ival);
		return T_INTEGER;
}

<INDEX>i { return('i'); }
<INDEX>N { return('N'); }

<INDEX>"*" { return('*'); }
<INDEX>"/" { return('/'); }
<INDEX>"+" { return('+'); }
<INDEX>"-" { return('-'); }

"(" { return('('); }
")" { return(')'); }
"," { return(','); }
"*" { return('*'); }
"/" { return('/'); }
"+" { return('+'); }
"-" { return('-'); }
"\n" { return('\n'); }

([0-9]+\.?[0-9]*|\.[0-9]+)([eE][+-]?[0-9]{1,3})? {
               sscanf(yytext,"%lf",&dssc_yylval.fval); 
               return(T_DOUBLE); }

[[:blank:]] 

