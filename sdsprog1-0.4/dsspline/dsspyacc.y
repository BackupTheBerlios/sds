%{
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <stdarg.h>
#include "dssp_y_l.h"


struct DssplParm yyparam={0,   /* set   */
			  0.0, /* min   */
			  0.0, /* max   */
			  0,   /* nop   */
			  0.0, /* sfac  */
			  0,   /* order */
			  0,   /* iopt  */
			  0,   /* mode  */
};

static struct SigmaPoint *sigpoints;
static int nsigpoints;
static int nadded;

int yyerror(char *s);
int yylex( void );

static void add_global_sigma(double sigma);
static void add_sigma(double x, double l,double e,double r);
static void delete_sigma(double x);
static int compare_sigpoints(const void *a, const void *b);
double sigma(double);

%}

%token YYERROR_VERBOSE

%union
{
	int ival;
	double fval;
}

%token	T_MIN	T_MAX	T_NOP	T_SFA	T_ORD	T_OK	T_ABORT	T_IOPT
%token	T_RES   T_REP	T_TR

%token	T_INT	T_SIG

%token	T_DOUBLE 

%type <ival> T_INT

%type <fval> T_DOUBLE

%%
        line:   exp
                | exp oneexp
                | error
                ; 


	exp:    /* empty */
		| exp oneexp ';'
		;

	oneexp: T_MIN T_DOUBLE  { yyparam.min=$2; yyparam.set|=MIN_SET; }
		| T_MIN T_INT  { yyparam.min=$2; yyparam.set|=MIN_SET; }
		| T_MAX T_DOUBLE  { yyparam.max=$2; yyparam.set|=MAX_SET; }
		| T_MAX T_INT  { yyparam.max=$2; yyparam.set|=MAX_SET; }
		| T_NOP T_INT  { yyparam.nop=$2; yyparam.set|=NOP_SET; }
		| T_SFA T_DOUBLE  { yyparam.sfac=$2; yyparam.set|=SFA_SET; }
		| T_SFA T_INT  { yyparam.sfac=$2; yyparam.set|=SFA_SET; }
		| T_ORD T_INT  { yyparam.order=$2; yyparam.set|=ORD_SET; }
		| T_IOPT T_INT  { yyparam.iopt=$2; yyparam.set|=INT_SET; }
		| T_TR  { yyparam.set|=TR_SET; }
                | T_OK     { yyparam.mode|=ACCEPT_MODE; }
		| T_ABORT  { yyparam.mode|=ABORT_MODE; }
                | T_RES    { yyparam.mode|=RES_MODE; }
                | T_REP    { yyparam.mode|=REP_MODE; }
		| T_SIG sigma_exp  {
			yyparam.set|=SIG_SET;
			if(nadded) {
				nsigpoints+=nadded;
				qsort(sigpoints,nsigpoints,
				      sizeof(struct SigmaPoint),
				      compare_sigpoints);
				nadded=0;
			}
		}
		;

	sigma_exp: sigma_sequence
		| '"' sigma_sequence '"'
		| T_DOUBLE { add_global_sigma($1);}
		;

	sigma_sequence: sigma_single
		| sigma_sequence sigma_single
		;

	sigma_single:  '(' T_DOUBLE ',' T_DOUBLE ')' { 
			add_sigma($2,$4,$4,$4); }
		| '(' T_DOUBLE ',' '|' T_DOUBLE ')' {
			add_sigma($2,-1.0,-1.0,$5); }
		| '(' T_DOUBLE ',' '|' T_DOUBLE '|' ')' {
			add_sigma($2,-1.0,$5,-1.0); }
		| '(' T_DOUBLE ',' '|' T_DOUBLE '|' T_DOUBLE ')' {
			add_sigma($2,-1.0,$5,$7); }
		| '(' T_DOUBLE ',' T_DOUBLE '|' ')' {
			add_sigma($2,$4,-1.0,-1.0); }
		| '(' T_DOUBLE ',' T_DOUBLE '|' T_DOUBLE ')' {
			add_sigma($2,$4,($4+$6)/2.0,$6); }
		| '(' T_DOUBLE ',' T_DOUBLE '|' T_DOUBLE '|' ')' {
			add_sigma($2,$4,$6,-1.0); }
		| '(' T_DOUBLE ',' T_DOUBLE '|' T_DOUBLE '|' T_DOUBLE ')' {
			add_sigma($2,$4,$6,$8); }
		| '(' T_DOUBLE ',' T_DOUBLE '|' '|' T_DOUBLE ')' {
			add_sigma($2,$4,-1.0,$7); }
		| '(' T_DOUBLE ',' ')' {
			delete_sigma($2); }
		;
