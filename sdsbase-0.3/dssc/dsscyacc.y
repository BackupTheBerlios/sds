%{
#include <stdlib.h>
#include <string.h>
#include "dssc.h"

char bef[500];
double cons[100]; 
int bi,ci;

int yyerror(char *s);
int yylex( void );

%}

%token YYERROR_VERBOSE

%union
{
	int ival;
	double fval;
}

%token	T_XARR	T_YARR	T_X	T_Y
%token	T_SIN	T_COS	T_TAN	T_ASIN	T_ACOS	T_ATAN	T_ATAN2
%token	T_EXP	T_LOG	T_LOG10	T_POW	T_SQRT	T_CBRT	T_HYPOT
%token	T_EXPM1	T_LOG1P	T_SINH	T_COSH	T_TANH	T_ASINH	T_ACOSH
%token  T_ATANH	T_FABS  T_PSI   T_PSIFN T_RAND  T_THETA T_MIN
%token  T_MAX   T_XMIN  T_XMAX  T_YMIN  T_YMAX

%token	T_INTEGER	T_IDOUBLE	T_NDOUBLE

%token	T_DOUBLE 

%type <ival> T_INTEGER

%type <fval> T_DOUBLE

%left '-' '+'
%left '*' '/'
%left NEG

%%
	line: exp

	exp: T_DOUBLE { cons[ci++] = $1; bef[bi++]=Y_PUT; }
		| exp '+' exp { bef[bi++]=Y_PLUS; }
		| exp '-' exp { bef[bi++]=Y_MINUS; }
		| exp '*' exp { bef[bi++]=Y_MULT; }
		| exp '/' exp { bef[bi++]=Y_DIV; }
		| '-' exp %prec NEG { bef[bi++]=Y_NEG; }
		| T_IDOUBLE { bef[bi++]=Y_PUTIND; }
		| T_NDOUBLE { bef[bi++]=Y_PUTMAXIND; }
		| T_XARR index ']' { bef[bi++]=Y_XARR; }
		| T_YARR index ']' { bef[bi++]=Y_YARR; }
		| T_X { bef[bi++]=Y_X; }
		| T_Y { bef[bi++]=Y_Y; }
		| T_XMIN { bef[bi++]=Y_XMIN; }
		| T_XMAX { bef[bi++]=Y_XMAX; }
		| T_YMIN { bef[bi++]=Y_YMIN; }
		| T_YMAX { bef[bi++]=Y_YMAX; }
		| '(' exp ')' 
		| T_SIN   '(' exp ')'         { bef[bi++]=Y_SIN; }
		| T_COS   '(' exp ')'         { bef[bi++]=Y_COS; }
		| T_TAN   '(' exp ')'         { bef[bi++]=Y_TAN; }
		| T_ASIN  '(' exp ')'         { bef[bi++]=Y_ASIN; }
		| T_ACOS  '(' exp ')'         { bef[bi++]=Y_ACOS; }
		| T_ATAN  '(' exp ')'         { bef[bi++]=Y_ATAN; }
		| T_ATAN2 '(' exp ',' exp ')' { bef[bi++]=Y_ATAN2; }
		| T_EXP   '(' exp ')'         { bef[bi++]=Y_EXP; }
		| T_LOG   '(' exp ')'         { bef[bi++]=Y_LOG; }
		| T_LOG10 '(' exp ')'         { bef[bi++]=Y_LOG10; }
		| T_POW   '(' exp ',' exp ')' { bef[bi++]=Y_POW; }
		| T_SQRT  '(' exp ')'         { bef[bi++]=Y_SQRT; }
		| T_CBRT  '(' exp ')'         { bef[bi++]=Y_CBRT; }
		| T_HYPOT '(' exp ',' exp ')' { bef[bi++]=Y_HYPOT; }
		| T_EXPM1 '(' exp ')'         { bef[bi++]=Y_EXPM1; }
		| T_LOG1P '(' exp ')'         { bef[bi++]=Y_LOG1P; }
		| T_SINH  '(' exp ')'         { bef[bi++]=Y_SINH; }
		| T_COSH  '(' exp ')'         { bef[bi++]=Y_COSH; }
		| T_TANH  '(' exp ')'         { bef[bi++]=Y_TANH; }
		| T_ASINH '(' exp ')'         { bef[bi++]=Y_ASINH; }
		| T_ACOSH '(' exp ')'         { bef[bi++]=Y_ACOSH; }
		| T_ATANH '(' exp ')'         { bef[bi++]=Y_ATANH; }
		| T_FABS  '(' exp ')'         { bef[bi++]=Y_FABS; }
		| T_PSI   '(' exp ')'         { bef[bi++]=Y_PSI; }
		| T_PSIFN '(' exp ',' exp ')' { bef[bi++]=Y_PSIFN; }
		| T_RAND  '(' exp ')'         { bef[bi++]=Y_RAND; }
		| T_MIN   '(' exp ',' exp ')' { bef[bi++]=Y_MIN; }
		| T_MAX   '(' exp ',' exp ')' { bef[bi++]=Y_MAX; }
		| T_THETA '(' exp ')'         { bef[bi++]=Y_THETA; }
		;

	index: T_INTEGER { cons[ci++]= $1; bef[bi++]=Y_PUT; } 
		| 'i' { bef[bi++]=Y_PUTIND; }
		| 'N' { bef[bi++]=Y_PUTMAXIND; }
		| index '+' index { bef[bi++]=Y_PLUS; }
		| index '-' index { bef[bi++]=Y_MINUS; }
		| index '*' index { bef[bi++]=Y_MULT; }
		| index '/' index { bef[bi++]=Y_DIV; }
		| '(' index ')'
		;

%%

int yyerror(char *s)
{
  printf("error: %s\n",s);
  return 0;
}
