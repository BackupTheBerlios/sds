%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SharedData.h>

int dsh_yyerror(char *s);
int dsh_yylex( void );


extern char *vername;

%}


%union {
	char *sval;
}

%token <sval> T_NAME  T_IDS
%token T_ENTRY

%%

entries: /* empty */
       | entries entry
;

entry: T_NAME ':' T_IDS '{' commands '}' {
	if (!strcmp(vername,$1))
		YYABORT;
        }
;
 
commands: /* empty */
        | commands T_ENTRY
;

%%

int dsh_yyerror(char *s)
{
  printf("%s\n",s);
  return 0;
}


#ifdef STANDALONE
int main()
{
  dsh_yyparse();

  return 0;
}
#endif

