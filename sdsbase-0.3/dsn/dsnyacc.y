%{
#include <stdlib.h>
#include <string.h>
#include "dsn.h"

#define YYDEBUG 1

struct DsnParam yyparam = { NULL, /* path */
			    NULL, /* name */
			    NULL, /* ver */
			    0,    /* ind */
			    0,    /* x */
			    0     /* y */
};

int yyerror(char *s);
int yylex( void );

%}

%token YYERROR_VERBOSE

%union
{
	int ival;
	char *sval;
}

%token	T_PATH   T_NAME  T_VER   T_IND   T_X   T_Y

%type <ival>  T_IND  T_X  T_Y
%type <sval>  T_PATH   T_NAME    T_VER

%%

        identstr: filename T_X T_Y { yyparam.x=$2; yyparam.y=$3; }
                | filename T_VER T_IND  { yyparam.ver=$2 ; yyparam.ind=$3; }
                | filename
		;

        filename: T_PATH T_NAME { yyparam.path=$1; yyparam.name=$2; }
                | T_NAME        { yyparam.name=$1; }
	        ;

%%

int yyerror(char *s)
{
  printf("error: %s\n",s);
  return 0;
}
