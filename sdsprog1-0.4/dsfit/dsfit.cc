#include <stdio.h>
#include <SharedData.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <g2c.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>
#include "dsfit_y_l.h"

extern "C" {
extern int dnls1_(S_fp fcn, integer *iopt, integer *m, integer *n,
		  doublereal *x, doublereal *fvec, doublereal *fjac,
		  integer *ldfjac, doublereal *ftol, doublereal *xtol,
		  doublereal *gtol, integer *maxfev, doublereal *epsfcn,
		  doublereal *diag, integer *mode, doublereal *factor,
		  integer *nprint, integer *info, integer *nfev,
		  integer *njev, integer *ipvt, doublereal *qtf,
		  doublereal *wa1, doublereal *wa2,
		  doublereal *wa3, doublereal *wa4);
extern int  dcov_(S_fp fcn, integer *iopt, integer *m, integer *n,
		  doublereal *x, doublereal *fvec, doublereal *r,
		  integer *ldr, doublereal *sigma, integer *info,
		  doublereal *wa1, doublereal *wa2,
		  doublereal *wa3, doublereal *wa4);
extern E_f r1mach_(integer *i);
extern doublereal d1mach_(integer *i);
extern integer i1mach_(integer *i);

extern int dgam_(doublereal *a, doublereal *x, real *acc, doublereal *g, 
		 doublereal *gstar, integer *iflg, integer *iflgst);

double sigma(double x);
void print_sigma(FILE *out);
void sprint_sigma(char **line, int *ll, int *off);

char *readline(char *prompt);
void add_history(char * line);
int dsfit_yyparse(void);
extern FILE *dsfit_yyin;
extern int dsfit_yydebug;
extern struct DsFitParm yyparam;
}

doublereal gammaq(doublereal a,doublereal x);
void print_covar(FILE *out,doublereal *fjac,integer ldfjac);
void sprint_covar(SharedData *sd, char *line,
		  doublereal *fjac, integer ldfjac);
void initialize_arrays(size_t size,int n,...);
void inform_user(double min, double max, long int nop,
		 double ftol, double xtol, double gtol,
		 double chi2, double variance, double gamq,
		 double *fjac, long int ldfjac,
		 double *xp, long int np);
void interac_loop(int &id, int &newid, int &mode,
		  integer     &itera,	doublereal  &ftol,
		  doublereal  &xtol,	doublereal  &gtol,
		  double      &min,	double      &max,
		  integer     &nop,
//global     doublereal *&xp,
//global     integer     &np,
		  S_fp         fitfunc,	integer     &iopt,
		  doublereal *&x,	doublereal *&fvec,
		  doublereal *&fjac,	integer     &ldfjac,
		  doublereal  &epsfcn,	doublereal *&diag,
		  integer     &fmode,	doublereal  &factor,
		  integer     &nprint,	integer    *&ipvt,
		  doublereal *&qtf,	doublereal *&wa1,
		  doublereal *&wa2,	doublereal *&wa3,
		  doublereal *&wa4,
		  integer     &nnp,	doublereal *&xarr,
		  doublereal *&yarr,
		  doublereal  &chi2,	doublereal  &variance,
		  doublereal  &gamq,	integer     &newn,
		  SharedData *&sd);
void update_log(int id, int newid, int mode,
		double min,		double max,
		integer nop,		doublereal ftol,
		doublereal xtol,	doublereal gtol,
		double chi2,		doublereal variance,
		doublereal gamq,	doublereal *fjac,
		integer ldfjac, SharedData *sd);

/* Adaptation of this program to new fit function can be achieved by
 * supplying the function and if possible its derivative and by proper
 * setting of the macos and declaration between MARK A and B */

/* This is the setting for a polynomial fit. Ist a good example. So do
 * not remove the code but rather comment it out */

/* MARK A*/

doublereal polfit(doublereal *xp, integer np, doublereal x);
doublereal dpolfit(integer i, doublereal *, integer, doublereal x);

#define  FIT(a,b,c)  polfit(a,b,c)
#define DFIT(a,b,c,d) dpolfit(a,b,c,d)

#define IOPT 3
/* IOPT is an paramter of the fortran routine dnls1. The fortran code
 * contains comments which explains it as follows: */
/* IOPT is an input variable which specifies how the Jacobian will be
 * calculated.  If IOPT=2 or 3, then the user must supply the
 * Jacobian, as well as the function values, through the subroutine
 * FCN.  If IOPT=2, the user supplies the full Jacobian with one call
 * to FCN.  If IOPT=3, the user supplies one row of the Jacobian with
 * each call.  (In this manner, storage can be saved because the full
 * Jacobian is not stored.)  If IOPT=1, the code will approximate the
 * Jacobian by forward differencing. */

#define EPSFCN 0.0
/* The value determines the step size in the forward-difference
 * approximation for the Jacobian. It is therefore only relevant in
 * the case of IOPT=1. Normaly a value of zero is ok, in wich case
 * machine precision for the fit function is assumed. */

/* MARK B */ 

int fitfunc_(integer *iflag, integer *m, integer *n, doublereal *x, 
 	     doublereal *fvec, doublereal *fjac, integer *ldfjac);

void fitit(const char *idstr, integer itera, 
	   doublereal ftol, doublereal xtol,
	   doublereal gtol,
	   double min, double max, integer nop, int mode);


char * rl_gets (void);
pid_t fork_qpl(void) ;

/* global variabels:
 *   double xp[np]
 *   integer np
 *   integer ifi[np]
 *   doublereal yuse[m]
 *   doublereal xuse[m]
 */

doublereal *xp=NULL;
integer np;
integer *ifi=NULL;
doublereal *xuse;
doublereal *yuse;

