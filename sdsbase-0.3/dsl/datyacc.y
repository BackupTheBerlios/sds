%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linestyle.h>
#include <symbolstyle.h>
#include <SharedData.h>

int yyerror(char *s);
int yylex( void );

extern SharedData *sdf;
extern int srcid;
extern int srctype;
extern int srcn;

char **brgs=NULL;
int brgsn=0;

%}


%union {
	int  ival;
	char *sval;
}

%token <ival> T_DATA T_NUM T_PLNUM
%token <sval> T_STRING
%token T_SIZE T_IDENT T_LSTYLE T_LCOLOR T_LWIDTH T_SSTYLE T_SCOLOR
%token T_SSIZE T_PLEV

%%
header: /*empty*/
        | options { if (brgs) sdf->set_ident(srcid,(const char **) brgs); }
;

options: option
         | options option
;

option:  T_DATA { srctype = $1; }
         | T_SIZE T_NUM { sdf->resize(srcid,$2); srcn=$2; }
         | T_IDENT T_NUM T_STRING { 
		 if ($2>=brgsn) {
			 brgsn=$2+1;
			 brgs = (char **) realloc(brgs, 
						  (brgsn+1)*sizeof(char *));
			 brgs[brgsn]=NULL;
		 }
		 brgs[$2] = $3;
	 }
         | T_LSTYLE T_STRING {
		 if($2) {
			 int i,max;
			 max=maxlinestyle();
			 for(i=0;i<max;i++)
				 if(!strcmp($2,linestyle[i])) {
					 sdf->set_linestyle(srcid,i);
					 break;
				 }
		}
	 }
         | T_LCOLOR T_STRING { sdf->set_linecolor(srcid,$2); }
         | T_LWIDTH T_NUM { sdf->set_linewidth(srcid,(unsigned int) $2); }
         | T_SSTYLE T_STRING { 
		 int i,max;
		 if ($2) {
			 max=maxsymbolstyle();
			 for(i=0;i<max;i++)
				 if(!strcmp($2,symbolstyle[i])) {
					 sdf->set_symbolstyle(srcid,i);
					 break;
				 }
		 }
	 }
         | T_SCOLOR T_STRING { sdf->set_symbolcolor(srcid,$2); }
         | T_SSIZE T_NUM { sdf->set_symbolsize(srcid, (unsigned int) $2); }
         | T_PLEV T_PLNUM { sdf->set_plev(srcid, $2); }
;  

%%

int yyerror(char *s)
{
  printf("%s\n",s);
  return 0;
}


#ifdef STANDALONE
int main()
{
  yyparse();

  return 0;
}
#endif

