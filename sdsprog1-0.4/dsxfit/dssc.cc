#include <stdio.h>
#include <stdlib.h>
#include <SharedData.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <g2c.h>
#include "dssc.h"
#include <Normal.h>
#include <ACG.h>
#include <errno.h>

static struct option const long_options[] =
{
	{"help"    , no_argument,       0, 'h'},
	{"x"       , required_argument, 0, 'x'},
	{"y"       , required_argument, 0, 'y'},
	{"new"     , required_argument, 0, 'n'},
	{"min"     , required_argument, 0, 'i'},
	{"max"     , required_argument, 0, 'a'},
	{"version" , no_argument,       0, 'v'},
	{"warranty", no_argument,       0, 'w'},
	{0, 0, 0, 0}
};

char verstring[] = "0.04";

extern char *program_invocation_short_name;

Normal *rand1;

extern "C" {
extern doublereal dpsi_(doublereal *x); 
extern int dpsifn_(doublereal *x, integer *n, integer *kode, integer *m,
		   doublereal *ans, integer *nz, integer *ierr); 
}

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
	      "usage: %s [options] regex(s)\n"
		"\n"
		"options:\n"
		"-x,                 mathexp\n"
		"-y,                 mathexp\n"
		"-n,    --new        number of points\n"
		"-o,    --output     basename of output and dsh file\n"
		"-s,    --save\n"
		"-d,    --dump\n"
		"-q,    --quiet      no protocol in dsh file, version unset\n"
		"-i,    --min        minimum\n"
		"-a,    --max        maximum\n"
		"-h,-?, --help       this message\n"
		"-v,    --version    display version information and exit.\n"
		"-w,    --warranty   display licensing terms.\n"
		"\n"
		"-[s|d] is to be specified exclusively\n" 
		"-o is to be used in conjunction with -n only\n",
		program_invocation_short_name);
	exit(0);
}

void version(void)
{
	fprintf(stderr, "%s version %s\n", program_invocation_short_name,
		verstring);

	exit(0);
}

extern "C" {
	int dssc_yyparse(void);
	struct yy_buffer_state *dssc_yy_scan_string(const char *);
	extern FILE *dssc_yyin;
	extern char bef[];
	extern double cons[];
	extern int bi,ci;
 	extern int dssc_yydebug;
}

