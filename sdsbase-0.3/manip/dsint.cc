#include <SharedData.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>

static struct option const long_options[] =
{
	{"help", no_argument, 0, 'h'},
	{"min"   , required_argument, 0, 'i'},
	{"max"   , required_argument, 0, 'a'},
	{"version",     no_argument,       0, 'V'},
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "0.04";

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
	      "usage: %s [-i minx] [-a maxx] ids\n"
		"\n"
		"-i,    --min\n"
		"-a,    --max\n"
		"-h,-?, --help       this message\n"
		"\t-V,    --version    display version information and exit.\n"
		"\t-W,    --warranty   display licensing terms.\n"
		"\t-C,    --copyright\n"
		"\n"
		"simple integration\n",
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

void integ(SharedData *sd, int id,
	   double min, int smin, double max, int smax,int *nop)
{
	int imin=0, imax=sd->n(id)-1;
	double *x=sd->x(id);
	double *y=sd->y(id);
	int i, j;
	double sum,tmp;

	*nop=0;

	if (smin) 
		while (x[imin]<min && imin<imax) imin++ ; 
	if (smax)
		while (x[imax]>max && imax>imin) imax-- ;

	if (imin==imax) {
		*nop=1;
		return;
	}

	for (i=imin,j=0,sum=0,tmp=0;i<imax;i++,j++) {
		sum+=(y[i]+y[i+1])/2*(x[i+1]-x[i]);
		x[j]=x[i];
		y[j]=tmp;
		tmp=sum;
	}
	x[j]=x[i];
	y[j++]=tmp;
	sd->set_n(id,j);
}

int main(int argc,char **argv)
{
	SharedData hallo;
	int i,c;
	double min=0.0,max=0.0;
	int smin=0,smax=0;
	
	int id,nop;
	int changed=0;
	
	while ((c = getopt_long(argc, argv, "i:a:h?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'i':
			min=strtod(optarg,NULL);
			smin=1;
			break;
		case 'a':
			max=strtod(optarg,NULL);
			smax=1;
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

		integ(&hallo, id, min, smin, max, smax,&nop);
		if(nop) {
			fprintf(stderr,"Nop on id %d\n",id);
		} else {
			if (smin)
				off+=asprintcat(&logline,&ll,off," -i %f",min);
			if (smax)
				off+=asprintcat(&logline,&ll,off," -a %f",max);
			off+=asprintcat(&logline,&ll,off," $%d",id);
			hallo.log(logline);
			changed=1;
		}
		free(logline);
	}
	if(changed)
		hallo.changed();

	return 0;
}

