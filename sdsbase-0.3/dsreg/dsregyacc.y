%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SharedData.h>
#include "util.h"
  
int dsreg_yyerror(char *s);
int dsreg_yylex( void );
 
extern int mode;
/* if mode>=0:  look for dependecies, set mode to number of dependencies 
   if mode==-1: look for lines matching idset wanted
*/

extern idset *wanted;
extern idset *found;
extern idset *temps;
extern idset *varlist;
extern FILE *dsh;

%}


%union {
	int  ival;
	char *sval;
	char cval;
	idset* idval;
	opidset* oval;
	tagset* tval;
}

%token <ival> T_TAGID
%token <ival> T_IDVAR
%token <sval> T_COMMAND
%token <sval> T_COMMENT
%token T_SCAN

%type <tval>  tag
%type <oval>  opidlist
%type <idval> idlist
%type <sval>  commands
%type <sval>  comments

%%

line: /* empty */
      | line '<' tag '>' T_SCAN {  /* when looking for dependencies */
	      idset *ops;
	      ops=join_sets($3->opids->modset,
			    join_sets($3->opids->newset,
				      $3->opids->delset));
	      if (match_sets(ops,wanted)) {  /* this tag is relevant */
		      sort_set(ops);
		      mode += ops->n - match_sets(wanted,ops);
		      append_set(&wanted,ops);  /* ALL opids must be 
						   registered! */
		      sort_set(wanted);
		      append_set(&found,ops);
		      sort_set(found);
		      if ($3->depids) { /* looking for new dependencies */
			      mode += $3->depids->n 
				      - match_sets(wanted,$3->depids);
			      append_set(&wanted,$3->depids);
			      sort_set(wanted);
			      append_set(&found,$3->depids);
			      sort_set(found);
		      }
		      append_set(&temps,$3->opids->delset); /* marking 
							       temporary ids */
		      sort_set(temps);
	      }
	      free_tag($3);
	      free_idset(ops);
      }
      | line '<' tag '>' commands comments { /* when collecting commands */
	      idset *ops;
	      ops=join_sets($3->opids->modset,
			    join_sets($3->opids->newset,
				      $3->opids->delset));
	      if (match_sets(ops,wanted)) { 
		      if ($3->opids->newset) {
			      print_set(dsh,transpose_set(varlist,
						      $3->opids->newset));
		      }
		      fprintf(dsh,"\t%s\n",$5);
		      if ($6) fprintf(dsh,"\t%s\n",$6);
	      }
	      free_idset(ops);
	      free_tag($3);
	      free($5); free($6);
      }
;

commands: T_COMMAND            { $$=$1; }
          | commands T_IDVAR   { 
	          asprintf(&($$),"%s$%d",$1,find_in_set(varlist,$2)); 
		  free($1);
          }
          | commands T_COMMAND { 
		  asprintf(&($$),"%s%s",$1,$2);
		  free($1); free($2);
	  }
;

comments:  /*empty*/ { $$ = NULL; } 
          | comments T_COMMENT { 
                  if ($1) asprintf(&($$),"%s\n\t%s",$1,$2); 
                  else asprintf(&($$),"%s",$2);
		  free($1); free($2);
	  }
;

tag: opidlist                { $$ = create_tag($1,NULL); }
     | opidlist ':' idlist   { $$ = create_tag($1,$3); }
;

opidlist: idlist           { $$ = create_opidset($1,NULL,NULL); }
          | '*' idlist '*' { $$ = create_opidset(NULL,$2,NULL); }
          | '+' idlist '+' { $$ = create_opidset(NULL,NULL,$2); }
          | opidlist ';' idlist {
		  append_set(&($1->modset),$3);
		  $$=$1; 
		  free_idset($3);
	  }
          | opidlist ';' '*' idlist '*' { 
		  append_set(&($1->newset),$4);
		  $$=$1;
		  free_idset($4);
	  }
          | opidlist ';' '+' idlist '+' { 
		  append_set(&($1->delset),$4);
		  $$=$1; 
		  free_idset($4);
	  }
;

idlist: T_TAGID                          { $$ = add_to_set(NULL,$1,0); }
        | idlist ',' T_TAGID             { $$ = add_to_set($1,$3,0); }
        | T_TAGID '-' T_TAGID            { $$ = add_to_set(NULL,$1,$3-$1); }
        | idlist ',' T_TAGID '-' T_TAGID { $$ = add_to_set($1,$3,$5-$3); }
;
 

%%

int dsreg_yyerror(char *s)
{
  printf("%s\n",s);
  return 0;
}


#ifdef STANDALONE
int main()
{
  dsreg_yyparse();

  return 0;
}
#endif