%%

int yyerror(char *s)
{
  printf("error: %s\n",s);
  return 0;
}

static int compare_sigpoints(const void *a, const void *b)
{
	double *da=&(((struct SigmaPoint*)a)->x);
	double *db=&(((struct SigmaPoint*)b)->x);
	
	return (*da>*db) - (*db>*da);
}

static void add_global_sigma(double sigma)
{
	if(sigpoints) {
		free(sigpoints);
		sigpoints=NULL;
		nsigpoints=0;
		nadded=0;
	}
	if(fabs(1-sigma)>DBL_EPSILON)
		add_sigma(0.0,sigma,sigma,sigma);	
}

static void add_sigma(double x,double l,double e, double r)
{
	static void * xrealloc (void *ptr, size_t size);
	int i;

	for(i=0;i<nsigpoints+nadded;i++) {
		if(fabs(x-sigpoints[i].x)<DBL_EPSILON) {
			if(l>0)
				sigpoints[i].l=l;
			if(e>0)
				sigpoints[i].e=e;
			if(r>0)
				sigpoints[i].r=r;
			return;
		}
	}

	sigpoints=(struct SigmaPoint*)
		xrealloc(sigpoints, sizeof(struct SigmaPoint)*
			 (nsigpoints+nadded+1));
	sigpoints[nsigpoints+nadded].x=x;
/*	if(l>0) {
		sigpoints[nsigpoints+nadded].l=l;
		if(e<=0)
			sigpoints[nsigpoints+nadded].e=l;
		if(r<=0)
			sigpoints[nsigpoints+nadded].r=l;
	}
	if(e>0) {
		sigpoints[nsigpoints+nadded].e=e;
		if(l<=0)
			sigpoints[nsigpoints+nadded].l=e;
		if(r<=0)
			sigpoints[nsigpoints+nadded].r=e;
	}
	if(r>0) {
		sigpoints[nsigpoints+nadded].r=r;
		if(l<=0)
			sigpoints[nsigpoints+nadded].l=r;
		if(e<=0)
			sigpoints[nsigpoints+nadded].e=r;
	}
	if(l<=0&&e<=0&&r<=0) {
		sigpoints[nsigpoints+nadded].l=1.0;
		sigpoints[nsigpoints+nadded].e=1.0;
		sigpoints[nsigpoints+nadded].r=1.0;
	}
*/
	if(l>0)
		sigpoints[nsigpoints+nadded].l=l;
	else
		sigpoints[nsigpoints+nadded].l=sigma(x);
	if(e>0) 
		sigpoints[nsigpoints+nadded].e=e;
	else
		sigpoints[nsigpoints+nadded].e=sigma(x);
	if(r>0)
		sigpoints[nsigpoints+nadded].r=r;
	else
		sigpoints[nsigpoints+nadded].r=sigma(x);
	nadded++;
/* 	qsort(sigpoints,nsigpoints, */
/* 	      sizeof(struct SigmaPoint),compare_sigpoints); */
}

static void delete_sigma(double x) 
{
	static void * xrealloc (void *ptr, size_t size);
	int i;
	int n=0;

	for(i=0;i+n<nsigpoints+nadded;i++)
		if(fabs(x-sigpoints[i].x)<DBL_EPSILON) {
			memmove(sigpoints+i,sigpoints+(i+1),
			       sizeof(struct SigmaPoint)*
			       ((nsigpoints+nadded)-(i+1)));
			n++;
			i--;
		}

	sigpoints=(struct SigmaPoint*)
		xrealloc(sigpoints,sizeof(struct SigmaPoint)*
			 nsigpoints+nadded-n);
	nsigpoints-=n;
}

static void * xrealloc (void *ptr, size_t size)
{
	static void fatal(const char *templ, ...);
	register void *value = realloc (ptr, size);
	if (value == 0)
		fatal ("Virtual memory exhausted");
	return value;
}