static struct option const long_options[] =
{
	{"a"           , required_argument, 0, 300},
	{"b"           , required_argument, 0, 301},
	{"c"           , required_argument, 0, 302},
	{"d"           , required_argument, 0, 303},
	{"e"           , required_argument, 0, 304},
	{"f"           , required_argument, 0, 305},
	{"g"           , required_argument, 0, 306},
	{"h"           , required_argument, 0, 307},
	{"i"           , required_argument, 0, 308},
	{"j"           , required_argument, 0, 309},
	{"k"           , required_argument, 0, 310},
	{"l"           , required_argument, 0, 311},
	{"m"           , required_argument, 0, 312},
	{"n"           , required_argument, 0, 313},
	{"o"           , required_argument, 0, 314},
	{"p"           , required_argument, 0, 315},
	{"q"           , required_argument, 0, 316},
	{"r"           , required_argument, 0, 317},
	{"s"           , required_argument, 0, 318},
	{"t"           , required_argument, 0, 319},
	{"u"           , required_argument, 0, 320},
	{"v"           , required_argument, 0, 321},
	{"w"           , required_argument, 0, 322},
	{"x"           , required_argument, 0, 323},
	{"y"           , required_argument, 0, 324},
	{"z"           , required_argument, 0, 325},
	{"a?"           , required_argument, 0, 400},
	{"b?"           , required_argument, 0, 401},
	{"c?"           , required_argument, 0, 402},
	{"d?"           , required_argument, 0, 403},
	{"e?"           , required_argument, 0, 404},
	{"f?"           , required_argument, 0, 405},
	{"g?"           , required_argument, 0, 406},
	{"h?"           , required_argument, 0, 407},
	{"i?"           , required_argument, 0, 408},
	{"j?"           , required_argument, 0, 409},
	{"k?"           , required_argument, 0, 410},
	{"l?"           , required_argument, 0, 411},
	{"m?"           , required_argument, 0, 412},
	{"n?"           , required_argument, 0, 413},
	{"o?"           , required_argument, 0, 414},
	{"p?"           , required_argument, 0, 415},
	{"q?"           , required_argument, 0, 416},
	{"r?"           , required_argument, 0, 417},
	{"s?"           , required_argument, 0, 418},
	{"t?"           , required_argument, 0, 419},
	{"u?"           , required_argument, 0, 420},
	{"v?"           , required_argument, 0, 421},
	{"w?"           , required_argument, 0, 422},
	{"x?"           , required_argument, 0, 423},
	{"y?"           , required_argument, 0, 424},
	{"z?"           , required_argument, 0, 425},
	{"min"          , required_argument, 0, 'i'},
	{"max"          , required_argument, 0, 'a'},
	{"NoP"          , required_argument, 0, 'n'},
	{"ftol"         , required_argument, 0, 'f'},
	{"xtol"         , required_argument, 0, 'x'},
	{"gtol"         , required_argument, 0, 'g'},
	{"sigma"        , required_argument, 0, 'm'},
	{"itera"        , required_argument, 0, 't'},
	{"interactive"  , no_argument,       0, 'c'},
	{"replace"      , no_argument,       0, 'R'},
	{"residuum"     , no_argument,       0, 'r'},
	{"help"         , no_argument,       0, 'h'},
	{"version"      , no_argument,       0, 'V'},
	{"warranty"     , no_argument,       0, 'W'},
	{"copyright"    , no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dsfit.cc,v 1.1 2003/01/23 15:12:06 vstein Exp $";

extern char *program_invocation_short_name;

void warranty(void)
{
	printf("This program is free software; you can redistribute it"
               " and/or modify it\n"
               "under the terms of the GNU General Public License as"
	       " published by the \n"
	       "Free Software Foundation; either version 2 of the License,"
	       " or (at your \n"
	       "option) any later version.\n"
	       "\n"
	       "This program is distributed in the hope that it will be"
	       " useful, but \n"
	       "WITHOUT ANY WARRANTY; without even the implied warranty of"
	       " MERCHANTABILITY\n"
	       "or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General"
	       " Public License \n"
	       "for more details.\n"
	       "\n"
	       "You should have received a copy of the GNU General Public"
	       " License along \n"
	       "with this program; if not, write to the Free Software"
	       " Foundation, Inc., \n"
	       "59 Temple Place Suite 330, Boston, MA 02111-1307 USA.\n");
	exit(0);
}

void usage(void)
{
	fprintf(stderr,
	      "usage: %s [options] regexp(s)\n"
		"\n"
		"-i,    --min          boundary of approximation intervall\n"
		"-a,    --max          boundary of approximation intervall\n"
		"-n     --NoP          Number of Points\n"
		"-f     --ftol\n"
		"-x     --xtol\n"
		"-g     --gtol\n"
		"-t     --itera\n"
		"-m     --sigma        string like"
		                         " \"(0.0,.1|.2|.3),(1.0,.3|1.0)\"\n"
		"-c     --interactive  interactive mode\n"
		"-R     --replace      replace mode\n"
		"-r     --residuum     residuum mode\n"
		"-h,-?, --help         this message\n"
		"-V,    --version      display version information and exit\n"
		"-W,    --warranty     display licensing terms.\n"
		"-C,    --copyright  \n"
		"\n"
		"--[a-z]=double        presets a parameter\n",
		program_invocation_short_name);
	exit(0);
}

void copyright(void)
{
	printf("%s version %s, Copyright (C) 1999 Roland Schaefer\n"
	       "%s comes with ABSOLUTELY NO WARRANTY;\n"
	       "This is free software, and you are welcome to"
	       " redistribute it\n"
	       "under certain conditions; type `%s -W' for details.\n",
	       program_invocation_short_name,verstring,
	       program_invocation_short_name,program_invocation_short_name);
	exit(0);
}

void version(void)
{
	fprintf(stderr, "%s version %s\n", program_invocation_short_name,
		verstring);
	
	exit(0);
}

int main(int argc,char **argv)
{
	int i,c;
	integer ii;
	int nop;
	double min=1.0,max=-1.0;   /* max<min triggers automated
				    * setting of min and max in
				    * function fitit(...) */
	doublereal ftol,xtol,gtol;
	int itera;
	int opt_set=0;
	int mode=ACCEPT_MODE;

	pid_t pid;

//	sleep(3600);
	while ((c = getopt_long(argc, argv, 
				"i:a:n:f:x:g:t:m:crRh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 300: case 301: case 302: case 303: case 304:
		case 305: case 306: case 307: case 308: case 309:
		case 310: case 311: case 312: case 313: case 314:
		case 315: case 316: case 317: case 318: case 319:
		case 320: case 321: case 322: case 323: case 324:
		case 325:
			if(sscanf(optarg,"%lf",&yyparam.pars[c-300])!=1)
				usage();
			break;
		case 400: case 401: case 402: case 403: case 404:
		case 405: case 406: case 407: case 408: case 409:
		case 410: case 411: case 412: case 413: case 414:
		case 415: case 416: case 417: case 418: case 419:
		case 420: case 421: case 422: case 423: case 424:
		case 425:
			if(sscanf(optarg,"%lf",&yyparam.pars[c-400])!=1)
				usage();
			yyparam.fit[c-400]=1;
			break;
		case 'i':
			if(sscanf(optarg,"%lf",&min)!=1)
				usage();
			opt_set|=MIN_SET;
			break;
		case 'a':
			if(sscanf(optarg,"%lf",&max)!=1)
				usage();
			opt_set|=MAX_SET;
			break;
		case 'n':
			if(sscanf(optarg,"%d",&nop)!=1)
				usage();
			opt_set|=NOP_SET;
			break;
		case 'f':
			if(sscanf(optarg,"%lf",&ftol)!=1)
				usage();
			opt_set|=FTO_SET;
			break;
		case 'x':
			if(sscanf(optarg,"%lf",&xtol)!=1)
				usage();
			opt_set|=XTO_SET;
			break;
		case 'g':
			if(sscanf(optarg,"%lf",&gtol)!=1)
				usage();
			opt_set|=GTO_SET;
			break;
		case 'm':
			dsfit_yyin=tmpfile();
			fprintf(dsfit_yyin,"sigma=%s;",optarg);
			rewind(dsfit_yyin);
			//dsspl_yydebug=1;
			if(dsfit_yyparse())
				usage();
			opt_set|=SIG_SET;
			break;
		case 't':
			if(sscanf(optarg,"%d",&itera)!=1)
				usage();
			opt_set|=ITE_SET;
			break;
		case 'c':
			opt_set|=INT_SET;
			break;
		case 'r':
			opt_set|=RES_SET;
			break;
		case 'R':
			opt_set|=REP_SET;
			break;
		case 'V':
			version();
			break;
		case 'W':
			warranty();
			break;
		case 'C':
			copyright();
			break;
		case 'h':
		case '?':
		default:
			usage();
			break;
		}
	}

	if(!(opt_set&ITE_SET))
		itera=200;

	if(!(opt_set&NOP_SET))
		nop=-1;
	ii=4;
	if(!(opt_set&FTO_SET))
		ftol=sqrt(r1mach_(&ii));

	if(!(opt_set&XTO_SET))
		xtol=sqrt(r1mach_(&ii));

	if(!(opt_set&GTO_SET))
		gtol=0.0;

	if(opt_set&INT_SET) {
		pid=fork_qpl();
		mode|=INTERAC_MODE;
	}

	if(opt_set&RES_SET) {
		mode|=RES_MODE;
	} 
	if(opt_set&REP_SET) {
		mode|=REP_MODE;
	}

	for(i=optind;i<argc;i++)
		fitit(argv[i],itera, ftol,xtol,gtol,
		      min,max,nop,mode);

	if(pid>0)
		kill(pid,SIGTERM);
}

pid_t fork_qpl(void) 
{
	pid_t pid;

	pid = fork ();
	if (pid == 0) {
		/* This is the child process.  Execute the shell command. */
		execlp ("qpl","qpl","-i","98","-a","99",NULL);
		_exit (EXIT_FAILURE);
	}
	return pid;
}

void fitit(const char *idstr, integer itera, 
	   doublereal ftol, doublereal xtol,
	   doublereal gtol,
	   double min, double max, integer nop, int mode)
{
	SharedData *sd;
	int id,newid=DS_UNUSED;
	integer i;
	integer newn;

	sd=new SharedData();
	errno=0;
	id=strtol(idstr,NULL,0);
	if (errno) {
		printf ("ID Overflow in %s\n",idstr);
		delete sd;
		return;
	}
	if(!sd->hasid(id)) {
		printf("No id %d (%s)\n",id,idstr);
		delete sd;
		return;
	}

	if(max<min) { // we set min and max automatically
		double *x=sd->x(id);
		int n=sd->n(id);
		int i;
		if(n<=0) {
			delete sd;
			return;
		}
		min=max=x[0];
		for(i=0;i<n;i++) {
			min=( min<x[i] ? min : x[i]);
			max=( max>x[i] ? max : x[i]);
		}
	}

	// Parameter used in the call to dnls1:
	// 1. FCN
	S_fp fitfunc=(S_fp)fitfunc_;
	// 2. IOPT
	// IOPT=1, 2, or 3. See the Macro definition above
	integer iopt=IOPT;
	// 3. M
	// The number of points used in the fit 
        // It is adjusted to the number opf points 
	// between min and max below.
	integer mu;
	// 4. N 
	// The number of actuall fited parameter. Adjusted below.
	integer n=0;
	// 5. X
	// array of the fitting parameter.
	// Its length is adjusted dynamically to the
	// largest possible number of fitted parameters
	doublereal *x=NULL;
	// 6. FVEC
	// array with y values of fit result
	// Its length equals m=sd->n(id)
	doublereal *fvec=NULL;
	// 7. FJAC
	// array organized matrix. 
	// The fortran code of dnls1 explains it like this
	// FJAC is an output array.  For IOPT=1 and 2, FJAC is an M by N
        //   array.  For IOPT=3, FJAC is an N by N array.  The upper N by N
        //   submatrix of FJAC contains an upper triangular matrix R with
        //   diagonal elements of nonincreasing magnitude such that
	//
        //          T     T           T
        //         P *(JAC *JAC)*P = R *R,
	//
        //   where P is a permutation matrix and JAC is the final calcu-
        //   lated Jacobian.  Column J of P is column IPVT(J) (see below)
        //   of the identity matrix.  The lower part of FJAC contains
        //   information generated during the computation of R.
	doublereal *fjac=NULL;	
	// 8. LDFJAC
	// Leading dimension of the matrix fjac
	integer ldfjac;	
	// 9. FTOL
	// 10. XTOL
	// 11. GTOL
	// determines fit presision.
	// These parameters are supplied by the calling function
	// See fortran code of dnls1 for details
	// 12. MAXFEV
	// limits the calls to fitfunc,
	// Here it is named "itera" and supplied by the calling function
	// 13. EPSFCN
	// See macro definition above
	doublereal epsfcn=EPSFCN;
	// 14. DIAG
	// array.
	// Its length is adjusted dynamically to the
	// largest possible number of fitted parameters
	doublereal *diag=NULL;
	// 15. MODE
	// here mode=1 is used.  See documentation of fortran code for
	// dnls1 for details.
	integer fmode=1;
	// 16. FACTOR 
	// The recomended value 100 is used.  See documentation of
	// fortran code for dnls1 for details.
	doublereal factor=100.0;
	// 17. NPRINT
	// No printing! Therefore:
	integer nprint=-1;
	// 18. INFO
	// In the fortran code of dnls1 we find:
	// INFO is an integer output variable.  If the user has terminated
	//   execution, INFO is set to the (negative) value of IFLAG.  See
	//   description of FCN and JAC. Otherwise, INFO is set as follows
	//
	//    INFO = 0  improper input parameters.
	//
	//    INFO = 1  both actual and predicted relative reductions in the
	//              sum of squares are at most FTOL.
	//
	//    INFO = 2  relative error between two consecutive iterates is
	//              at most XTOL.
	//
	//    INFO = 3  conditions for INFO = 1 and INFO = 2 both hold.
	//
	//    INFO = 4  the cosine of the angle between FVEC and any column
	//              of the Jacobian is at most GTOL in absolute value.
	//
	//    INFO = 5  number of calls to FCN for function evaluation
	//              has reached MAXFEV.
	//
	//    INFO = 6  FTOL is too small.  No further reduction in the sum
	//              of squares is possible.
	//
	//    INFO = 7  XTOL is too small.  No further improvement in the
	//              approximate solution X is possible.
	//
	//    INFO = 8  GTOL is too small.  FVEC is orthogonal to the
	//              columns of the Jacobian to machine precision.
	integer info;
	// 19. NFEV
	// On each output of dnls1 the number of calls to fitfunc.
	integer nfev;
	// 20. NJEV
	// On each output from dnls1 the number of evaluations of
	// the full Jacobian.
	integer njev;
	// 21. IPVT
	// output array.
	// Its length is adjusted dynamically to the
	// largest possible number of fitted parameters
	integer *ipvt=NULL;
	// 22. QTF
	// output array.
	// Its length is adjusted dynamically to the
	// largest possible number of fitted parameters
	doublereal *qtf=NULL;
	// 23. WA1
	// work array.
	// Its length is adjusted dynamically to the
	// largest possible number of fitted parameters
	doublereal *wa1=NULL;
	// 24. WA2
	// work array.
	// Its length is adjusted dynamically to the
	// largest possible number of fitted parameters
	doublereal *wa2=NULL;
	// 25. WA3
	// work array.
	// Its length is adjusted dynamically to the
	// largest possible number of fitted parameters
	doublereal *wa3=NULL;
	// 26. WA4
	// work array.
	// Its length equals m=sd->n(id).
	doublereal *wa4=NULL;

	//Parameters used in the call to dcov:
	// 1. FCN         -------------------------- as in calls to dnls1
	// 2. IOPT        -------------------------- as in calls to dnls1
	// 3. M (here mu) -------------------------- as in calls to dnls1
	// 4. N           -------------------------- as in calls to dnls1
	// 5. X           -------------------------- as in calls to dnls1
	// 6. FVEC        -------------------------- as in calls to dnls1
	// 7. R
	// array organized matrix
	// The fortran code of dcov explains it like this
        // R is an output array.  For IOPT=1 and 2, R is an M by N array.
	//   For IOPT=3, R is an N by N array.  On output, if INFO=1,
        //   the upper N by N submatrix of R contains the covariance
        //   matrix evaluated at X.
	//                -------------------------- parameter fjac is used
	// 8. LDR
	// Leading dimension of r;
	//                -------------------------- parameer ldfjac is used
	// 9. SIGMA
	// SIGMA is chi^2/(mu-n). It can be used as an estimate of the 
	// error bars of the individual points in case sigma(x) is one.
	doublereal variance=0.0;
	// 10. INFO       -------------------------- as parameter 18
	//                                           in calls to dnls1
	// 11. WA1        -------------------------- as parameter 23
	//                                           in calls to dnls1
	// 12. WA2        -------------------------- as parameter 24
	//                                           in calls to dnls1
	// 13. WA3        -------------------------- as parameter 25
	//                                           in calls to dnls1
	// 14. WA4        -------------------------- as parameter 26
	//                                           in calls to dnls1
	

	integer nnp=0; // maximal number of fit parameters

	doublereal *xs;
	doublereal *ys;
	doublereal *xarr=NULL;
	doublereal *yarr=NULL;
	doublereal *xo, *yo;
	doublereal *xn,*yn;
	doublereal gamq=1.0;
	doublereal chi2=0.0;

	const integer m=sd->n(id);

	// arrays of fixed size m=sd->n(id)
	initialize_arrays(sizeof(doublereal)*m,4,
			  &xarr,&yarr,&fvec,&wa4);

	// initialize nnp to the maximal number of fit parameters
	for(i=0;i<26;i++)
		if(yyparam.pars[i]!=0||yyparam.fit[i])
			nnp=i+1;

	// is there anything to fit ?
	if(nnp) {
		initialize_arrays(sizeof(doublereal)*nnp,7,
				  &x,&diag,&qtf,&xp,&wa1,&wa2,&wa3);
		
		initialize_arrays(sizeof(integer)*nnp,2,
				  &ipvt,&ifi);

		if(iopt==1||iopt==2) {
			initialize_arrays(sizeof(doublereal)*nnp*m,1,
					  &fjac);
			ldfjac=m;
		} else {
			initialize_arrays(sizeof(doublereal)*nnp*nnp,1,
					  &fjac);
			ldfjac=nnp;
		}			
	}

	for(i=0;i<nnp;i++) {
		xp[i]=yyparam.pars[i];
		ifi[i]=yyparam.fit[i];
		if(xp[i]!=0||ifi[i]) {
			// np is a global variable which contains
			// i+1, where i is the index of highest indexed 
			// fit parameter
			np=i+1;
		}
	}

// Inteactive call to fitit
	if(mode&INTERAC_MODE) {
		interac_loop(id, newid, mode,
			     itera, ftol, xtol, gtol, min,  max,  nop,
			     fitfunc, iopt, x, fvec, fjac, ldfjac,
			     epsfcn, diag, fmode, factor, nprint,
			     ipvt, qtf, wa1, wa2, wa3, wa4,
			     nnp, xarr, yarr, 
			     chi2, variance, gamq,
			     newn, sd);
		if (mode&ABORT_MODE) 
			goto abort;
	}

// Noninteractive call to fitit
	for(i=0,n=0;i<np;i++)
		if(ifi[i])
			x[n++]=xp[i];
	
	// copy point in the intervall [min,max] to {x,y}arr
	for(xo=sd->x(id),yo=sd->y(id),mu=0,i=0;
	    i<m;
	    xo++,yo++,i++) {
		if(*xo>=min&&*xo<=max){
			xarr[mu]=*xo;
			yarr[mu++]=*yo;
		}
	}
	
	// condition of dnls1:
	if(mu<n)
		fatal("not enough points in the"
		      " interval %f<x<%f\n", min,max);
	
	if( (mode&RES_MODE)&&(nop!=-1) ) { 
		fprintf(stderr,"changing nop from %ld to -1\n",nop);
		nop=-1;
	}

	if (nop!=-1)
		newn=nop; // output at nop equidistance points
	                  // in [min,max]
	else 
		newn=m;   // output at x values of input

	xuse=xarr;
	yuse=yarr;
	
	if(n) {
		// fit..........
		dnls1_(fitfunc, &iopt, &mu, &n,
		       x, fvec, fjac, &ldfjac, 
		       &ftol, &xtol, &gtol,
		       &itera, &epsfcn,
		       diag, &fmode, &factor,
		       &nprint, &info, &nfev,
		       &njev, ipvt, qtf,
		       wa1, wa2, wa3, wa4);
		// cov.....
		dcov_(fitfunc, &iopt, &mu, &n,
		      x, fvec, fjac, &ldfjac, &variance,
		      &info, wa1, wa2, wa3, wa4);
	}
	
	chi2=variance*(mu-n);
	gamq=gammaq((mu-n)/2,chi2/2);

	if (mode&REP_MODE) {
		if (nop!=-1) {
			// resize old dataset
			sd->resize(id,newn);
			sd->set_n(id,newn);
			xs=sd->x(id);
			for(i=0;i<nop;i++)
				xs[i]=min+
					(double)i/(double)(newn-1)*
					(max-min);
		}
		xuse=sd->x(id);
		ys=sd->y(id);
		yuse=(doublereal *)xmalloc(sizeof(doublereal)*newn);
		if (mode &RES_MODE) {          // REP_MODE & RES_MODE
			for(i=0;i<newn;i++) {
				yuse[i]=ys[i];
			}
			i=1;
			fitfunc_(&i,&newn,&n,x,ys,NULL,NULL);
			for(i=0;i<newn;i++)
				ys[i]*=sigma(xuse[i]);
		} else {                       // REP_MODE & ~RES_MODE
			for(i=0;i<newn;i++)
				yuse[i]=0.0;
			i=1;
			fitfunc_(&i,&newn,&n,x,ys,NULL,NULL);
			for(i=0;i<newn;i++)
				ys[i]*=-sigma(xuse[i]);
		}
		free(yuse);
	} else {
		newid=sd->create(newn);
		sd->set_n(newid,newn);
		sd->set_plev(newid,sd->plev(id));
		printf("new id=%d\n",newid);
		xn=sd->x(newid);
		yn=sd->y(newid);
		if (mode&RES_MODE) {          // ~REP_MODE & RES_MODE
			int k;
			sd->set_ident(newid,(const char **)sd->ident(id,&k));
			sd->add_ident(newid,"%s -r",
				      program_invocation_short_name);
			xuse=sd->x(id);
			yuse=sd->y(id);
			i=1;
			// evaluate the y values
			fitfunc_(&i,&newn,&n,x,yn,NULL,NULL);
			for(i=0;i<newn;i++) {
				xn[i]=xuse[i];
				yn[i]*=sigma(xn[i]);
			}
		} else {                      // ~REP_MODE & ~RES_MODE
			int k;
			sd->set_ident(newid,(const char **)sd->ident(id,&k));
			sd->add_ident(newid,"%s",
				      program_invocation_short_name);
			yuse=(doublereal *)xmalloc(sizeof(doublereal)*newn);
			if(nop!=-1) {
				for(i=0;i<newn;i++) {
					xn[i]=min+
						(double)i/(double)(newn-1)*
						(max-min);
					yuse[i]=0.0;
				}
			} else {
				xs=sd->x(id);
				for(i=0;i<newn;i++) {
					xn[i]=xs[i];
					yuse[i]=0.0;
				}
			}
			i=1;
			xuse=xn;
			fitfunc_(&i,&newn,&n,x,yn,NULL,NULL);
			free(yuse);
			for(i=0;i<newn;i++)
				yn[i]*=-sigma(xn[i]);
		}
	}
			
	update_log(id, newid, mode, min, max, nop, ftol, xtol,
		   gtol, chi2, variance, gamq, fjac, ldfjac, sd);
	sd->changed();

 abort:
	free(wa1);
	free(wa2);
	free(wa3);
	free(wa4);
	free(x);
	free(fjac);
	free(diag);
	free(qtf);
	free(fvec);
	free(xarr);
	free(yarr);

	return;
}

char * rl_gets (void)
/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
{
	static char *lr = (char *)NULL;
	/* If the buffer has already been allocated, return the memory
	   to the free pool. */
	if (lr) {
		free (lr);
		lr = (char *)NULL;
	}
     
	/* Get a line from the user. */
	lr = readline (">");
     
	/* If the line has any text in it, save it on the history. */
	if (lr && *lr && strlen(lr)>3)
		add_history (lr);
	
	return (lr);
}

int fitfunc_(integer *iflag, integer *m, integer *n, doublereal *x, 
	     doublereal *fvec, doublereal *fjac, integer *ldfjac)
{
	int i,j,k;

	if(*iflag==0) {
		fatal("printing not implemented in fit function");
	}

	for(i=0,j=0;i<np;i++)
		if(ifi[i])
			xp[i]=x[j++];
	if(j!=*n)
		fatal("n!=j");
	if(*iflag==1) {
		for(i=0;i<*m;i++)
			fvec[i]=(yuse[i]-FIT(xp,np,xuse[i]))/sigma(xuse[i]);
		return 0;
	}

	if(*iflag==2) {
		for(i=0,j=0;i<np;i++){
			if(ifi[i]) {
				for(k=0;k<*m;k++)
					fjac[j*(*ldfjac)+k]=
						DFIT(i,xp,np,xuse[k])/
						sigma(xuse[k]);
				j++;
			}
		}
		if(j!=*n)
			fatal("n!=j");
		return 0;
	}

	if(*iflag==3)
	{
		for(i=0,j=0;i<np;i++){
			if(ifi[i]) {
				fjac[j++]=
					DFIT(i,xp,np,xuse[*ldfjac-1])/
					sigma(xuse[*ldfjac-1]);
			}
		}
		if(j!=*n)
			fatal("n!=j");
	}
	return 0;
}

doublereal polfit(doublereal *xp, integer np, doublereal x)
{
	int i;
	doublereal ret=0.0;
	for(i=np-1;i>0;i--) {
		ret+=xp[i];
		ret*=x;
	}
	if (np) 
		return ret+xp[0];

	return 0.0;
}

/* doublereal dpolfit(integer i, doublereal *xp, integer np, doublereal x) */
doublereal dpolfit(integer i, doublereal *, integer, doublereal x)
{
	return -pow(x,i);
}

extern "C" {
extern doublereal dgamit_(doublereal *a, doublereal *x);
}

doublereal gammaq(doublereal a,doublereal x)
{
	real acc=3;
	double g,gstar;
	integer iflg,iflgst;
	dgam_(&a,&x,&acc,&g,&gstar,&iflg,&iflgst);
	if(iflg==4)
		return 0;
	if(iflg==5)
		return NAN;
	return g;

}

void print_covar(FILE *out,doublereal *fjac,integer ldfjac)
{
	int i,ii;
	int j,jj;
	printf("\n");
	for(i=0,ii=0;i<np;i++) {
		if(ifi[i]) {
			for(j=i+1,jj=ii+1;j<np;j++) {
				if(ifi[j]) {
					printf("<%c,%c>=% #5.1f%% ",
					       (int)i+'a',(int)j+'a',
					       100*fjac[jj*ldfjac+ii]/
					       sqrt(fjac[jj*ldfjac+jj]*
						    fjac[ii*ldfjac+ii]));
					jj++;
				}
			}
			printf("\n");
			ii++;
			for(j=0;j<ii;j++)
				printf("             ");
		}
	}
	printf("\n");
}

void sprint_covar(char **logline,int *ll,int *off,
		 doublereal *fjac, integer ldfjac)
{
	int i,ii;
	int j,jj;
	*off+=asprintcat(logline,ll,*off,"\n# ");
	for(i=0,ii=0;i<np;i++) {
		if(ifi[i]) {
			for(j=i+1,jj=ii+1;j<np;j++) {
				if(ifi[j]) {
					*off+=asprintcat(logline,ll,*off,
							 "<%c,%c>=% #5.1f%% ",
							 (int)i+'a',(int)j+'a',
							 100*fjac[jj*
								 ldfjac+ii]/
							 sqrt(fjac[jj*
								  ldfjac+jj]*
							      fjac[ii*
								  ldfjac+ii]
								 )
						);
					jj++;
				}
			}
			*off+=asprintcat(logline,ll,*off,"\n# ");
			ii++;
			for(j=0;j<ii;j++)
				*off+=asprintcat(logline,ll,*off,
						 "             ");
		}
	}
}

void initialize_arrays(size_t size,int n,...)
{	
	va_list ap;
	int i;
	void **targ;

	va_start(ap,n);
	
	for(i=0;i<n;i++) {
		targ= va_arg(ap,void **);
		*targ=xrealloc(*targ,size);
	}
	
	va_end(ap);
}

void inform_user(double min, double max, long int nop,
		 double ftol, double xtol, double gtol,
		 double chi2, double variance, double gamq,
		 double *fjac, long int ldfjac,
		 double *xp, long int np)
{
	integer i,ii;
	printf("min=%g, max=%g, N=%ld, F=%g, X=%g, G=%g\n"
	       "chi^2=%g, variance= %g, gammaq=%g\n",
	       min,max,nop,ftol,xtol,gtol,chi2,variance,gamq);
	print_sigma(stdout);
	if(chi2>0) {
		print_covar(stdout,fjac,ldfjac);
	}
	for(i=0,ii=0;i<np;i++) {
		printf("%c= % #8.6g",
		       'a'+(int)i,
		       xp[i]);
		if((ifi[i])) {
			printf(" +/- %.2g\n", 
			       sqrt(fjac[ii*ldfjac+ii]));
			ii++;
		} else
			printf("\n");
	}
}

void interac_loop(int &id, int &newid, int &mode,
		  integer     &itera,	doublereal  &ftol,
		  doublereal  &xtol,	doublereal  &gtol,
		  double      &min,	double      &max,
		  integer     &nop,
//global     doublereal *&xp,
//global     integer     &np,
		  S_fp         fitfunc,	integer     &iopt,
		  doublereal *&x,	doublereal *&fvec,
		  doublereal *&fjac,	integer     &ldfjac,
		  doublereal  &epsfcn,	doublereal *&diag,
		  integer     &fmode,	doublereal  &factor,
		  integer     &nprint,	integer    *&ipvt,
		  doublereal *&qtf,	doublereal *&wa1,
		  doublereal *&wa2,	doublereal *&wa3,
		  doublereal *&wa4,
		  integer     &nnp,	doublereal *&xarr,
		  doublereal *&yarr,
		  doublereal  &chi2,	doublereal  &variance,
		  doublereal  &gamq,
		  integer     &newn,
		  SharedData *&sd)
{
	integer mu;
	integer n;
	integer info;
	integer nfev;
	integer njev;
	integer m;
	static int show_res=0;
	double yfac;

	int opl;                 //original plot level of set

	char *line;
	integer i,j;
	doublereal tmp;
	doublereal *xo,*yo;
	doublereal *xn,*yn;
	doublereal *xs;
	
	variance=0.0;
	chi2=0.0;
	gamq=0.0;

	opl=sd->plev(id);
	sd->set_plev(id,99);
	m=sd->n(id);
	
	while(1) {
		inform_user(min, max, nop, ftol, xtol, gtol, chi2,
			    variance, gamq, fjac,ldfjac, xp, np);
		
		delete sd;
		// readline
		line=rl_gets();
		sd= new SharedData();

		// check wether someone has stollen an id we have to rely on
		if(newid!=DS_UNUSED) {
			if(!sd->hasid(newid)) {
				printf("aborting after"
				       " intervening removal!\n");
				sd->set_plev(id,opl);
				return;
			}
		}
		if(!sd->hasid(id)) {
			printf("aborting after intervening removal!\n");
			sd->set_plev(id,opl);
			return;
		}

		// mark all param fields as unset
		yyparam.set=0;
		yyparam.parset=0;
		yyparam.mode=0;
		// parse readline input
		dsfit_yyin=tmpfile();
		fprintf(dsfit_yyin,"%s",line);
		rewind(dsfit_yyin);
		//dsspl_yydebug=1;
		if(dsfit_yyparse())
			continue;
		rewind(dsfit_yyin);
		fclose(dsfit_yyin);
		// set parameter found in parsing input
		if(yyparam.set&TR_SET)
			if(show_res)
				show_res=0;
			else
				show_res=1;
		if(yyparam.set&MIN_SET)
			min=yyparam.min;
		if(yyparam.set&MAX_SET)
			max=yyparam.max;
		if(yyparam.set&NOP_SET)
			nop=yyparam.nop;
		if(yyparam.set&FTO_SET)
			ftol=yyparam.ftol;
		if(yyparam.set&XTO_SET)
			xtol=yyparam.xtol;
		if(yyparam.set&GTO_SET)
			gtol=yyparam.gtol;
		if(yyparam.set&ITE_SET)
			itera=yyparam.itera;
		for(i=0;i<26;i++) {
			if(yyparam.parset&1<<i) {
				if((i+1)>nnp) {
					// realloc arrays with 
					// enlarged size
					np=nnp;
					nnp=i+1;
					initialize_arrays(
						sizeof(doublereal)*nnp,7,
						&x,&diag,&qtf,&xp,
						&wa1,&wa2,&wa3);
					initialize_arrays(
						sizeof(integer)*nnp,2,
						&ipvt,&ifi);
					if(iopt==1||iopt==2) {
						initialize_arrays(
							sizeof(doublereal)*
							nnp*m,1,
							&fjac);
						ldfjac=m;
					} else {
						initialize_arrays(
							sizeof(doublereal)*
							nnp*nnp,1,
							&fjac);
						ldfjac=nnp;
					}
					// set unused arguments to zero
					for(j=np;j<i;j++) {
						xp[j]=0.0;
						ifi[j]=0;
					}
				}
				xp[i]=yyparam.pars[i];
				ifi[i]=yyparam.fit[i];
			}
		}

		for(i=0;i<nnp;i++)
			if(xp[i]!=0.0||ifi[i]) {
				// np is a global variable
				// which contains i+1, where i
				// is the index of highest
				// indexed fit parameter
				np=i+1;
			}

		// correct for strange input
		if(min>max) {
			tmp=min;
			min=max;
			max=tmp;
		}

		// are we done ?
		if(yyparam.mode) {
			mode|=yyparam.mode;
			break;
		}

		// anything to do in this turn?
		if(!np)
			continue;

			
		// off we go!
		// spool the actual fit parameter to array x
		for(i=0,n=0;i<np;i++)
			if(ifi[i])
				x[n++]=xp[i];
		// copy point in the intervall [min,max] to {x,y}arr
		for(xo=sd->x(id),yo=sd->y(id),mu=0,i=0;
		    i<m;
		    xo++,yo++,i++) {
			if(*xo>=min&&*xo<=max){
				xarr[mu]=*xo;
				yarr[mu++]=*yo;
			}
		}

		// condition of dnls1:
		if(mu<n) {
			printf("not enough points in the"
			       " interval %f<x<%f\n", min,max);
			continue;
		}
		
		if(nop==-1) {
			// output at x values of input
			newn=m;
		} else {
			// output at nop equidistancial points in [min,max]
			newn=nop;
		}


		if (newid==DS_UNUSED) { // create new dataset
			newid=sd->create(newn);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"auxiliary data of dsfit");
			sd->set_plev(newid,98);
			sd->set_linecolor(newid,"Gray");
			sd->set_linewidth(newid,3);
		} else if (newn!=(int)sd->n(newid)) {
			sd->resize(newid,newn);
			sd->set_n(newid,newn);
		}
		xn=sd->x(newid);
		yn=sd->y(newid);

		xuse=xarr;
		yuse=yarr;
		if(n) {
			// fit......
			dnls1_(fitfunc, &iopt, &mu, &n,
			       x, fvec, fjac, &ldfjac, 
			       &ftol, &xtol, &gtol,
			       &itera, &epsfcn,
			       diag, &fmode, &factor,
			       &nprint, &info, &nfev,
			       &njev, ipvt, qtf,
			       wa1, wa2, wa3, wa4);
			// cov.....
			dcov_(fitfunc, &iopt, &mu, &n,
			      x, fvec, fjac, &ldfjac, &variance,
			      &info, wa1, wa2, wa3, wa4);
		}
			
		chi2=variance*(mu-n);
		gamq=gammaq((mu-n)/2.0,chi2/2);

		// spool result to xp and yyparam
		for(i=0,n=0;i<np;i++)
			if(ifi[i])
				yyparam.pars[i]=xp[i]=x[n++];
		// plot the result
		yuse=(doublereal*)xmalloc(sizeof(doublereal)*newn);
		if(nop==-1) { // plot at x valuse of input
			xs=sd->x(id);
			if(show_res) {
				yfac=1.0;
				double *ys=sd->y(id);
				for(i=0;i<newn;i++) {
					xn[i]=xs[i];
					yuse[i]=ys[i];
				} 
			} else {
				yfac=-1.0;
				for(i=0;i<newn;i++) {
					xn[i]=xs[i];
					yuse[i]=0.0;
				}
			}
		} else { // plot at nop point between min and max
			yfac=-1.0;
			for(i=0;i<newn;i++) {
				xn[i]=min+(double)i/(double)
					(newn-1)*(max-min);
				yuse[i]=0.0;
			}
		}
		i=1;
		xuse=xn;
		// evaluate the y values
		fitfunc_(&i,&newn,&n,x,yn,NULL,NULL);
		free(yuse);
		for(i=0;i<newn;i++)
			yn[i]*=yfac*sigma(xn[i]);

		sd->set_n(newid,newn);
		sd->changed();
	}
	// remove former newid if any
	if(newid!=DS_UNUSED) {
		sd->remove(newid);
		newid=DS_UNUSED;
	}
	sd->set_plev(id,opl);
	return;
}


void update_log(int id, int newid, int mode,
		double min,		double max,
		integer nop,		doublereal ftol,
		doublereal xtol,	doublereal gtol,
		double chi2,		doublereal variance,
		doublereal gamq,	doublereal *fjac,
		integer ldfjac, SharedData *sd)
{
	char *logline=NULL;
	int ll=0,off=0;

	integer i,ii;

	if(mode&REP_MODE) 
		off=asprintcat(&logline,&ll,0,"<%d>\t%s",id,
			       program_invocation_short_name);
	else
		off=asprintcat(&logline,&ll,0,"<*%d*:%d>\t%s",newid,id,
			       program_invocation_short_name);

	if(mode&REP_MODE)
		off+=asprintcat(&logline,&ll,off," -R");
	if(mode&RES_MODE)
		off+=asprintcat(&logline,&ll,off," -r");

	off+=asprintcat(&logline,&ll,off," -i %g -a %g",min,max);

	if(nop!=-1)
		off+=asprintcat(&logline,&ll,off," -n %ld",nop);

		
	for(i=0;i<np;i++) {
		off+=asprintcat(&logline,&ll,off," --%c",'a'+(int)i);
		if(ifi[i])
			off+=asprintcat(&logline,&ll,off,"?");
		off+=asprintcat(&logline,&ll,off," %g",xp[i]);
	}
	sprint_sigma(&logline,&ll,&off);
	off+=asprintcat(&logline,&ll,off," $%d",id);

	off+=asprintcat(&logline,&ll,off,"\n# min=%g, max=%g, N=%ld, F=%g,"
			" X=%g, G=%g", min,max,nop,ftol,xtol,gtol);
	off+=asprintcat(&logline,&ll,off,"\n# chi^2=%g, variance= %g,"
			" gammaq=%g", chi2,variance,gamq);

	if(chi2>0) {
		sprint_covar(&logline,&ll,&off,fjac,ldfjac);
	}

	for(i=0,ii=0;i<np;i++) {
		if((ifi[i])) {
			off+=asprintcat(&logline,&ll,off, 
					"\n# %c= % #8.6g +/- %.2g",
					'a'+(int)i, xp[i], 
					sqrt(fjac[ii*ldfjac+ii]));
			ii++;
		} else
			off+=asprintcat(&logline,&ll,off,
					"\n# %c= % #8.6g",
					'a'+(int)i, xp[i]);
	}	

	sd->log(logline);
	free(logline);
}

