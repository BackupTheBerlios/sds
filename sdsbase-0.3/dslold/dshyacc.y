%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dsh_y_l.h"
#include <stdarg.h>
#include <errno.h>

#define CHUNK 16;

struct DshLine *dshlines;
int n_dshlines;
int max_ind_dshlines;
struct Comment *comments;
int n_comments; 
int max_ind_comments;
int ncom;

int yyerror(char *s);
int yylex( void );
static void add_dshl(struct Version* version);
static void add_arg(char *arg);
static void add_verp_arg(struct Version* version);
static void add_global_comment(char *token);
static void add_local_comment(char *token, int type);
static void add_dependency(struct Version *depend);
extern int line_number;
%}

%token YYERROR_VERBOSE

%union
{
	char *text;
	struct Version *verp;
}

%token	T_TEXT		T_VERSION	T_COMMENT	T_COMMENT_DSLASH
	T_COMMENT_HASH	T_BEG_COM	T_END_COM       T_DEPENDENCE

%type <text>	T_TEXT		T_COMMENT	T_COMMENT_DSLASH
		T_COMMENT_HASH
%type <verp>	T_VERSION	T_DEPENDENCE

%%
	line:		/* empty */
			| line oneline
			| error { return -1; }
			;

	oneline: 	T_VERSION {
				add_dshl($1);
				free($1);
			} command ';' 
			| comment
			| dependency
			;
	
	command:	arg
			| command arg
			;

	arg:		T_TEXT	{
				add_arg($1);
			}
			| T_VERSION {
				add_verp_arg($1);
				free($1);
			}
			;

	comment:	T_COMMENT_DSLASH {
				add_local_comment($1,COM_DSLASH);
			}
			| T_COMMENT_HASH {
				add_local_comment($1,COM_HASH);
			}
			| global_comment
			;

	global_comment:	T_BEG_COM {
				ncom++;
			}
			| T_END_COM
			| T_COMMENT {
				add_global_comment($1);
			}
			;

	dependency: T_DEPENDENCE {
				add_dependency($1);
			}
			;

%%

int yyerror(char *s)
{
	static void fatal(const char *templ, ...);
	char *mess;

	if(!n_dshlines)
		fatal("error in line %d\n"
		      "%s",
		      line_number,
		      s);

	if(dshlines[n_dshlines-1].argc>3)
		asprintf(&mess,"%s ... %s %s",
			 dshlines[n_dshlines-1].argv[0],
			 dshlines[n_dshlines-1].argv[
				 dshlines[n_dshlines-1].argc-2],
			 dshlines[n_dshlines-1].argv[
				 dshlines[n_dshlines-1].argc-1]);
	else if(dshlines[n_dshlines-1].argc==3)
		asprintf(&mess,"%s %s %s",
			 dshlines[n_dshlines-1].argv[0],
			 dshlines[n_dshlines-1].argv[1],
			 dshlines[n_dshlines-1].argv[2]);
	else if(dshlines[n_dshlines-1].argc==2)
		asprintf(&mess,"%s %s",
			 dshlines[n_dshlines-1].argv[0],
			 dshlines[n_dshlines-1].argv[1]);
	else if(dshlines[n_dshlines-1].argc==1)
		asprintf(&mess,"%s",
			 dshlines[n_dshlines-1].argv[0]);
	else
		asprintf(&mess," ");
	
	fatal("error in line %d\n"
	      "parsed so far: %s.%d %s\n"
	      "%s",
	      line_number,
	      dshlines[n_dshlines-1].verbase,
	      dshlines[n_dshlines-1].ver,
	      mess,
	      s);

	return 0;
}

void add_dshl(struct Version* version)
{
	static void * xrealloc (void *ptr, size_t size);
	n_dshlines++;
	if(n_dshlines>max_ind_dshlines) {
		max_ind_dshlines+=CHUNK;
		dshlines=(struct DshLine *)
			xrealloc(dshlines,
				 sizeof(struct DshLine)*max_ind_dshlines);
	}
	dshlines[n_dshlines-1].verbase=version->verbase;
	dshlines[n_dshlines-1].ver=version->ver;
	dshlines[n_dshlines-1].argc=0;
	dshlines[n_dshlines-1].argv=(char **)
		xrealloc(0,
			 sizeof(char *)*dshlines[n_dshlines-1].argc+1);
	dshlines[n_dshlines-1].argv[dshlines[n_dshlines-1].argc]=
		NULL;
	dshlines[n_dshlines-1].comc=0;
	dshlines[n_dshlines-1].comv=(struct Comment **)
		xrealloc(0,
			 sizeof(struct Comment *)
			 *dshlines[n_dshlines-1].comc+1);
	dshlines[n_dshlines-1].comv[dshlines[n_dshlines-1].comc]=
		NULL;
	dshlines[n_dshlines-1].dependc=0;
	dshlines[n_dshlines-1].dependv=(struct Version **)
		xrealloc(0,
			 sizeof(struct Version *)
			 *dshlines[n_dshlines-1].dependc+1);
	dshlines[n_dshlines-1].dependv[dshlines[n_dshlines-1].dependc]=
		NULL;
}