double fx(double *xa,double *ya, int max,
	  char *bef, int bi, double *cons,int,
	  int ind,double limits[4])
{
	int i,j,sti;
	double st[500];
	doublereal temp;
	integer eins=1, n, nz, ierr;

	for(i=0,j=0,sti=0;i<bi;i++) {
		switch(bef[i]) {
		case Y_PUT:
			st[sti++]=cons[j++];
			break;
		case Y_PLUS:
			st[sti-2]+=st[sti-1];
			sti--;
			break;
		case Y_MINUS:
			st[sti-2]-=st[sti-1];
			sti--;
			break;
		case Y_MULT:
			st[sti-2]*=st[sti-1];
			sti--;
			break;
		case Y_DIV:
			st[sti-2]/=st[sti-1];
			sti--;
			break;
		case Y_NEG:
			st[sti-1]=-st[sti-1];
			break;
		case Y_PUTIND:
			st[sti++]=ind;
			break;
		case Y_PUTMAXIND:
			st[sti++]=max;
			break;
		case Y_XARR:
			if(st[sti-1]<0)
				st[sti-1]=xa[0];
			else if(st[sti-1]>=max)
				st[sti-1]=xa[max-1];
			else 
				st[sti-1]=xa[(int)st[sti-1]];
			break;
		case Y_YARR:
			if(st[sti-1]<0)
				st[sti-1]=ya[0];
			else if(st[sti-1]>=max)
				st[sti-1]=ya[max-1];
			else 
				st[sti-1]=ya[(int)st[sti-1]];
			break;
		case Y_X:
			st[sti++]=xa[ind];
			break;
		case Y_Y:
			st[sti++]=ya[ind];
			break;
		case Y_PARAM:
			st[sti-1]=xp[(int)st[sti-1]];
			break;
                case Y_SIN:
			st[sti-1]=sin(st[sti-1]);
			break;
		case Y_COS:
			st[sti-1]=cos(st[sti-1]);
			break;
		case Y_TAN:
			st[sti-1]=tan(st[sti-1]);
			break;
		case Y_ASIN:
			st[sti-1]=asin(st[sti-1]);
			break;
		case Y_ACOS:
			st[sti-1]=acos(st[sti-1]);
			break;
		case Y_ATAN:
			st[sti-1]=atan(st[sti-1]);
			break;
		case Y_ATAN2:
			st[sti-2]=atan2(st[sti-2],st[sti-1]);
			sti--;
			break;
		case Y_EXP:
			st[sti-1]=exp(st[sti-1]);
			break;
		case Y_LOG:
			st[sti-1]=log(st[sti-1]);
			break;
		case Y_LOG10:
			st[sti-1]=log10(st[sti-1]);
			break;
		case Y_POW:
			st[sti-2]=pow(st[sti-2],st[sti-1]);
			sti--;
			break;
		case Y_SQRT:
			st[sti-1]=sqrt(st[sti-1]);
			break;
		case Y_CBRT:
			st[sti-1]=cbrt(st[sti-1]);
			break;
		case Y_HYPOT:
			st[sti-2]=hypot(st[sti-2],st[sti-1]);
			sti--;
			break;
		case Y_EXPM1:
			st[sti-1]=expm1(st[sti-1]);
			break;
		case Y_LOG1P:
			st[sti-1]=log1p(st[sti-1]);
			break;
		case Y_SINH:
			st[sti-1]=sinh(st[sti-1]);
			break;
		case Y_COSH:
			st[sti-1]=cosh(st[sti-1]);
			break;
		case Y_TANH:
			st[sti-1]=tanh(st[sti-1]);
			break;
		case Y_ASINH:
			st[sti-1]=asinh(st[sti-1]);
			break;
		case Y_ACOSH:
			st[sti-1]=acosh(st[sti-1]);
			break;
		case Y_ATANH:
			st[sti-1]=atanh(st[sti-1]);
			break;
		case Y_FABS:
			st[sti-1]=fabs(st[sti-1]);
			break;
		case Y_PSI:
			temp=st[sti-1];
			st[sti-1]=dpsi_(&temp);
			break;
		case Y_PSIFN:
			temp=st[sti-2];
			n=(integer)st[sti-1];
			dpsifn_(&temp,&n,&eins,&eins,&st[sti-2],&nz,&ierr);
			sti--;
			break;
		case Y_RAND:
			rand1->variance(st[sti-1]);
			st[sti-1]=(*rand1)();
			break;
		case Y_THETA:
			st[sti-1]=((st[sti-1]>=0) ? 1.0 : 0.0);
			break;
		case Y_MAX:
			st[sti-2]=(st[sti-1]>st[sti-2]? st[sti-1] : st[sti-2]);
			sti--;
			break;
		case Y_MIN:
			st[sti-2]=(st[sti-1]<st[sti-2]? st[sti-1] : st[sti-2]);
			sti--;
			break;
		case Y_XMIN:
			st[sti++]=limits[0];
			break;
		case Y_XMAX:
			st[sti++]=limits[1];
			break;
		case Y_YMIN:
			st[sti++]=limits[2];
			break;
		case Y_YMAX:
			st[sti++]=limits[3];
			break;
		}
	}
	return st[0];
}

