#include <SharedData.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

static struct option const long_options[] =
{
	{"help",        no_argument,       0, 'h'},
	{"number",      required_argument, 0, 'n'},
	{"add",         required_argument, 0, 'a'},
	{"log2",        required_argument, 0, 'b'},
	{"save",        no_argument,       0, 's'},
	{"dump",        no_argument,       0, 'd'},
	{"version",     no_argument,       0, 'V'},
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dspad.cc,v 1.1 2003/01/23 14:50:49 vstein Exp $";

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
		"Pads with zeros.\n"
                "If the options -n, -a, -b are not used, the next matching power "
		"of 2 is used.\n"
		"The new x-axis values are calculated using x[0] and x[N-1], so the\n"
		"dataset should be linearly arranged.\n"
		"options:\n"
		"-n num, --number num   enlarge dataset to num points\n"
		"-a num, --add num      add num points to dataset\n"
		"-b exp, --log2 exp     enlarge dataset to 2^exp points\n"
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


#define SET_N 0x0001
#define SET_A 0x0002
#define SET_B 0x0004

int main(int argc,char **argv)
{
	SharedData hallo;
	int c,i,no,k;
	int n=0,a=0,b=0;
	int nmax=INT_MAX/2+1;
	unsigned char params=0;
	int id;
	double *x,*y;
	double dx,xn;
	int changed=0;

	while ((c = getopt_long(argc, argv, "n:a:b:h?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'n':
			if  (params) 
				use_params();
			if (sscanf(optarg,"%d",&n)!=1)
				usage();
			params=SET_N;
			break;
		case 'a':
			if  (params) 
				use_params();
			if (sscanf(optarg,"%d",&a)!=1)
				usage();
			params=SET_A;
			break;
		case 'b':
			if  (params) 
				use_params();
			if (sscanf(optarg,"%d",&b)!=1)
				usage();
			params=SET_B;
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

		no=hallo.n(id);
		
		if (params&SET_N) {
			if (n<no) {
				fprintf(stderr,"nop on %d (%s)\n",id,argv[i]);
				continue;
			}
			a=n-no;
			off+=asprintcat(&logline,&ll,off," -n %d",n);
		}
		if (params&SET_A) {
			if (a<1) {
				fprintf(stderr,"nop on %d (%s)\n",id,argv[i]);
				continue;
			}
			n=no+a;
			off+=asprintcat(&logline,&ll,off," -a %d",a);
		}
		if (params&SET_B) {
			if (b<1) {
				fprintf(stderr,"nop on %d (%s)\n",id,argv[i]);
				continue;
			}
			n=1<<b;
			if (n<=no) {
				fprintf(stderr,"nop on %d (%s)\n",id,argv[i]);
				continue;
			}
			off+=asprintcat(&logline,&ll,off," -b %d",b);
		}
		if (!params) {
			for (n=1,b=0;n<no&&n<nmax;b++) n<<=1;
			if (n<=no) {
				fprintf(stderr,"nop on %d (%s)\n",id,argv[i]);
				continue;
			}
			off+=asprintcat(&logline,&ll,off," -b %d",b);
		}
		
		hallo.resize(id,n);
		x=hallo.x(id);
		y=hallo.y(id);
			
		dx=(x[no-1]-x[0]);
		if (no>1) dx/=(no-1);
		xn=x[no-1]+dx;

		for (k=no;k<n;k++,xn+=dx) {
			x[k]=xn;
			y[k]=0.0;
		}
		hallo.set_n(id,n);
		off+=asprintcat(&logline,&ll,off," $%d",id);
		hallo.log(logline);
		free(logline);
		changed=1;
	}
	if (changed)
		hallo.changed();
	return 0;
}