void add_global_comment(char *token)
{
	static void * xrealloc (void *ptr, size_t size);
	n_comments++;
	if(n_comments>max_ind_comments) {
		max_ind_comments+=CHUNK;
		comments=(struct Comment *)
			xrealloc(comments,
				 sizeof(struct Comment)*max_ind_comments);
	}
	comments[n_comments-1].token=token;
	comments[n_comments-1].type=ncom;
}

void add_arg(char *arg)
{
	static void * xrealloc (void *ptr, size_t size);
	dshlines[n_dshlines-1].argc++;
	dshlines[n_dshlines-1].argv=(char **)
		xrealloc(dshlines[n_dshlines-1].argv,
			 sizeof(char *)*dshlines[n_dshlines-1].argc+1);
	dshlines[n_dshlines-1].argv[dshlines[n_dshlines-1].argc-1]=
		arg;
	dshlines[n_dshlines-1].argv[dshlines[n_dshlines-1].argc]=
		NULL;
	
}

void add_dependency(struct Version *depend)
{
	static void fatal(const char *templ, ...);
	static void * xrealloc (void *ptr, size_t size);
	if(!n_dshlines) {
		errno=0;
		if(depend->verbase) {
			fatal("This describes a dependency:\n\t: %s-%s.%d\n"
			      "Its ought to be associated with a line in the dsh file\n"
			      "describing a version (wich in turn depends on another version).\n"
			      "It is only allowed after such a line!\n",
			      depend->source,depend->verbase,depend->ver);
		} else {
			fatal("This describes a dependency:\n\t: %s\n"
			      "Its ought to be associated with a line in the dsh file\n"
			      "describing a version (wich in turn depends on another version).\n"
			      "It is only allowed after such a line!\n",
			      depend->source);
		}
	}

	dshlines[n_dshlines-1].dependc++;
	dshlines[n_dshlines-1].dependv=(struct Version **)
		xrealloc(dshlines[n_dshlines-1].dependv,
			 sizeof(struct Version *)*dshlines[n_dshlines-1].dependc+1);
	dshlines[n_dshlines-1].dependv[dshlines[n_dshlines-1].dependc-1]=
		depend;
	dshlines[n_dshlines-1].dependv[dshlines[n_dshlines-1].dependc]=
		NULL;
	
}

void add_local_comment(char *token, int type)
{
	static void fatal(const char *templ, ...);
	static void * xrealloc (void *ptr, size_t size);
	if(!n_dshlines) {
		errno=0;
		fatal("This is a local comment:\n\t%s\n"
		      "Its ought to be associated with a line in the dsh file\n"
		      "describing a version. It is only allowed after such a line!\n"
		      "Use /* and */ to bracket comments at the beginnig of files.\n",
		      token);
	}
	dshlines[n_dshlines-1].comc++;
	dshlines[n_dshlines-1].comv=(struct Comment **)
		xrealloc(dshlines[n_dshlines-1].comv,
			 sizeof(struct Comment *)
			 *dshlines[n_dshlines-1].comc+1);
	dshlines[n_dshlines-1].comv[dshlines[n_dshlines-1].comc-1]=
		(struct Comment *)xrealloc(0,sizeof(struct Comment));
	dshlines[n_dshlines-1].comv[dshlines[n_dshlines-1].comc-1]->token=
		token;
	dshlines[n_dshlines-1].comv[dshlines[n_dshlines-1].comc-1]->type=
		type;
	dshlines[n_dshlines-1].comv[dshlines[n_dshlines-1].comc]=
		NULL;
	
}

void add_verp_arg(struct Version *verp)
{
	static void * xrealloc (void *ptr, size_t size);
	dshlines[n_dshlines-1].argc++;
	dshlines[n_dshlines-1].argv=(char **)
		xrealloc(dshlines[n_dshlines-1].argv,
			 sizeof(char *)*dshlines[n_dshlines-1].argc+1);
	asprintf(&(dshlines[n_dshlines-1].argv[dshlines[n_dshlines-1].argc-1]),
		 "%s.%d",verp->verbase,verp->ver);
	free(verp->verbase);
	dshlines[n_dshlines-1].argv[dshlines[n_dshlines-1].argc]=
		NULL;
}

static void * xrealloc (void *ptr, size_t size)
{
	static void fatal(const char *templ, ...);
	register void *value = realloc (ptr, size);
	if (value == 0)
		fatal ("Virtual memory exhausted");
	return value;
}

static void fatal(const char *templ, ...)
{
	char *tmp;
	va_list ap;

	va_start(ap,templ);
	vasprintf(&tmp,templ,ap);
	va_end(ap);

	if(errno)
		perror(tmp);
	else
		fprintf(stderr,"FATAL: %s\n",tmp);
	exit(1);
}
