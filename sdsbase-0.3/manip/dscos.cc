#include <SharedData.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <float.h>

static struct option const long_options[] =
{
	{"help",        no_argument,       0, 'h'},
	{"new",         required_argument, 0, 'n'},
	{"min",         required_argument, 0, 'i'},
	{"max",         required_argument, 0, 'a'},
	{"temperature", required_argument, 0, 't'},
	{"biasvoltage", required_argument, 0, 'v'},
	{"ga",          required_argument, 0, 'l'},
	{"gb",          required_argument, 0, 'r'},
	{"ec",          required_argument, 0, 'e'},
	{"offset",      required_argument, 0, 'o'},
	{"scale",       required_argument, 0, 's'},
	{"factor",      required_argument, 0, 'f'},
	{"type",        required_argument, 0, 'x'},
	{"version",     no_argument,       0, 'V'},
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dscos.cc,v 1.1 2003/01/23 14:50:49 vstein Exp $";

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
		"calculates coulomb oscilations in different ways.\n"
		"options:\n"
		"-n num, --new num      use new dataset with num points\n"
		"-i min, --min min\n"
		"-a max, --max max\n"
		"-t T,   --temperature T\n"
		"-v v,   --biasvoltage v\n"
		"-l ga,  --ga ga\n"
		"-r gb,  --gb gb\n"
		"-e ec,  --ec ec\n"
		"-o xoff,--offset xoff\n"
		"-s xsca,--scale xsca\n"
		"-f yfac,--factor yfac\n"
		"-x type,--type type 0=exact, 1=fast, 2=single\n"
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

double gred(double e, double beta) {
// e/(1-exp(-beta*e))
	if (fabs(beta * e) < DBL_EPSILON)
		return 1.0 / beta;
	else if (beta * e > 37)
		return e;
	else if (beta * e < -709)
		return 0.0;
	return (-e / expm1(-beta * e));
}

double ginc(double e, double beta) {
// e/(exp(beta*e)-1)
	if (fabs(beta * e) < DBL_EPSILON)
		return 1.0 / beta;
	else if (beta * e < -37)
		return -e;
	else if (beta * e > 709)
		return 0.0;
	return (e / expm1(beta * e));
}

//#define deltav FLT_EPSILON
//#define deltac 1e6
//#define cT 1e0
//#define gb_ga 1000

double coulos(double xn, double a,  double b,  double c,
	      double v , double ec, double t, double ga, double gb)
{
// xn in units what ever you like
// b offset in the same units
// a period in the same units
// c scales the size ( c=1 corresponds to coulos=0.5 @ maximum)
// v bias voltage
// ec charging energy
// t temperature
// ga, gb conductances
// internals:
// ng is dimensionless gate charge
// ecom, ea, eb, ... energies in units of 2E_c
// gai, gar, ... rates in units of G_a/C_Sum
	double ng,ecom,ec2,ea,eb,eanu,eand,ebnu,ebnd,pu,pd,i,norm;
	double wn_np,wnp_n,wn_nm,wnm_n,gai,gar;
	double nu,nd,nmin,nmax,nstart;

	double be=1/t;

	ng=(xn-b)/a;
	ec2=2.0*ec;
	ecom=ec2*(0.5-ng);
	ea=ecom+v;
	eb=ecom-v;

	nstart=rint(-ecom/ec2);
	nmin=rint((-ecom-200.0*t)/ec2)-1;
	nmax=rint((-ecom+200.0*t)/ec2)+1;

	norm=pu=pd=1.0;
	eanu=nstart*ec2+ea;
	i=ginc(eanu,be)*ga-gred(eanu-ec2,be)*gb;

	for(nu=nstart+1,nd=nstart-1;nu<=nmax||nd>=nmin;nu++,nd--) {
		eanu=nu*ec2+ea;
		ebnu=nu*ec2+eb;
		gai=ginc(eanu,be)*ga;
		gar=gred(eanu-ec2,be)*ga;
		wn_nm=gar+gred(ebnu-ec2,be)*gb;
		wnm_n=ginc(eanu-ec2,be)*ga+ginc(ebnu-ec2,be)*gb;
		pu*=wnm_n/wn_nm;
		norm+=pu;
		i+=pu*(gai-gar);

		eand=nd*ec2+ea;
		ebnd=nd*ec2+eb;
		gai=ginc(eand,be)*ga;
		gar=gred(eand-ec2,be)*ga;
		wn_np=gai+ginc(ebnd,be)*gb;
		wnp_n=gred(eand,be)*ga+gred(ebnd,be)*gb;
		pd*=wnp_n/wn_np;
		norm+=pd;
		i+=pd*(gai-gar);
	}
		
	return -c*i/norm/v;
}

double coulfastsingle(double xn, double a,  double b,  double c,
		      double v, double ec, double t, double ga, double gb)
{

//itnicea:=ga*gb*(exp(2*v*bet)-1)*(et^2-v^2)/ 
// (ga*(et+v)*(exp((et+v)*bet)+1)*(exp(-(et-v)*bet)-1)-
//  gb*(et-v)*(exp((et+v)*bet)-1)*(exp(-(et-v)*bet)+1));


	double ng,et,i,be;

	ng=(xn-b)/a;
	et=2*ec*(0.5-ng);
	be=1/t;

	i=ga*gb*(exp(2.0*v*be)-1.0)*(et*et-v*v)/
	  (ga*(et+v)*(exp((et+v)*be)+1.0)*(exp(-(et-v)*be)-1.0)-
	   gb*(et-v)*(exp((et+v)*be)-1.0)*(exp(-(et-v)*be)+1.0));

	return -c*i/v;
}


double coulfast(double xn, double a,  double b,  double c,
		double v, double ec, double t, double ga, double gb)
{

//itnicea:=ga*gb*(exp(2*v*bet)-1)*(et^2-v^2)/ 
// (ga*(et+v)*(exp((et+v)*bet)+1)*(exp(-(et-v)*bet)-1)-
//  gb*(et-v)*(exp((et+v)*bet)-1)*(exp(-(et-v)*bet)+1));


	double ng,ec2,et,etcom,be,sum;
	double nmin,nmax,n;

	ng=(xn-b)/a;
	ec2=ec*2.0;
	etcom=ec2*(0.5-ng);
	be=1/t;

	nmin=rint((-200*t-etcom)/ec2);
	nmax=rint((200*t-etcom)/ec2);
	for(n=nmin,sum=0.0;n<=nmax;n++) {
		et=etcom+n*ec2;
		sum+=ga*gb*(exp(2.0*v*be)-1.0)*(et*et-v*v)/
	             (ga*(et+v)*(exp((et+v)*be)+1.0)*(exp(-(et-v)*be)-1.0)-
	              gb*(et-v)*(exp((et+v)*be)-1.0)*(exp(-(et-v)*be)+1.0));
	}

	return -c*sum/v;
}

int main(int argc,char **argv)
{
	SharedData hallo;
	int c,i;
	int n=0;
	double min=-1.0,max=1.0;
	double temperature=1.0/100.0;
	double voltage=1.0/10.0;
	double ga=1.0;
	double gb=1.0;
	double ec=1.0;
	double offset=0.0;
	double scale=1.0;
	double factor=1.0;
	
	int type=0;
	int id;
	double *x,*y;
	double dx,xn;
	int changed=0;

	while ((c = getopt_long(argc, argv, "n:i:a:t:v:l:r:e:o:s:f:x:h?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'n':
			if (sscanf(optarg,"%d",&n)!=1)
				usage();
			break;
		case 'i':
			if (sscanf(optarg,"%lf",&min)!=1)
				usage();
			break;
		case 'a':
			if (sscanf(optarg,"%lf",&max)!=1)
				usage();
			break;
		case 't':
			if (sscanf(optarg,"%lf",&temperature)!=1)
				usage();
			break;
		case 'v':
			if (sscanf(optarg,"%lf",&voltage)!=1)
				usage();
			break;
		case 'l':
			if (sscanf(optarg,"%lf",&ga)!=1)
				usage();
			break;
		case 'r':
			if (sscanf(optarg,"%lf",&gb)!=1)
				usage();
			break;
		case 'o':
			if (sscanf(optarg,"%lf",&offset)!=1)
				usage();
			break;
		case 's':
			if (sscanf(optarg,"%lf",&scale)!=1)
				usage();
			break;
		case 'f':
			if (sscanf(optarg,"%lf",&factor)!=1)
				usage();
			break;
		case 'x':
			if (sscanf(optarg,"%d",&type)!=1)
				usage();
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
//			y[i]=coulos(xn,scale,offset,factor,voltage,
//				    ec,temperature,ga,gb);
			switch(type){
			case 0:
				y[i]=coulos(xn,scale,offset,factor,voltage,
				      	    ec,temperature,ga,gb);
				break;
			case 1:
				y[i]=coulfast(xn,scale,offset,factor,voltage,
				      	      ec,temperature,ga,gb);
				break;
			case 2:
				y[i]=coulfastsingle(xn,scale,offset,factor,voltage,
				      	            ec,temperature,ga,gb);
				break;
			}
		}
		off=asprintcat(&logline,&ll,0,"<%d>\t%s -t %d -n %d -i %g -a %g"
			       " -t %g -v %g -l %g -r %g"
			       " -e %g -o %g -s %g -f %g",
			       id, program_invocation_short_name,
			       type, n, min, max, 
                               temperature, voltage, ga, gb, 
			       ec, offset, scale, factor);
//		if(derivative)
//			off+=asprintcat(&logline,&ll,off," -d");

		hallo.log(logline);
		hallo.set_n(id,n);
		hallo.add_ident(id,"dscos");
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
		off=asprintcat(&logline,&ll,0,"<%d>\t%s -t %d"
			       " -t %g -v %g -l %g -r %g"
			       " -e %g -o %g -s %g -f %g",
			       id, program_invocation_short_name,type,
                               temperature, voltage, ga, gb, 
			       ec, offset, scale, factor);

		n=hallo.n(id);
		x=hallo.x(id);
		y=hallo.y(id);
			
		for(i=0;i<n;i++) {
			xn=x[i];
			switch(type){
			case 0:
				y[i]=coulos(xn,scale,offset,factor,voltage,
				      	    ec,temperature,ga,gb);
				break;
			case 1:
				y[i]=coulfast(xn,scale,offset,factor,voltage,
				      	      ec,temperature,ga,gb);
				break;
			case 2:
				y[i]=coulfastsingle(xn,scale,offset,factor,voltage,
				      	            ec,temperature,ga,gb);
				break;
			}
		}
		hallo.log(logline);
		free(logline);
		changed=1;
	}
	if (changed)
		hallo.changed();
	return 0;
}

