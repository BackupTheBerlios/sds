%{
#include <stdlib.h>
#include <string.h>
#include "dsdel.h"

#define YYDEBUG 1

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
%token  T_ATANH	T_FABS	T_FMOD

%token	T_INTEGER	T_IDOUBLE       T_NDOUBLE

%token	T_DOUBLE	T_COMP		T_LOGI		T_NOT

%type <ival>		T_INTEGER	T_LOGI		T_COMP

%type <fval> T_DOUBLE

%left '-' '+'
%left '*' '/'
%left NEG

%%
	line: compare '\n'
		;
	
	compare: exp T_COMP exp { 
			switch( $2) {
			case C_NE:
				bef[bi++]=Y_NE;
				break;
			case C_EQ:
				bef[bi++]=Y_EQ;
				break;
			case C_LE:
				bef[bi++]=Y_LE;
				break;
			case C_GE:
				bef[bi++]=Y_GE;
				break;
			case C_LT:
				bef[bi++]=Y_LT;
				break;
			case C_GT:
				bef[bi++]=Y_GT;
				break;
			}	
		}
		| '(' compare ')'
		| T_NOT '(' compare ')' { bef[bi++]=Y_NOT; }
		| '(' compare ')' T_LOGI '(' compare ')' {
			switch( $4) {
			case L_AND:
				bef[bi++]=Y_AND;
				break;
			case L_OR:
				bef[bi++]=Y_OR;
				break;
			}
		}
		;

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
		| T_FMOD  '(' exp ',' exp ')' { bef[bi++]=Y_FMOD; }
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