void set_limits(double limits[4], double *xa, double *ya, int n)
{
	int i;

	if(!n) {
		limits[0]=limits[1]=0;
		limits[2]=limits[3]=0;
		return;
	}
		
	limits[0]=limits[1]=xa[0];
	limits[2]=limits[3]=ya[0];

	for(i=1;i<n;i++) {
		limits[0]=((limits[0]<xa[i])? limits[0] : xa[i]);
		limits[1]=((limits[1]>xa[i])? limits[1] : xa[i]);
		limits[2]=((limits[2]<ya[i])? limits[2] : ya[i]);
		limits[3]=((limits[3]>ya[i])? limits[3] : ya[i]);
	}
}

int main(int argc,char **argv)
{
	SharedData hallo;
	double limits[4];

	int i,max=0;

	char *xbf,*ybf;
	char *xbz,*ybz;
	char *cmi=NULL,*cma=NULL;
	double *xcons,*ycons;
	int xbi=0,ybi=0;
	int xci,yci;
	int c;
	double *x,*xa,*y,*ya;
	double xmin=NAN;
	double xmax=NAN;
	double delta;

	int changed=0;
	int id;

// 	dssc_yydebug=1;

	char *cseed=getenv(DSSC_SEED);
	int seed;
	if (!cseed) {
		FILE *filp;
		filp=fopen("/dev/urandom","r");
		if (filp)
			fscanf(filp,"%4c",(char *) &seed);
		else
			seed=4711;
	}
	else {
		if (sscanf(cseed,"%d",&seed)!=1)
			seed=4711;
	}
	ACG gen(seed);
	rand1 = new Normal(0,1,&gen);

	while ((c = getopt_long(argc, argv, "x:y:n:i:a:h?vw",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'x':
			bi=ci=0;
			dssc_yy_scan_string(xbz=optarg);
			if(dssc_yyparse())
				exit(1);
			xbf=(char*)xmalloc(sizeof(char)*(bi+1));
			memcpy(xbf,bef,bi*sizeof(char));
			xbf[bi]=0;
			xbi=bi;
			xcons=(double*)xmalloc(sizeof(double)*(ci+1));
			memcpy(xcons,cons,ci*sizeof(double));
			xci=ci;
			break;
		case 'y':
			bi=ci=0;
			dssc_yy_scan_string(ybz=optarg);
			if(dssc_yyparse())
				exit(1);
			ybf=(char*)xmalloc(sizeof(char)*(bi+1));
			memcpy(ybf,bef,bi*sizeof(char));
			ybf[bi]=0;
			ybi=bi;
			ycons=(double*)xmalloc(sizeof(double)*(ci+1));
			memcpy(ycons,cons,ci*sizeof(double));
			yci=ci;
			break;
		case 'n':
			if(sscanf(optarg,"%d",&max)!=1) {
				fprintf(stderr,"error in option --new:"
					" \"%s\" should be an integer\n",
					optarg);
				usage();
			}
			break;
		case 'i':
			if(sscanf(optarg,"%lf",&xmin)!=1) {
				fprintf(stderr,"error in option --minimum:"
					" \"%s\" should be a double\n",
					optarg);
				usage();
			}
			cmi=optarg;
			break;
		case 'a':
			if(sscanf(optarg,"%lf",&xmax)!=1) {
				fprintf(stderr,"error in option --maximum:"
					" \"%s\" should be a double\n",
					optarg);
				usage();
			}
			cma=optarg;
			break;
		case 'w':
			warranty();
			break;
		case 'v':
			version();
			break;
		case 'h':
		case '?':
		default:
			usage();
			break;
		}
	}

	if(max) {
		char *logline=NULL;
		int ll=0,off=0;
		if(isnan(xmax)&&isnan(xmin)) {
			xmin=0.0;
			delta=1.0;
		}
		else if(isnan(xmax))
			delta=1.0;
		else if(isnan(xmin)) {
			xmax=fabs(xmax);
			xmin=-fabs(xmax);
			delta=2*fabs(xmax)/((double)(max-1));
		}
		else {
			if(xmin>xmax) {
				double tmp=xmax;
				xmax=xmin;
				xmin=tmp;
			}
			delta=(xmax-xmin)/((double)(max-1));
		}

		id=hallo.create(max);
		off=asprintcat(&logline,&ll,0,"<*%d*>\t%s",id,
			       program_invocation_short_name);

		x=hallo.x(id);
		y=hallo.y(id);
		for(i=0;i<max;i++)
			y[i]=x[i]=xmin+i*delta;
		xa=(double *)xmalloc(sizeof(double)*max);
		ya=(double *)xmalloc(sizeof(double)*max);
		memcpy(xa,x,sizeof(double)*max);
		memcpy(ya,y,sizeof(double)*max);
		limits[0]=limits[2]=xmin;
		limits[1]=limits[3]=xmax;
		hallo.add_ident(id,"dssc");
		if(xbi) {
			for(i=0;i<max;i++) {
				x[i]=fx(xa,ya,max,xbf,xbi,xcons,xci,i,limits);
			}
			off+=asprintcat(&logline,&ll,off," -x %s",xbz);
			hallo.add_ident(id,"-x %s",xbz);
		}
		if(ybi) {
			for(i=0;i<max;i++) {
				y[i]=fx(xa,ya,max,ybf,ybi,ycons,yci,i,limits);
			}
			off+=asprintcat(&logline,&ll,off," -y %s",ybz);
			hallo.add_ident(id,"-y %s",ybz);
		}
		free(xa);
		free(ya);
		hallo.set_n(id,max);
		off+=asprintcat(&logline,&ll,off," -n %d",max);
		if(cmi)
			off+=asprintcat(&logline,&ll,off," -i %s",cmi);
		if(cma)
			off+=asprintcat(&logline,&ll,off," -a %s",cma);
		hallo.log(logline);
		hallo.changed();
		printf("new id=%d\n",id);
		return 0;
	}

	for(i=optind;i<argc;i++) {
		char *logline=NULL;
		int ll=0,off=0;
		int j;

		errno=0;
		id=strtol(argv[i],NULL,0);
		if (errno) {
			fprintf (stderr,"ID Overflow in %s\n",argv[i]);
			continue;
		}
		if(!hallo.hasid(id)) {
			fprintf(stderr,"No id %d (%s)\n",id,argv[i]);
			continue;
		}
		off=asprintcat(&logline,&ll,0,"<%d>\t%s",id,
			       program_invocation_short_name);

		x=hallo.x(id);
		y=hallo.y(id);;
		max=hallo.n(id);
		xa=(double *)xmalloc(sizeof(double)*max);
		ya=(double *)xmalloc(sizeof(double)*max);
		memcpy(xa,x,sizeof(double)*max);
		memcpy(ya,y,sizeof(double)*max);
		set_limits(limits,xa,ya,max);
		if(xbi) {
			for(j=0;j<max;j++) {
				x[j]=fx(xa,ya,max,xbf,xbi,xcons,
					xci,j,limits);
			}
			if(blanks_in(xbz))
				off+=asprintcat(&logline,&ll,off,
						" -x \"%s\"", xbz);
			else
				off+=asprintcat(&logline,&ll,off,
						" -x %s", xbz);
		}
		if(ybi) {
			for(j=0;j<max;j++) {
				y[j]=fx(xa,ya,max,ybf,ybi,ycons,
					yci,j,limits);
			}
			if(blanks_in(ybz))
				off+=asprintcat(&logline,&ll,off,
						" -y \"%s\"", ybz);
			else
				off+=asprintcat(&logline,&ll,off,
						" -y %s", ybz);
		}
		free(xa);
		free(ya);
		if(xbi||ybi) {
			off+=asprintcat(&logline,&ll,off," $%d", id);
			hallo.log(logline);
			changed=1;
			free(logline);
		}
	}
	if(changed)
		hallo.changed();
	return 0;
}
