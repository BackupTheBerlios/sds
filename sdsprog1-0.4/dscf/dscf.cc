#include <stdio.h>
#include <SharedData.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <g2c.h>
#include <setjmp.h>
#include "dscf_y_l.h"

extern "C" {
char *readline(char *prompt);
void add_history(char * line);
int dscfl_yyparse(void);
extern FILE *dscfl_yyin;
extern int dscfl_yydebug;
extern struct DscfParm yyparam;
}

void inform_user(double min, double max, int nop,
		 int iopt, double rms, double pc, double pspan, 
		 double rms2, double pc2,
		 double ms, int slor, double tc, int flty,
		 double *p, double lnP, int iter);

void update_log(int id, int newid, integer mode,
		double min, double max, int nop, int iopt,
		double rms, double pc, double pspan,
		double rms2, double pc2,
		double ms,  int slor,  double tc, int flty,
		double *p, double lnP, int iter,
		SharedData *sd);

void fitit(const char *idstr, int mode,
	   double min, double max, int nop, int iopt,
	   double rms, double pc, double pspan,
	   double rms2, double pc2,
	   double ms,  int slor,  double tc, int flty);

char * rl_gets (void);
pid_t fork_qpl(void);
pid_t pid=0;

double _rms_;
double _pc_;
double _time_constant_;
double _mean_square_;
int _filter_slope_;
int _filter_type_;

double lnPspecial(double *x, double *xa, double *ya, int n);
int dlnPspecial(double *x, double *erg,double *xa, double *ya, int n);
double pspecial(double nu, double *p);

struct lnPParam{
	double *x,*y;
	int n;
};

/* Signal handling in interactive mode: SIGINT should be used to
   interrupt the FORTRAN routine when looping forever.  However, the
   qpl child process should _not_ terminate.
   A second handler is used to abort the interactive loop in a clean way.
*/
sigjmp_buf tnbccall, loopcall;

void terminate_tnbc(int signum) {
	siglongjmp(tnbccall,1);
}

void terminate_loop(int signum) {
	siglongjmp(loopcall,1);
}

