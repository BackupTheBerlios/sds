#include <sds/shareddata.h>
#include <getopt.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

static struct option const long_options[] =
{
	{"min",   required_argument, 0, 'i'},
	{"max",   required_argument, 0, 'a'},
	{"N"  ,   required_argument, 0, 'n'},
	{"save",        no_argument, 0, 's'},
	{"dump",        no_argument, 0, 'd'},
	{"help",        no_argument, 0, 'h'},
	{"version",     no_argument, 0, 'V'},
	{"warranty",    no_argument, 0, 'W'},
	{"copyright",   no_argument, 0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "0.04";

char *program;

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
	      "usage: %s -i min -a max -n #-of-points regex(s)\n"
		"\n"
		"-i,    --min       minimal dx to start with\n"
		"-a,    --max       maximal dx to end with\n"
		"-n,    --N         number of dx values to use\n"
		"-s,    --save\n"
		"-d,    --dump\n"
		"-h,-?, --help       this message\n"
		"\n"
		"devides the x-range in dx wide pices and sums up\n"
		"the maximal changes in y values within those pices.\n",
		program);
	exit(0);
}

void copyright(void)
{
	printf("%s version %s, Copyright (C) 1999 Roland Schaefer"
	       "%s comes with ABSOLUTELY NO WARRANTY;\n"
	       "This is free software, and you are welcome to"
	       " redistribute it\n"
	       "under certain conditions; type `%s -w' for details.\n",
	       program,verstring,program,program);
	exit(0);
}

void version(void)
{
	fprintf(stderr, "%s version %s\n", program,
		verstring);
	
	exit(0);
}

double fraktal(double *x, double *y, int n, double dx)
{
	int ia,ip,i;
	double xa,ya,xap,yap;
	double ymin,ymax;
	double ret=0;
	
	if(n<1)
		return 0;
	
	xa=x[0];
	ya=y[0];
	ia=0;
	xap=xa+dx;
	ip=1;

	while(ia<n) {
		while(ip<n&&x[ip]<xap)
			ip++;
		if(ip<n)
			yap=y[ip-1]+
				(y[ip]-y[ip-1])*(xap-x[ip-1])/(x[ip]-x[ip-1]);
		else
			yap=y[n-1];
		
		ymin=(ya<yap? ya: yap);
		ymax=(ya>yap? ya: yap);

		for(i=ia;i<ip;i++) {
			ymin=(ymin<y[i]? ymin: y[i]);
			ymax=(ymax>y[i]? ymax: y[i]);
		}

		ret+=ymax-ymin;

		xa=xap;
		ya=yap;
		ia=ip;
		xap=xa+dx;

	}

	return ret/dx;
}

int main(int argc,char **argv)
{
	SharedDataSeg seg;
	char **lbn=NULL;
	int nlbn=0;
	SharedDataSet *set;
	int i,j,k;
	int c;
	double min=1.0,max=100.0;
	int np=100;
	int dump=0;
	int save=0;

	double *x,*y,*xa,*ya;
	double ra,dr;
	int n;

	program = basen(argv[0]);
	
	while ((c = getopt_long(argc, argv, "i:a:n:sdh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'i':
			min=strtod(optarg,NULL);
			break;
		case 'a':
			max=strtod(optarg,NULL);
			break;
		case 'n':
			np=(int)strtol(optarg,NULL,0);
			break;
		case 's':
			if(dump)
				usage();
			save=1;
			break;
		case 'd':
			if(save)
				usage();
			dump=1;
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


	if(np<2)
		usage();

	dr=(max-min)/(np-1);

	xa=(double*)xmalloc(np*sizeof(double));
	ya=(double*)xmalloc(np*sizeof(double));
	
	for(i=optind;i<argc;i++) {
		seg.listbyname(&lbn,&nlbn,argv[i]);
		for(j=0;lbn[j];j++) {
			set=seg.item(lbn[j]);
			n=set->n();
			x=set->x();
			y=set->y();
			for(ra=min,k=0;k<np;ra+=dr,k++){
				xa[k]=ra;
				ya[k]=(double)fraktal(x,y,n,ra);
			}
			set=seg.resize(set,np);
			x=set->x();
			y=set->y();
			memcpy(x,xa,np*sizeof(double));
			memcpy(y,ya,np*sizeof(double));
			set->set_n(np);
			set->add_line_to_dsh("%s -i %f -a %f -n %d $@;",
					     program,min,max,np);
		}
		seg.setChanged(1);
	}

	free(xa);
	free(ya);

	return 0;
}

