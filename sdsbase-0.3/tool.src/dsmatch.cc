#include <SharedData.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <float.h>

static struct option const long_options[] = {
	{"help",      no_argument,       0, 'h'},
	{"version",   no_argument,       0, 'V'},
	{"warranty",  no_argument,       0, 'W'},
	{"copyright", no_argument,       0, 'C'},
	{"verbose",   no_argument,       0, 'v'},
	{"X",         required_argument, 0, 'x'},
	{"Y",         required_argument, 0, 'y'},
	{"min",       required_argument, 0, 'i'},
	{"max",       required_argument, 0, 'a'},
	{"Min",       required_argument, 0, 'I'},
	{"Max",       required_argument, 0, 'A'},
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
	      "usage: %s [-x|-y Num] ids\n"
		"\n"
		"\t-x,    --X          find y-value matching this x\n"
		"\t-y,    --Y          find x-value matching this y\n"
		"\t-i,    --min        \n"
		"\t-a,    --max        \n"
		"\t-I,    --Min        \n"
		"\t-A,    --Max        \n"
		"\t-v,    --verbose    \n"
		"\t-h,-?, --help       this message\n"
		"\t-V,    --version    display version information and exit.\n"
		"\t-W,    --warranty   display licensing terms.\n"
		"\t-C,    --copyright\n\n"
		"linear interpolation is used between the two data points \n "
                "whose x- (or y-) value are closest to Num. This may lead \n"
		"to strange results for non-monotonous data..\n"
		"the range is limited to min<Num<max and Min<result<Max \n",
		program_invocation_short_name);
	exit(0);
}

void copyright(void)
{
	printf("%s version %s, Copyright (C) 2000 Roland Schaefer"
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

int main(int argc,char **argv)
{
	SharedData sd;
	int c,i,j,n;
	int imin,imax,opt,matchx,verb;
	int minlopt,maxlopt,minsopt,maxsopt;
	double match,min,max,result;
	double minl,maxl,mins,maxs;
	double *lookup, *show;

	opt=0;
	minlopt=maxlopt=minsopt=maxsopt=0;
	verb=0;

	while ((c = getopt_long(argc, argv, "x:y:i:a:I:A:vh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
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
		case 'x':
			if (opt)
				usage();
			if(sscanf(optarg,"%lf",&match)!=1)
				usage();
			matchx=1;
			opt=1;
			break;
		case 'y':
			if (opt)
				usage();
			if(sscanf(optarg,"%lf",&match)!=1)
				usage();
			matchx=0;
			opt=1;
			break;
		case 'i':
			if(sscanf(optarg,"%lf",&minl)!=1)
				usage();
			minlopt=1;
			break;
		case 'a':
			if(sscanf(optarg,"%lf",&maxl)!=1)
				usage();
			maxlopt=1;
			break;
		case 'I':
			if(sscanf(optarg,"%lf",&mins)!=1)
				usage();
			minsopt=1;
			break;
		case 'A':
			if(sscanf(optarg,"%lf",&maxs)!=1)
				usage();
			maxsopt=1;
			break;
		case 'v':
			verb=1;
			break;
		case 'h':
		case '?':
		default:
			usage();
			break;
		}
	}

	if (!opt)
		usage();
			
	for(j=optind;j<argc;j++) {
		int id=strtol(argv[j],NULL,0);

		if (!sd.hasid(id))
			continue;
		n=sd.n(id);
		if (matchx) {
			lookup=sd.x(id);
			show=sd.y(id);
		} else {
			lookup=sd.y(id);
			show=sd.x(id);
		}

		imin=imax=-1;
		min=max=match;
		for (i=0;i<n;i++) {
			double val=lookup[i];

			if (    (minlopt && (val<minl))
			     || (maxlopt && (val>maxl))
			     || (minsopt && (show[i]<mins))
			     || (maxsopt && (show[i]>maxs)))
				continue;
			
			if (val<match) {
				if ((imin<0)||(val>min)) {
					min=val;
					imin=i;
				}
			} else {
				if ((imax<0)||(val<max)) {
					max=val;
					imax=i;
				}
			}
		}
		if (imin<0) {
			fprintf(stderr,"ERROR! Didn't find anything "
				"smaller than %g for id %d!\n",match,id);
			continue;
		}
		if (imax<0) {
			fprintf(stderr,"ERROR! Didn't find anything "
				"larger than %g for id %d!\n",match,id);
			continue;
		}

		if (verb) {
			printf("Found best match to be %g < %g <= %g "
			       "at positions %d and %d, respectively.\n",
			       min,match,max,imin,imax);
			printf("Corresponding %c values are %g and %g, "
			       "respectively.\n",matchx ? 'y' : 'x', 
			       show[imin],show[imax]);
		}

		if ((max-match)<DBL_EPSILON) {
			if (verb)
				printf("%g seems close enough!\n->",max);
			result=show[imax];
		} else {
			if (verb)
				printf("Interpolating.. \n->");
			result=show[imax]-(show[imax]-show[imin])
				*(max-match)/(max-min);
		}
		printf("%g\n",result);
	}

}