double sigma(double x)
{
	int i,j,k;
	double h;
	
	if(nsigpoints<=0)
		return 1.0;

	if(sigpoints[0].x-x>=DBL_EPSILON)
		return sigpoints[0].l;

	j=nsigpoints-1;
	if(x-sigpoints[j].x>=DBL_EPSILON)
		return sigpoints[j].r;

	if(nsigpoints==1)
		return sigpoints[0].e;
		
	if(fabs(sigpoints[j].x-x)<DBL_EPSILON)
		return sigpoints[j].e;

	i=0;
	while(j-i>1) {
		k=(i+j)/2;
		if(sigpoints[k].x>x)
			j=k;
		else
			i=k;
	}

	if(fabs(x-sigpoints[i].x)<DBL_EPSILON)
		return sigpoints[i].e;

	h=(x-sigpoints[i].x)/(sigpoints[j].x-sigpoints[i].x);
	
	return sigpoints[i].r+(sigpoints[j].l-sigpoints[i].r)*h;
}

void print_sigpoint(FILE *out, struct SigmaPoint *sp)
{
	if(fabs(sp->l-sp->e)<DBL_EPSILON &&
	   fabs(sp->e-sp->r)<DBL_EPSILON)
		fprintf(out,"(%g,%g)",sp->x,sp->e);
	else
		fprintf(out,"(%g,%g|%g|%g)",sp->x,sp->l,sp->e,sp->r);
}

int asprintcat(char **dest, int *dl, int off, const char *fstr, ...)
{
	static void fatal(const char *templ, ...);
	char *tmp;
	size_t len;
	va_list ap;

	va_start(ap,fstr);
	vasprintf(&tmp,fstr,ap);
	va_end(ap);
	
	len = strlen(tmp);

	errno=0;
	if (off + len + 1 >(unsigned int) *dl) {
		*dl = 100 + off + len;
		*dest = (char *) realloc (*dest, *dl);
		if(!*dest)
			fatal("realloc error in asprintcat");
	}
	memcpy(*dest+off,tmp,len);
	*(*dest+off+len)='\0';
	free(tmp);
	return len;
}

void sprint_sigpoint(char **line, int *ll,int *off, struct SigmaPoint *sp)
{
	if(fabs(sp->l-sp->e)<DBL_EPSILON &&
	   fabs(sp->e-sp->r)<DBL_EPSILON)
		*off+=asprintcat(line,ll,*off,"(%g,%g)",sp->x,sp->e);
	else
		*off+=asprintcat(line,ll,*off,
			      "(%g,%g|%g|%g)",sp->x,sp->l,sp->e,sp->r);
}

void print_sigma(FILE *out)
{
	int i;
	if(nsigpoints==0) {
		fprintf(out,"sigma=1.0\n");
		return;
	}
	if(nsigpoints==1&&
	   fabs(sigpoints[0].l-sigpoints[0].e)<DBL_EPSILON&&
	   fabs(sigpoints[0].e-sigpoints[0].r)<DBL_EPSILON) {
		fprintf(out,"sigma=%g\n",sigpoints[0].e);
		return;
	}
	fprintf(out,"sigma=");
	print_sigpoint(out,sigpoints);
	for(i=1;i<nsigpoints;i++) {
		if(i!=1&&!((i-1)%5))
			fprintf(out,"\n");
		print_sigpoint(out,sigpoints+i);
	}
	fprintf(out,"\n");
}

void sprint_sigma(char **line, int *ll, int *off)
{
	int i;
	if(nsigpoints==0) {
		*off+=asprintcat(line,ll,*off," -m 1.0");
		return;
	}
	if(nsigpoints==1&&
	   fabs(sigpoints[0].l-sigpoints[0].e)<DBL_EPSILON&&
	   fabs(sigpoints[0].e-sigpoints[0].r)<DBL_EPSILON) {
		*off+=asprintcat(line,ll,*off," -m %g",sigpoints[0].e);
		return;
	}
	*off+=asprintcat(line,ll,*off," -m \"");
	sprint_sigpoint(line,ll,off,sigpoints);
	for(i=1;i<nsigpoints;i++)
		sprint_sigpoint(line,ll,off,sigpoints+i);
	*off+=asprintcat(line,ll,*off,"\"");
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
