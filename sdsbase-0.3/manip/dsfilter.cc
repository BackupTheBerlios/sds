#include <SharedData.h>
#include <getopt.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>


static struct option const long_options[] =
{
	{"help", no_argument, 0, 'h'},
	{"up"   , no_argument, 0, 'u'},
	{"down"   , no_argument, 0, 'o'},
	{"rate"   , required_argument, 0, 'r'},
	{"T_c"   , required_argument, 0, 't'},
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
	      "usage: %s -t T_c -r rate [-u|-o] id(s)\n"
		"\n"
		"-t,    --T_c        Time constant in seconds\n"
		"-r,    --rate       rate in x-unit per hour\n"
		"-u,    --up\n"
		"-d,    --down\n"
		"-h,-?, --help       this message\n"
		"\n"
		"performs 6dB low pass exponential filtering\n",
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

void filter(double *x, double *y, int n, double bconst, int up_down)
{
	double y0;
	int i;
  
	if(up_down)
		for(i=n-2,y0=y[n-1];i>=0;i--) {
			y0+=(y[i]-y0)*(1-exp(-(x[i+1]-x[i])/bconst));
			y[i]=y0;
		}
	else
		for(i=1,y0=y[0];i<n;i++){
			y0+=(y[i]-y0)*(1-exp(-(x[i]-x[i-1])/bconst));
			y[i]=y0;
		}
}

int main(int argc,char **argv)
{
	SharedData hallo;
	int i;
	int c;
	double t_c=0.0,rate=0.0;
	double b_c;
	int up_down=0,uoset=0;

	int id;
	int changed=0;
	
	while ((c = getopt_long(argc, argv, "t:r:udh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 't':
			t_c=strtod(optarg,NULL);
			break;
		case 'r':
			rate=strtod(optarg,NULL);
			break;
		case 'u':
			if(uoset)
				usage();
			up_down=0;
			uoset=1;
			break;
		case 'd':
			if(uoset)
				usage();
			up_down=1;
			uoset=1;
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

	if(rate<=0.0||t_c<=0.0)
		usage();
	
	b_c=t_c*rate/3600.0;

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
		
		filter(hallo.x(id), hallo.y(id), hallo.n(id),
			       b_c, up_down);
		
		off+=asprintcat(&logline,&ll,off," -t %f -r %f -%c $%d",
				t_c, rate, (up_down ? 'd':'u'),id);
		hallo.log(logline);
		free(logline);
		changed=1;
	}
	if(changed)
		hallo.changed();

	return 0;
}

