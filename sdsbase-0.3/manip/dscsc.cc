#include <SharedData.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

static struct option const long_options[] =
{
	{"help",        no_argument,       0, 'h'},
	{"new",         required_argument, 0, 'n'},
	{"min",         required_argument, 0, 'i'},
	{"max",         required_argument, 0, 'a'},
	{"betaec",      required_argument, 0, 'b'},
	{"derivative",  no_argument,       0, 'd'},
	{"version",     no_argument,       0, 'V'},
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dscsc.cc,v 1.1 2003/01/23 14:50:49 vstein Exp $";

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


void use_params(void)
{
	fprintf(stderr,	" use only -n OR -a OR -b !\n");
	exit(0);
}

void usage(void)
{
	fprintf(stderr,
	      "usage: %s [options] id(s)\n"
		"calculates coulomb staircase.\n"
		"options:\n"
		"-n num, --new num      use new dataset with num points\n"
		"-i min, --min min\n"
		"-i max, --max max\n"
		"-b be,  --betaec be\n"
		"-d,     --derivative\n"
		"-h,-?,  --help         this message\n"
		"-V,     --version      display version information and exit\n"
		"-W,     --warranty     display licensing terms.\n"
		"-C,     --copyright  \n\n",
		program_invocation_short_name);
	exit(0);
}

void copyright(void)
{
	printf("%s version %s, Copyright (C) 1999 Roland Schaefer"
	       "%s comes with ABSOLUTELY NO WARRANTY;\n"
	       "This is free software, and you are welcome to"
	       " redistribute it\n"
	       "under certain conditions; type `%s -w' for details.\n",
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

//double csc(double xn, double be)
double csc(double xn, double a,  double b,  double c, double be)
{
	double arg,narg,earg,carg,n,nmin,nmax;
	double N=0.0,Z=0.0,Nx=0.0,Zx=0.0;

	arg=a*(xn-b);
	nmin=rint(arg-100/be);
	nmax=rint(arg+100/be);
	
	for(n=nmin;n<nmax;n++)
	{
		narg=(n-arg);
		earg=exp(-be*narg*narg);
		carg=2*a*be*narg*earg;

		N+=n*earg;
		Nx+=n*carg;
		Z+=earg;
		Zx+=carg;
	}
	return c*N/Z;
}

double dcsc(double xn, double a,  double b,  double c, double be)
{
	double arg,narg,earg,carg,n,nmin,nmax;
	double N=0.0,Z=0.0,Nx=0.0,Zx=0.0;

	arg=a*(xn-b);
	nmin=rint(arg-100/be);
	nmax=rint(arg+100/be);
	
	for(n=nmin;n<nmax;n++)
	{
		narg=(n-arg);
		earg=exp(-be*narg*narg);
		carg=2*a*be*narg*earg;

		N+=n*earg;
		Nx+=n*carg;
		Z+=earg;
		Zx+=carg;
	}
	return c*(Z*Nx-N*Zx)/(Z*Z);
}

int main(int argc,char **argv)
{
	SharedData hallo;
	int c,i;
	int n=0;
	double min=-1.0,max=1.0,be=1.0;
	
	int id;
	double *x,*y;
	double dx,xn;
	int changed=0;
	int derivative=0;

	while ((c = getopt_long(argc, argv, "n:a:i:b:dh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'n':
			if (sscanf(optarg,"%d",&n)!=1)
				usage();
			break;
		case 'a':
			if (sscanf(optarg,"%lf",&max)!=1)
				usage();
			break;
		case 'i':
			if (sscanf(optarg,"%lf",&min)!=1)
				usage();
			break;
		case 'b':
			if (sscanf(optarg,"%lf",&be)!=1)
				usage();
			break;
		case 'd':
			derivative=1;
			break;
		case 'W':
			warranty();
			break;
		case 'V':
			version();
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

	if(n) {
		char *logline=NULL;
		int ll=0,off=0;
		errno=0;
		id=hallo.create(n);
		x=hallo.x(id);
		y=hallo.y(id);
		
		dx=(max-min)/(double)(n-1);
		for(i=0,xn=min;i<n;i++,xn+=dx) {
			x[i]=xn;
			if(derivative)
				y[i]=dcsc(xn,1.0,0.0,1.0,be);
			else
				y[i]=csc(xn,1.0,0.0,1.0,be);
		}
		off=asprintcat(&logline,&ll,0,"<%d>\t%s -n %d -i %f -a %f"
			       " -b %f",id, program_invocation_short_name,
			       n,min,max,be);
		if(derivative)
			off+=asprintcat(&logline,&ll,off," -d");

		hallo.log(logline);
		hallo.set_n(id,n);
		hallo.add_ident(id,"dscsc -b %f",be);
		free(logline);
		hallo.changed();
		printf("new id=%d\n",id);
		return 0;
	}

	for(i=optind;i<argc;i++) {
		char *logline=NULL;
		int ll=0,off=0;
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

		n=hallo.n(id);
		x=hallo.x(id);
		y=hallo.y(id);
			
		dx=(max-min)/(double)(n-1);
		for(i=0,xn=min;i<n;i++,xn+=dx) {
			x[i]=xn;
			if(derivative)
				y[i]=dcsc(xn,1.0,0.0,1.0,be);
			else
				y[i]=csc(xn,1.0,0.0,1.0,be);
		}
		off=asprintcat(&logline,&ll,0," -n %d -i %f -a %f"
			       " -b %f", n,min,max,be);
		if(derivative)
			off+=asprintcat(&logline,&ll,off," -d");
		
		hallo.log(logline);
		free(logline);
		changed=1;
	}
	if (changed)
		hallo.changed();
	return 0;
}

