#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <SharedData.h>

#include <getopt.h>

static struct option const long_options[] = {
	{"help",        no_argument,       0, 'h'},
	{"version",     no_argument,       0, 'V'},
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "0.2";

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
	  "usage: %s [options] [regexp(s)]\n"
	  "\n"
	  "    the options are ...\n"
	  "-h,-?, --help      this message\n"
	  "-V,    --Version   display version information and exit.\n"
	  "-W,    --warranty  display licensing terms.\n"
	  "-C,    --copyright\n",
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

#define MAX(a,b) ((a) > (b) ? (a) : (b))


int main(int argc, char *argv[])
{
	SharedData sd;
	int id;
	int i,j,c;
	int idn=0;

	while ((c = getopt_long(argc, argv, "h?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
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

	idn=argc-optind;

	if(!idn)
	  return 0;

	printf("Minima: x             y       Maxima: x             y\n");

	for(i=optind;i<argc;i++) {
	  errno=0;
	  id=strtol(argv[i],NULL,0);
	  if (errno) {
	    fprintf (stderr,"ID Overflow in %s\n",argv[i]);
	    continue;
	  }
	  if(!sd.hasid(id)) {
	    fprintf(stderr,"No id %d (%s)\n",id,argv[i]);
	    continue;
	  }
	  int n=sd.n(id);
	  const double *x=sd.x(id);
	  const double *y=sd.y(id);
	  int slope, slope_old, minrest=0;
	  double min_x=NAN, min_y=NAN;
	    
	  if(idn>1)
	    printf("Id=%d:\n",id);

	  if(n>1)
	    slope_old=(y[1] < y[0] ? -1 : 1);
	  
	  for(j=2;j<n;j++) {
	    slope=(y[j] < y[j-1] ? -1 : 1);
	    if (slope != slope_old)
	      {
		if (slope == 1)
		  {
		    min_x=x[j-1];
		    min_y=y[j-1];
		    minrest=1;
		  }
		else
		  {
		    if (minrest)
		      printf("%14e %14e %14e %14e\n",
			     min_x,min_y,x[j-1],y[j-1]);
		    else
		      printf("      --             --       %14e %14e\n",
			     x[j-1],y[j-1]);
		    minrest=0;
		  }
		slope_old=slope;
	      }
	  }
	  if (minrest)
	    printf("%14e %14e       --             --\n",
		   min_x,min_y);
	}
	return 0;
}