void terminate_qpl(int signum) {
	if (pid>0)
		kill(pid,SIGTERM);
	signal(signum, SIG_DFL);
	raise(signum);
}
static struct option const long_options[] =
{
	{"rms"          , required_argument, 0, 300},
	{"pc"           , required_argument, 0, 301},
	{"pspan"        , required_argument, 0, 302},
	{"ms"           , required_argument, 0, 303},
	{"slope_order"  , required_argument, 0, 304},
	{"tc"           , required_argument, 0, 305},
	{"rms2"         , required_argument, 0, 306},
	{"pc2"          , required_argument, 0, 307},
	{"filter_type"  , required_argument, 0, 308},
	{"min"          , required_argument, 0, 'i'},
	{"max"          , required_argument, 0, 'a'},
	{"NoP"          , required_argument, 0, 'n'},
	{"opt"          , required_argument, 0, 'o'},
	{"interactive"  , no_argument,       0, 'c'},
	{"replace"      , no_argument,       0, 'R'},
	{"residuum"     , no_argument,       0, 'r'},
	{"help"         , no_argument,       0, 'h'},
	{"version"      , no_argument,       0, 'V'},
	{"warranty"     , no_argument,       0, 'W'},
	{"copyright"    , no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dscf.cc,v 1.1 2003/01/23 15:12:06 vstein Exp $";

extern char *program_invocation_short_name;

lnPParam lnppar;

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
	      "usage: %s [options] ID(s)\n"
		"special fit function for conductance fluctuations\n"
		"\n"
		"-i,    --min          boundary of fit intervall\n"
		"-a,    --max          boundary of fit intervall\n"
		"-n     --NoP          Number of Points\n"
		"       --rms          fit parameter: Amplitude\n"
		"       --pc           fit parameter: correlation field\n"
		"       --pspan        scaling parameter: field span\n"
		"       --rms2         auxiliary UCF amplitude\n"
		"       --pc2          auxiliary UCF correlation field\n"
		"       --ms           noise amplitude\n"
		"       --slope_order  order of filter\n"
		"       --tc           filter cutoff frequency\n"
		"       --filter_type  0: exponential, 1: rectangle\n"
		"-o     --opt          mode: 0: no fit 1: fit\n"
		"-c     --interactive  interactive mode\n"
		"-R     --replace      replace mode\n"
		"-r     --residuum     residuum mode\n"
		"-h,-?, --help         this message\n"
		"-V,    --version      display version information and exit\n"
		"-W,    --warranty     display licensing terms.\n"
		"-C,    --copyright  \n",
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
	int nop, iopt,slor,flty;
	double rms,pc,pspan,rms2,pc2,ms,tc;
	double min=1.0,max=-1.0;   /* max<min triggers automated
				    * setting of min and max in
				    * function fitit(...) */

	int opt_set=0;
	integer mode=ACCEPT_MODE;
 
	while ((c = getopt_long(argc, argv, "i:a:n:o:crRh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 300:
			if(sscanf(optarg,"%lf",&rms)!=1)
				usage();
			opt_set|=RMS_SET;
			break;
		case 301:
			if(sscanf(optarg,"%lf",&pc)!=1)
				usage();
			opt_set|=PC_SET;
			break;
		case 302: 
			if(sscanf(optarg,"%lf",&pspan)!=1)
				usage();
			opt_set|=PSPAN_SET;
			break;
		case 303:
			if(sscanf(optarg,"%lf",&ms)!=1)
				usage();
			opt_set|=MS_SET;
			break;
		case 304:
			if(sscanf(optarg,"%i",&slor)!=1)
				usage();
			opt_set|=SLOR_SET;
			break;
		case 305:
			if(sscanf(optarg,"%lf",&tc)!=1)
				usage();
			opt_set|=TC_SET;
			break;
		case 306:
			if(sscanf(optarg,"%lf",&rms2)!=1)
				usage();
			opt_set|=RMS2_SET;
			break;
		case 307:
			if(sscanf(optarg,"%lf",&pc2)!=1)
				usage();
			opt_set|=PC2_SET;
			break;
		case 308:
			if(sscanf(optarg,"%i",&flty)!=1)
				usage();
			opt_set|=FLTY_SET;
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
		case 'o':
			if(sscanf(optarg,"%d",&iopt)!=1)
				usage();
			if(iopt!=0&&iopt!=1)
				usage();
			opt_set|=OPT_SET;
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


	if(!(opt_set&RMS_SET))
		rms=1.0;

	if(!(opt_set&PC_SET))
		pc=1.0;

	if(!(opt_set&PSPAN_SET))
		pspan=1.0;

	if(!(opt_set&NOP_SET))
		nop=-1;

	if(!(opt_set&OPT_SET))
		iopt=1;

	if(!(opt_set&MS_SET))
		ms=0;

	if(!(opt_set&SLOR_SET))
		slor=1;

	if(!(opt_set&TC_SET))
		tc=0;

	if(!(opt_set&FLTY_SET))
		flty=0;

	if(!(opt_set&RMS2_SET))
		rms2=0;

	if(!(opt_set&PC2_SET))
		pc2=1;

	if(opt_set&INT_SET) {
		pid=fork_qpl();
		mode|=INTERAC_MODE;
	}

	if(opt_set&RES_SET) 
		mode|=RES_MODE;

	if(opt_set&REP_SET) 
		mode|=REP_MODE;

	for(i=optind;i<argc;i++)
		fitit(argv[i], mode, min, max, nop, iopt, rms, pc, pspan, 
		      rms2, pc2, ms, slor, tc, flty);

	if(pid>0)
		kill(pid,SIGTERM);
}

pid_t fork_qpl(void) 
{
	pid_t pid;
	sigset_t procset, chldset;

	/* We want to use SIGINT to stop the fit routine, but
           qpl should keep on running.  The child inherits the
	   process sigmask, so we (temporarily) block SIGINT. */
	sigemptyset(&chldset);
	sigaddset(&chldset,SIGINT);
	sigprocmask(SIG_BLOCK,&chldset,&procset);
	pid = fork ();
	if (pid == 0) {
		/* This is the child process.  Execute the shell command. */
		execlp ("qpl","qpl","-i","98","-a","99",NULL);
		_exit (EXIT_FAILURE);
	}
	sigprocmask(SIG_SETMASK,&procset,NULL);
	return pid;
}


extern "C" {
extern int tnbc_(integer *ierror, integer *n, doublereal *x, doublereal *f, 
		 doublereal *g, doublereal *w, integer *lw, S_fp sfun,
		 doublereal *low, doublereal *up, integer *ipivot);
extern doublereal mchpr1_();
}

void fitit(const char *idstr, int mode,
	   double min, double max, int nop, int iopt,
	   double rms, double pc, double pspan, double rms2, double pc2,
	   double ms,  int  slor, double tc, int flty)
{
	SharedData *sd;
	int id,newid=DS_UNUSED;
	int i,j;
	static int show_res=0;

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

	const integer m=sd->n(id);
 	doublereal *x=sd->x(id);
 	doublereal *y=sd->y(id);
	doublereal *xo;
	doublereal *yo;

	double p1[2];
	double lnP=0;
	int iter=0;
	double al=sqrt(sqrt(2.0)-1.0);
	int newn;
	double *xn,*yn;
	double tmp;
	char *line;
	int opl;
	int sfun(integer *n, doublereal *x, doublereal *f, doublereal *g);

	doublereal f;
	integer np=2;
	integer lw=np*14;
	integer ierror;
	doublereal w[28],p[2],low[2],up[2],g[2];
	integer ipivot[2];


//set globale variable:
	_filter_slope_=slor;
	_time_constant_=tc;
	_mean_square_=ms;
	_rms_=rms2*rms2*M_PI*pc2/2/pspan;
	_pc_=pc2/al;
	_filter_type_=flty;
		
//interactive call to fitit
	if (mode&INTERAC_MODE) {
		opl=sd->plev(id);
		sd->set_plev(id,99);
	}
	while(mode&INTERAC_MODE) {
		p1[1]=pc/al;
		p1[0]=rms*rms*M_PI*p1[1]/2/pspan;
		
		inform_user(min,  max, nop,  iopt,
			    rms,  pc,  pspan, rms2, pc2, ms, slor, tc, flty,
			    p1, lnP, iter);
		sd->changed();
		
		delete sd;

		// set up signal handler to leave loop on SIGINT
		signal(SIGINT, terminate_loop);
		if (sigsetjmp(loopcall,1)) {
			printf("\nAborting on SIGINT!\n");
			sd= new SharedData();
  			if(newid!=DS_UNUSED)
 				sd->remove(newid);
			sd->set_plev(id,opl);
			sd->changed();
			// restore default handler
			signal(SIGINT, terminate_qpl);
			raise(SIGINT);
		}

		// readline
		line=rl_gets();
		sd= new SharedData();
		
		// check wether someone has stolen an id we have to 
		// rely on
		if(newid!=DS_UNUSED) {
			if(!sd->hasid(newid)) {
				printf("aborting after"
				       " intervening removal!");
				sd->set_plev(id,opl);
				sd->changed();
				delete sd;
				signal(SIGINT, terminate_qpl);
				return;
			}
		}
		if(!sd->hasid(id)) {
			printf("aborting after intervening removal!");
  			if(newid!=DS_UNUSED)
 				sd->remove(newid);
			sd->set_plev(id,opl);
			sd->changed();
			signal(SIGINT, terminate_qpl);
			delete sd;
			return;
		}
		
		// mark all param fields as unset
		yyparam.set=0;
		yyparam.mode=0;
		// parse readline input
		dscfl_yyin=tmpfile();
		fprintf(dscfl_yyin,"%s",line);
		rewind(dscfl_yyin);
		//dscfl_yydebug=1;
		if(dscfl_yyparse())
			continue;
		fclose(dscfl_yyin);
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
		if(yyparam.set&RMS_SET)
			rms=yyparam.rms;
		if(yyparam.set&PC_SET)
			pc=yyparam.pc;
		if(yyparam.set&PSPAN_SET)
			pspan=yyparam.pspan;
		if(yyparam.set&RMS2_SET) {
			rms2=yyparam.rms2;
			_rms_=rms2*rms2*M_PI*pc2/2/pspan;
		}
		if(yyparam.set&PC2_SET) {
			pc2=yyparam.pc2;
			_pc_=pc2/al;
		}
		if(yyparam.set&SWAP_SET) {
			double swap;
			swap=rms2; rms2=rms; rms=swap;
			swap=pc2;  pc2=pc;   pc=swap;
			_rms_=rms2*rms2*M_PI*pc2/2/pspan;
			_pc_=pc2/al;
		}
		if(yyparam.set&MS_SET) {
			ms=yyparam.ms;
			_mean_square_=ms;
		}
		if(yyparam.set&SLOR_SET) {
			slor=yyparam.slor;
			_filter_slope_=slor;
		}
		if(yyparam.set&TC_SET) {
			tc=yyparam.tc;
			_time_constant_=tc;
		}
		if(yyparam.set&FLTY_SET) {
			flty=yyparam.flty;
			_filter_type_=flty;
		}
		if(yyparam.set&OPT_SET) {
			iopt=yyparam.iopt;
			if(iopt<0||iopt>1) {
				printf("iopt is set to zero."
				       " Try also iopt=1\n");
				iopt=0;
			}
		}

		// correct for strange input
		if(min>max) {
			tmp=min;
			min=max;
			max=tmp;
		}
		
		// are we done ?
		if(yyparam.mode) {
			sd->set_plev(id,opl);
			if(newid!=DS_UNUSED)
				sd->remove(newid);
			sd->changed();
			// restore signal handler
			signal(SIGINT, terminate_qpl);
			
			if (yyparam.mode&ABORT_MODE)
				return;

			mode|=yyparam.mode;
			// go to non-interactive block
			break;
		}
			
		
		// off we go
		for(xo=sd->x(id),yo=sd->y(id),i=0;
		    i<m && (*xo<min || *xo>max);
		    xo++,yo++,i++);
		for(j=0;
		    i<m && (*xo<=max && *xo>=min);
		    xo++,j++,i++);
		if(j<=2) {
			printf("not enough points in the"
			       " interval %f<x<%f\n", min,max);
			continue;
		}
		
		xo-=j;
		
		p1[1]=pc/al;
		p1[0]=rms*rms*M_PI*p1[1]/2/pspan;
		
		if (iopt==1) {
			
			lnppar.x=xo;
			lnppar.y=yo;
			lnppar.n=j;
			
			p[0]=p1[0];
			p[1]=p1[1];
			low[0]=0;
			low[1]=0;
			up[0]=1;
			up[1]=1;

			// prepare signal handler to interrupt tnbc_ 
			if (signal(SIGINT, terminate_tnbc)==SIG_IGN) {
				printf("ignoring SIGINT\n");
				signal(SIGINT, SIG_IGN);
			}
			if  (sigsetjmp(tnbccall,1)) {
				/* we caught a SIGINT!! */
				printf("Call to fit routine interrupted!\n");
				continue;
			} 

			tnbc_(&ierror,&np,p,&f,g,w,&lw,(S_fp)sfun,
			      low,up,ipivot);
			lnP=f;
			p1[0]=p[0];
			p1[1]=p[1];
		}
		
		rms=sqrt(2*pspan*fabs(p1[0]/p1[1])/M_PI);
		pc=fabs(p1[1])*al;

		if(nop==-1)
			newn=m;
		else
			newn=nop;

		if (newid==DS_UNUSED) { 
			newid=sd->create(newn);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"auxilary data of dscf");
			sd->set_plev(newid,98);
			sd->set_linecolor(newid,"Gray");
			sd->set_linewidth(newid,3);
		} else if (newn!=(int) sd->n(newid)) {
			sd->resize(newid,newn);
			sd->set_n(newid,newn);
		}
		xn=sd->x(newid);
		yn=sd->y(newid);

		if(nop==-1) {
			if(show_res)
				for(i=0;i<newn;i++) {
					xn[i]=x[i];
					yn[i]=y[i]-pspecial(xn[i],p1);
				}
			else
				for(i=0;i<newn;i++) {
					xn[i]=x[i];
					yn[i]=pspecial(xn[i],p1);
				}
			
		} else {
			for(i=0;i<newn;i++) {
				xn[i]=min+(double)i/(double)
					(newn-1)*(max-min);
				yn[i]=pspecial(xn[i],p1);
			}
			
		}
	}

//non-interactive call to fitit

	for(xo=sd->x(id),yo=sd->y(id),i=0;
	    i<m && (*xo<min || *xo>max);
	    xo++,yo++,i++);
	for(j=0;i<m && (*xo<=max && *xo>=min);xo++,j++,i++);
	if(j<=2) {
		fprintf(stderr,"not enough points in the"
			" interval %f<x<%f\n", min,max);
		return;
	}
	xo-=j;
	
	p1[1]=pc/al;
	p1[0]=rms*rms*M_PI*p1[1]/2/pspan;
		
	if (iopt==1) {
		lnppar.x=xo;
		lnppar.y=yo;
		lnppar.n=j;
		
		p[0]=p1[0];
		p[1]=p1[1];
		low[0]=0;
		low[1]=0;
		up[0]=1;
		up[1]=1;
		
		tnbc_(&ierror,&np,p,&f,g,w,&lw,(S_fp)sfun,
		      low,up,ipivot);
		lnP=f;
		p1[0]=p[0];
		p1[1]=p[1];
		
	}
	
	rms=sqrt(2*pspan*fabs(p1[0]/p1[1])/M_PI);
	pc=fabs(p1[1])*al;

	if( (mode&RES_MODE) && (nop!=-1) ) {
		fprintf(stderr,"changing nop from %d to -1\n",nop);
		nop=-1;
	} 

	if(nop==-1)
		newn=m;
	else
		newn=nop;
	
	if (mode&REP_MODE) {
		if (nop!=-1) {
			// resize old set
			sd->resize(id,newn);
			sd->set_n(id,newn);
		}
		x=sd->x(id);
		y=sd->y(id);
		if (mode&RES_MODE) {  // REP_MODE & RES_MODE
			for(i=0;i<newn;i++) {
				y[i]-=pspecial(x[i],p1);
			}
		} else {              // REP_MODE & ~RES_MODE
			if(nop!=-1) {
				for(i=0;i<newn;i++) {
					x[i]=min+(double)i/(double)(newn-1)
						*(max-min);
					y[i]=pspecial(x[i],p1);
				}
			} else {
				for(i=0;i<newn;i++) {
					y[i]=pspecial(x[i],p1);
				}
			}
			
		}
	} else {
		newid=sd->create(newn);
		sd->set_n(newid,newn);
		sd->set_plev(newid,sd->plev(id));
		printf("new id=%d\n",newid);
		xn=sd->x(newid);
		yn=sd->y(newid);
		x=sd->x(id);
		y=sd->y(id);
		if (mode&RES_MODE) {  // ~REP_MODE & RES_MODE
			int k;
			sd->set_ident(newid,(const char **)sd->ident(id,&k));
			sd->add_ident(newid,"%s -r",
				      program_invocation_short_name);
			for(i=0;i<newn;i++) {
				xn[i]=x[i];
				yn[i]=y[i]-pspecial(x[i],p1);
			}
		} else {              // ~REP_MODE & ~RES_MODE
			int k;
			sd->set_ident(newid,(const char **)sd->ident(id,&k));
			sd->add_ident(newid,"%s",
				      program_invocation_short_name);
			if(nop!=-1) {
				for(i=0;i<newn;i++) {
					xn[i]=min+
						(double)i/(double)(newn-1)*
						(max-min);
					yn[i]=pspecial(xn[i],p1);
				}
			} else {
				for(i=0;i<newn;i++) {
					xn[i]=x[i];
					yn[i]=pspecial(xn[i],p1);
				}
			}
		}
	}		

	update_log(id,newid,mode,
		   min, max, nop, iopt, rms, pc, pspan, rms2, pc2, 
		   ms, slor, tc, flty, p1, lnP, iter, sd);
	return;
	
}

double sincsq(double x)
{
	if (x<1.0e-6)
		return 1;
	else if (fabs(sin(x))<1e-4)
		return 1e8;
	else
		return pow(sin(x)/x,-2) ;
}

double pspecial(double nu, double *p)
{
	double nu2p=nu*2*M_PI;
	double nu2ptc=nu2p*_time_constant_;
	double iff=(_filter_type_ 
		    ? pow(sincsq(nu2ptc),_filter_slope_)
		    : pow(1+nu2ptc*nu2ptc,_filter_slope_));
	return (fabs(p[0])*(1+fabs(p[1])*nu2p)*exp(-fabs(p[1])*nu2p)+
		_rms_*(1+_pc_*nu2p)*exp(-_pc_*nu2p)+
		_mean_square_)/iff;
}

double pspecial_p0(double nu, double *p)
{
	double nu2p=nu*2*M_PI;
	double nu2ptc=nu2p*_time_constant_;
	double iff=(_filter_type_ 
		    ? pow(sincsq(nu2ptc),_filter_slope_)
		    : pow(1+nu2ptc*nu2ptc,_filter_slope_));
	return (1+fabs(p[1])*nu2p)*exp(-fabs(p[1])*nu2p)/iff;
}

double pspecial_p1(double nu, double *p)
{
	double nu2p=nu*2*M_PI;
	double nu2ptc=nu2p*_time_constant_;
	double iff=(_filter_type_ 
		    ? pow(sincsq(nu2ptc),_filter_slope_)
		    : pow(1+nu2ptc*nu2ptc,_filter_slope_));
	return -fabs(p[0])*fabs(p[1])*nu2p*nu2p*exp(-fabs(p[1])*nu2p)/iff;
}

double lnPspecial(double *x, double *xa, double *ya, int n)
{
  int i;
  double sum;

  for(i=0,sum=0.0;i<n;i++)
    {
      double ps=pspecial(xa[i],x);
      sum+=ya[i]/ps+log(ps);
    }

  //  sum=sin(M_PI*x[0])*cos(M_PI*x[1]);
  return sum/10000.0;
}

int dlnPspecial(double *x, double *erg,double *xa, double *ya, int n)
{
  int i;

  for(i=0,erg[0]=erg[1]=0.0;i<n;i++)
    {
      double ps=pspecial(xa[i],x);
      double dabf=(ps-ya[i])/(ps*ps);
      erg[0]+=dabf*pspecial_p0(xa[i],x);
      erg[1]+=dabf*pspecial_p1(xa[i],x);
    }

  //  erg[0]=M_PI*cos(M_PI*x[0])*cos(M_PI*x[1]);
  //  erg[1]=-M_PI*sin(M_PI*x[0])*sin(M_PI*x[1]);

  erg[0]/=10000.0;
  erg[1]/=10000.0;

  return 0;
}

int sfun(integer *n, doublereal *x, doublereal *f, doublereal *g)
{

	*f=lnPspecial(x,lnppar.x,lnppar.y,lnppar.n);
	dlnPspecial(x,g,lnppar.x,lnppar.y,lnppar.n);

	return 0;
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

void inform_user(double min, double max, int nop,
		 int iopt, double rms,
		 double pc, double pspan, double rms2, double pc2,
		 double ms, int slor, double tc, int flty,
		 double *p, 
		 double lnP, int iter)
{
	printf("i=%f, a=%f, n=%d, iopt=%d, pspan=%f\n",
	       min,max,nop,iopt,pspan);
	printf("rms2=%g, pc2=%g, ms=%g, slor=%d, flt=%s, tc=%g\n", rms2, pc2,
	       ms, slor, (flty ? "rec" : "exp"), tc);
	printf("rms=%f, pc=%f, P_0=%f, beta=%f\n",
	       rms, pc, p[0], p[1]);
	if (iopt==1)
		printf("iter=%d, lnP=%f\n", 
		       iter, lnP);
}


void update_log(int id, int newid, integer mode,
		double min, double max, int nop, int iopt,
		double rms, double pc, double pspan, double rms2, double pc2,
		double ms, int slor, double tc, int flty,
		double *p, double lnP, int iter,
	        SharedData *sd)
{
	char *logline=NULL;
	int ll=0,off=0;

	if(mode&REP_MODE) 
		off=asprintcat(&logline,&ll,0,"<%d>\t%s",id,
			       program_invocation_short_name);
	else
		off=asprintcat(&logline,&ll,0,"<*%d*:%d>\t%s",newid,id,
			       program_invocation_short_name);

	if(mode&REP_MODE)
		off+=asprintcat(&logline,&ll,off," -R");
	else if(mode&RES_MODE)
		off+=asprintcat(&logline,&ll,off," -r");
		
	off+=asprintcat(&logline,&ll,off," -i %g -a %g",min,max);

	if(nop!=-1)
		off+=asprintcat(&logline,&ll,off," -n %ld",nop);

	off+=asprintcat(&logline,&ll,off,
			" --rms %g --pc %g --pspan %g",rms, pc, pspan);

	off+=asprintcat(&logline,&ll,off,
			" --rms2 %g --pc2 %g --ms %g --slope_order %d "
			"--filter_type %d --tc %g",
			rms2, pc2, ms, slor, flty, tc);

	if (iopt!=1)
		off+=asprintcat(&logline,&ll,off, " -o %d",iopt);
	
	off+=asprintcat(&logline,&ll,off," $%d",id);

	if (iopt!=0) 
		off+=asprintcat(&logline,&ll,off,
				"\n# rms=%g, pc=%g, P_0=%g,"
				" beta=%g, iter=%d",
				rms,pc,p[0],p[1],iter);

	sd->log(logline);
	free(logline);
}
