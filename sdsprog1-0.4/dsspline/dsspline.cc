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
#include <setjmp.h>
#include "dssp_y_l.h"

extern "C" {
extern int curfit_(integer *iopt, integer *m, doublereal *x, doublereal *y,
		   doublereal *w, doublereal *xb, doublereal *xe, integer *k,
		   doublereal *s, integer *nest, integer *n, doublereal *t,
		   doublereal *c, doublereal *fp, doublereal *wrk,
		   integer *lwrk, integer *iwrk, integer *ier);
extern int splev_(doublereal *t, integer *n, doublereal *c, integer *k,
		  doublereal *x, doublereal *y, integer *m, integer *ier);

double sigma(double x);
void print_sigma(FILE *out);
void sprint_sigma(char **line, int *ll, int *off);


char *readline(char *prompt);
void add_history(char * line);
int dsspl_yyparse(void);
extern FILE *dsspl_yyin;
extern int dsspl_yydebug;
extern struct DssplParm yyparam;
}

void inform_user(double min,		double max,	integer nop,
		 doublereal sfac,	integer iopt,	doublereal fp,
		 doublereal *t,		integer n,	integer ier,
		 integer order);
void update_log(int id, int newid, integer mode,
 		double min,		double max,	integer nop,
		doublereal sfac,	integer iopt,	doublereal fp,
 		doublereal *t,		integer n,	integer ier,
		integer order,		SharedData *sd);


void splineit(const char *idstr, integer order, double sfac, integer iopt,
	      double min, double max, integer nop, integer mode);
char * rl_gets (void);
pid_t fork_qpl(void) ;
pid_t pid=0;

/* Signal handling in interactive mode: SIGINT should be used to
   interrupt the FORTRAN routine when looping forever.  However, the
   qpl child process should _not_ terminate.
   A second handler is used to abort the interactive loop in a clean way.
*/
sigjmp_buf splinecall, loopcall;

