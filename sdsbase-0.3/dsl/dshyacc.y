%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SharedData.h>

int yyerror(char *s);
int yylex( void );

int argsn=0; 
idset *vars=init_set(0);
extern SharedData *sdf;
extern idset *retids;
extern int verbose,recurse,reclev;

%}


%union {
	int  ival;
	char *sval;
	char **argval;
	idset *idval;
}

%token <ival> T_ID
%token <ival> T_IDVAR
%token <sval> T_STRING
%token T_COMMAND T_GOTIT

%type <idval> idlist
%type <argval> args
%type <sval> quarg

%%

entry: /* empty */
       | T_GOTIT ':' idlist '{' commands '}' { 
	     int i;
	     retids=init_set($3->n);
	     for (i=0;i<$3->n;i++)
		     retids->ids[i]=vars->ids[$3->ids[i]-1];
	     return 0;
       }
;

commands: command 
          | commands command 
;

command:  T_COMMAND args { 
		  int i;

		  argsn++;
		  $2 = (char **) realloc($2,argsn*sizeof(char *));
		  $2[argsn-1] = NULL;

		  if (verbose) {
			  fprintf(stderr,"executing \"");
			  for (i=0;i<(argsn-2);i++)
				  fprintf(stderr,"%s ",$2[i]);
			  fprintf(stderr,"%s\" ...\n",$2[argsn-2]);
		  }

		  exec_command($2,&sdf);

		  for (i=0;i<(argsn-1);i++) {
			  free($2[i]);
		  }
		  free($2);
		  argsn=0;
         }
         | idlist T_COMMAND args {  
		  int i;
		  idset oldids,newids;

		  if (recurse && (!strcmp($3[0],"dsl"))) {
			  if (verbose) argsn+=3;
			  else argsn+=2;
			  $3 = (char **) realloc($3,argsn*sizeof(char *));
			  if (verbose) asprintf(&$3[argsn-3],"-v");
			  asprintf(&$3[argsn-2],"-r %d",recurse-1);
			  $3[argsn-1] = NULL;
		  } else {
			  argsn++;
			  $3 = (char **) realloc($3,argsn*sizeof(char *));
			  $3[argsn-1] = NULL;
		  }

		  if (verbose) {
			  fprintf(stderr,"executing \"");
			  for (i=0;i<(argsn-2);i++)
				  fprintf(stderr,"%s ",$3[i]);
			  fprintf(stderr,"%s\" ...\n",$3[argsn-2]);
		  }

		  oldids.ids = sdf->record_state(&oldids.n);
		  exec_command($3,&sdf);
		  newids.ids = sdf->what_s_new(oldids.ids,oldids.n,
						  &newids.n);
		  sort_set(&newids); /* this works because new ids are assigned
                                        in ascending order */
		  if ($1->n != newids.n) {
			  fprintf(stderr,"ERROR executing command \"");
			  for (i=0;i<(argsn-2);i++) 
				  fprintf(stderr,"%s ",$3[i]);
			  fprintf(stderr,"%s\"\n",$3[argsn-2]);
			  fprintf(stderr,"got %d new ids, expected %d!\n",
				  newids.n, $1->n);
			  if (newids.n) {
				  fprintf(stderr,"be careful with ids ");
				  print_set(stderr,&newids);
				  fprintf(stderr,"!\n");
			  }
			  YYABORT;
		  }

		  for (i=0;i<$1->n;i++) {
			  if ($1->ids[i]>vars->n) {
				  vars->n=$1->ids[i];
				  vars->ids = (int *) realloc
					  (vars->ids, vars->n*sizeof(int));
			  }
			  vars->ids[$1->ids[i]-1]=newids.ids[i];
		  }

		  for (i=0;i<(argsn-1);i++) {
			  free($3[i]);
		  }
		  free($3); 
		  argsn=0;
	 }
;

args: T_STRING  { $$=(char **) malloc(sizeof(char *)); $$[0]=$1; argsn=1; }
      | T_IDVAR { asprintf($$,"%d",vars->ids[$1-1]); argsn=1; }
      | '"' quarg '"' { $$=(char **) malloc(sizeof(char *)); 
                        $$[0]=$2; argsn=1; }
      | args T_IDVAR { 
	      char *tmp;
	      asprintf(&tmp,"%d",vars->ids[$2-1]);
	      argsn+=1; 
	      $$=(char **)realloc($$,argsn*sizeof(char *));
	      $$[argsn-1]=tmp;
      }
      | args T_STRING { 
	      argsn+=1; 
	      $$=(char **)realloc($$,argsn*sizeof(char *));
	      $$[argsn-1]=$2;
      }
      | args '"' quarg '"' { 
	      argsn+=1; 
	      $$=(char **)realloc($$,argsn*sizeof(char *));
	      $$[argsn-1]=$3;
      }
;

quarg:  T_STRING       { $$=$1; }
      | T_IDVAR        { asprintf(&$$,"%d",vars->ids[$1-1]); } 
      | quarg T_STRING { asprintf(&$$,"%s%s",$1,$2); free($1); free($2); }
      | quarg T_IDVAR  { asprintf(&$$,"%s%d",$1,vars->ids[$2-1]); free($1); }
;

idlist: T_ID                          { $$ = add_to_set(NULL,$1,0); }
        | idlist ',' T_ID             { $$ = add_to_set($1,$3,0); }
        | T_ID '-' T_ID               { $$ = add_to_set(NULL,$1,$3-$1); }
        | idlist ',' T_ID '-' T_ID    { $$ = add_to_set($1,$3,$5-$3); }
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