void terminate_spline(int signum) {
	siglongjmp(splinecall,1);
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
	{"min"          , required_argument, 0, 'i'},
	{"max"          , required_argument, 0, 'a'},
	{"NoP"          , required_argument, 0, 'n'},
	{"s-factor"     , required_argument, 0, 'f'},
	{"spline-order" , required_argument, 0, 'k'},
	{"opt"          , required_argument, 0, 'o'},
	{"sigma"        , required_argument, 0, 'm'},
	{"interactive"  , no_argument,       0, 'c'},
	{"replace"      , no_argument,       0, 'R'},
	{"residuum"     , no_argument,       0, 'r'},
	{"help"         , no_argument,       0, 'h'},
	{"version"      , no_argument,       0, 'V'},
	{"warranty"     , no_argument,       0, 'W'},
	{"copyright"    , no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dsspline.cc,v 1.1 2003/01/23 15:12:07 vstein Exp $";

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
		"-k     --spline-order 1,2,3,4 or 5, defdaults to 3\n"
		"-m     --sigma        string like"
		                         " \"(0.0,.1|.2|.3),(1.0,.3|1.0)\"\n"
		"-o     --opt          mode: -1 or 0; too hard to explain\n"
		"-c     --interactive  interactive mode\n"
		"-R     --replace      replace mode\n"
		"-r     --residuum     residuum mode\n"
		"-f     --s-factor     smoothing factor\n"
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
	int order;
	int nop;
	int iopt;
	double min=1.0,max=-1.0;   /* max<min triggers automated
				    * setting of min and max in
				    * function fitit(...) */
	double sfac;
	int opt_set=0;
	integer mode=ACCEPT_MODE;
 
	while ((c = getopt_long(argc, argv, "i:a:n:f:k:o:m:crRh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
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
		if(sscanf(optarg,"%lf",&sfac)!=1)
				usage();
			opt_set|=SFA_SET;
			break;
		case 'k':
			if(sscanf(optarg,"%d",&order)!=1)
				usage();
			if(order<1||order>5)
				usage();
			opt_set|=ORD_SET;
			break;
		case 'o':
			if(sscanf(optarg,"%d",&iopt)!=1)
				usage();
			if(iopt!=0&&iopt!=-1)
				usage();
			opt_set|=OPT_SET;
			break;
		case 'm':
			dsspl_yyin=tmpfile();
			fprintf(dsspl_yyin,"sigma=%s;",optarg);
			rewind(dsspl_yyin);
			//dsspl_yydebug=1;
			if(dsspl_yyparse())
				usage();
			opt_set|=SIG_SET;
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

	if(!(opt_set&ORD_SET))
		order=3;

	if(!(opt_set&NOP_SET))
		nop=-1;

	if(!(opt_set&SFA_SET))
		sfac=0.0;

	if(!(opt_set&OPT_SET))
		iopt=0;

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
		splineit(argv[i], order, sfac, iopt,
			 min, max, nop, mode);


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

#define IOPT_UNSET -2

void splineit(const char *idstr, integer order, double sfac, integer iopt,
	      double min, double max, integer nop, integer mode)
{
	SharedData *sd;
	int id,newid=DS_UNUSED;
	integer i,j;
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
	doublereal *w=(doublereal*)xmalloc(sizeof(doublereal)*m);
	integer nest=m+order+1;
	integer n=0;
	doublereal *t=(doublereal*)xmalloc(sizeof(doublereal)*nest);
	doublereal *c=(doublereal*)xmalloc(sizeof(doublereal)*nest);
	doublereal fp=0.0;
	integer lwrk=m*(order+1)+nest*(7+3*order);
	doublereal *wrk=(doublereal*)xmalloc(sizeof(doublereal)*lwrk);
	integer *iwrk=(integer*)xmalloc(sizeof(integer)*nest);
	integer ier=0;
	integer ier1=0;
	integer oldiopt=IOPT_UNSET;
	integer newn;
	doublereal *xn,*yn;
	doublereal tmp;
	char *line;
	int opl;
	
//interactive call to splineit
	if (mode&INTERAC_MODE) {
		opl=sd->plev(id);
		sd->set_plev(id,99);
		sd->changed();
	}
	while(mode&INTERAC_MODE) {
		inform_user(min, max, nop, sfac, iopt, fp,
			    t, n, ier, order);

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
				signal(SIGINT, terminate_qpl);
				goto abort;
			}
		}
		if(!sd->hasid(id)) {
			printf("aborting after intervening removal!");
  			if(newid!=DS_UNUSED)
 				sd->remove(newid);
			sd->set_plev(id,opl);
			sd->changed();
			signal(SIGINT, terminate_qpl);
			goto abort;
		}
			
		x=sd->x(id);
		y=sd->y(id);
			
		// mark all param fields as unset
		yyparam.set=0;
		yyparam.mode=0;
		// parse readline input
		dsspl_yyin=tmpfile();
		fprintf(dsspl_yyin,"%s",line);
		rewind(dsspl_yyin);
		//dsspl_yydebug=1;
		if(dsspl_yyparse()) {
			printf("whoops!\n");
			continue;
		}
		fclose(dsspl_yyin);
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
		if(yyparam.set&SFA_SET)
			sfac=yyparam.sfac;
		if(yyparam.set&ORD_SET) {
			order=yyparam.order;
			nest=m+order+1;
			lwrk=m*(order+1)+nest*(7+3*order);
			t=(doublereal*)xrealloc(t,
						sizeof(doublereal)
						*nest);
			c=(doublereal*)xrealloc(c,
						sizeof(doublereal)
						*nest);
			wrk=(doublereal*)xrealloc(wrk,
						  sizeof(doublereal)
						  *lwrk);
			iwrk=(integer*)xrealloc(iwrk,
						sizeof(integer)
						*nest);
		}
		if(yyparam.set&INT_SET) {
			iopt=yyparam.iopt;
			if(iopt<-1||iopt>1) {
				printf("iopt is set to zero."
				       " Try also iopt=-1 or"
				       " iopt=1.\n");
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
			if(newid!=DS_UNUSED)
				sd->remove(newid);
			sd->set_plev(id,opl);
			sd->changed();
			// restore signal handler
			signal(SIGINT, terminate_qpl);
			if (yyparam.mode&ABORT_MODE)
				goto abort;
			mode|=yyparam.mode;
			// go to non-interacitve block
			break;
		}
			
		// off we go
		for(xo=sd->x(id),yo=sd->y(id),i=0;
		    i<m && (*xo<min || *xo>max);
		    xo++,yo++,i++);
		for(j=0;
		    i<m && j<1 && (*xo<=max && *xo>=min);
		    xo++,j++,i++);
		for(;
		    i<m && (*xo<=max && *xo>=min);
		    xo++,j++,i++) {
			if(*(xo-1)>=*xo) {
				fprintf(stderr,"unsorted data encountered\n");
				if(newid!=DS_UNUSED)
					sd->remove(newid);
				sd->set_plev(id,opl);
				sd->changed();
				signal(SIGINT, terminate_qpl);
				goto abort;
			}
		}

		if(j<=order) {
			printf("not enough points in the"
			       " interval %f<x<%f\n", min,max);
			continue;
		}
			
		xo-=j;
		
		for(i=0;i<j;i++)
			w[i]=1/sigma(xo[i]);
		if(oldiopt==IOPT_UNSET && iopt!=0) {
			printf("iopt is set to zero"
			       " in the first run.\n");
			iopt=0;
		}
		if(oldiopt==1&&iopt==-1) {
			printf("Do not change iopt from 1 to -1"
			       " directly. Decrease it gradually.\n"
			       "We first use iopt=0.\n");
			iopt=0;
		}

		// prepare signal handler to interrupt curfit_ 
		if (signal(SIGINT, terminate_spline)==SIG_IGN) {
			printf("ignoring SIGINT\n");
			signal(SIGINT, SIG_IGN);
		}
		if  (sigsetjmp(splinecall,1)) {
			/* we caught a SIGINT!! Should be OK, except
			   for iopt ?? */
			printf("Call to spline fit interrupted. "
			       "Please check parameters!\n");
			continue;
		} 
		curfit_(&iopt, &j, xo, yo, w, &min, &max, &order, 
			&sfac, &nest, &n, t, c, &fp, wrk, &lwrk,
			iwrk, &ier);
		oldiopt=iopt;

		if(nop==-1)
			newn=m;
		else
			newn=nop;
		
		if(newid==DS_UNUSED) {
			newid=sd->create(newn);
			sd->set_n(newid,newn);
			sd->add_ident(newid,"auxilary data of dsspline");
			sd->set_plev(newid,98);
			sd->set_linecolor(newid,"Gray");
			sd->set_linewidth(newid,3);
		} else if (newn!=(int)sd->n(newid)) {
			sd->resize(newid,newn);
			sd->set_n(newid,newn);
		}
		xn=sd->x(newid);
		yn=sd->y(newid);

		if(nop==-1) {
			for(i=0;i<newn;i++) 
				xn[i]=x[i];
		} else {
			for(i=0;i<newn;i++) 
				xn[i]=min+(double)i/(double)
					(newn-1)*(max-min);
		}
		splev_(t,&n,c,&order,xn,yn,&newn,&ier1);
		if(show_res&&nop==-1)
			for(i=0;i<newn;i++)
				yn[i]=y[i]-yn[i];

		sd->changed();

	}

//non-interactive call to splineit
	for(xo=sd->x(id),yo=sd->y(id),i=0;
	    i<m && (*xo<min || *xo>max);
	    xo++,yo++,i++);
	for(j=0;
	    i<m && j<1 && (*xo<=max && *xo>=min);
	    xo++,j++,i++);
	for(;
	    i<m && (*xo<=max && *xo>=min);
	    xo++,j++,i++) {
		if(*(xo-1)>=*xo) {
			fprintf(stderr,"unsorted data encountered in"
				" id %d: aborting\n", id);
			delete sd;
			return;
		}
	}

	if(j<=order) {
		fprintf(stderr,"not enough points in the"
			" interval %f<x<%f\n in id %d\n", min,max,id);
		delete sd;
		return;
	}
	xo-=j;
		
	if( (mode&RES_MODE)&&(nop!=-1) ) { 
		fprintf(stderr,"changing nop from %ld to -1\n",nop);
		nop=-1;
	}

	if (nop!=-1)
		newn=nop; // output at nop equidistance points
	                  // in [min,max]
	else 
		newn=m;   // output at x values of input
		
	for(i=0;i<j;i++)
		w[i]=1/sigma(xo[i]);
	if(iopt==-1) {
		iopt=0;
		curfit_(&iopt, &j, xo, yo, w, &min, &max, &order, 
			&sfac, &nest, &n, t, c, &fp, wrk, &lwrk,
			iwrk, &ier);
		iopt=-1;
	}
	curfit_(&iopt, &j, xo, yo, w, &min, &max, &order, 
		&sfac, &nest, &n, t, c, &fp, wrk, &lwrk,
		iwrk, &ier);

	if (mode&REP_MODE) {
		if (nop!=-1) {
			// resize old set
			sd->resize(id,newn);
			sd->set_n(id,newn);
		}
		x=sd->x(id);
		y=sd->y(id);
		if (mode&RES_MODE) {  // REP_MODE & RES_MODE
			yn=(doublereal *)xmalloc(sizeof(doublereal)*newn);
			splev_(t,&n,c,&order,x,yn,&newn,&ier1);
			for(i=0;i<newn;i++)
				y[i]-=yn[i];
			free(yn);
		} else {              // REP_MODE & ~RES_MODE
			if(nop!=-1) {
				for(i=0;i<nop;i++)
					x[i]=min+(double)i/(double)(newn-1)*
						(max-min);
			}
			splev_(t,&n,c,&order,x,y,&newn,&ier1);
			
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
			splev_(t,&n,c,&order,x,yn,&newn,&ier1);
			for(i=0;i<newn;i++) {
				xn[i]=x[i];
				yn[i]=y[i]-yn[i];
			}
		} else {              // ~REP_MODE & ~RES_MODE
			int k;
			sd->set_ident(newid,(const char **)sd->ident(id,&k));
			sd->add_ident(newid,"%s",
				      program_invocation_short_name);
			if(nop!=-1) {
				for(i=0;i<newn;i++)
					xn[i]=min+
						(double)i/(double)(newn-1)*
						(max-min);
				sd->set_n(newid,newn);
			} else {
				for (i=0;i<newn;i++)
					xn[i]=x[i];
			}
			splev_(t,&n,c,&order,xn,yn,&newn,&ier1);
		}
	}

	update_log(id, newid, mode, min, max, nop, sfac, 
		   iopt, fp, t, n, ier, order, sd);
	sd->changed();


 abort:
	delete sd;

	free(iwrk);
	free(wrk);
	free(c);
	free(t);
	free(w);
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

void inform_user(double min,		double max,	integer nop,
		 doublereal sfac,	integer iopt,	doublereal fp,
		 doublereal *t,		integer n,	integer ier,
		 integer order)
{
	printf("ier=%ld, fp=%g, #=%ld\n",ier,fp,n);
	printf("i=%f, a=%f, n=%ld, f=%f, k=%ld, iopt=%ld\n",
	       min,max,nop,sfac,order,iopt);
	print_sigma(stdout);
}

void update_log(int id, int newid, integer mode,
 		double min,		double max,	integer nop,
		doublereal sfac,	integer iopt,	doublereal fp,
 		doublereal *t,		integer n,	integer ier,
		integer order,		SharedData *sd)
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
        if(mode&RES_MODE)
		off+=asprintcat(&logline,&ll,off," -r");
		
	off+=asprintcat(&logline,&ll,off," -i %g -a %g",min,max);

	if(nop!=-1)
		off+=asprintcat(&logline,&ll,off," -n %ld",nop);

	off+=asprintcat(&logline,&ll,off,
			" -k %ld -f %g -o %ld",order,sfac,iopt);
	
	sprint_sigma(&logline,&ll,&off);

	off+=asprintcat(&logline,&ll,off," $%d",id);
	off+=asprintcat(&logline,&ll,off,"\n# fp=%g, ier=%ld, #=%ld",
				fp,ier,n);

	sd->log(logline);
	free(logline);
}
