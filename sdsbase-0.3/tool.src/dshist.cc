#include <SharedData.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <float.h>
#include <math.h>

static struct option const long_options[] = {
	{"help",      no_argument,       0, 'h'},
	{"version",   no_argument,       0, 'V'},
	{"warranty",  no_argument,       0, 'W'},
	{"copyright", no_argument,       0, 'C'},
	{"delta",     required_argument, 0, 'd'},
	{"NoP",       required_argument, 0, 'n'},
	{"min",       required_argument, 0, 'i'},
	{"max",       required_argument, 0, 'a'},
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
	      "usage: %s [-d delta|-n NoP] [-i min] [-a max] ids\n"
		"\n"
		"\t-d     --delta      slotsize\n"
		"\t-n     --NoP        slotnumber\n"
		"\t-i,    --min        \n"
		"\t-a,    --max        \n"
		"\t-h,-?, --help       this message\n"
		"\t-V,    --version    display version information and exit.\n"
		"\t-W,    --warranty   display licensing terms.\n"
		"\t-C,    --copyright\n\n"
		"Computes histogram of y-data on interval min<=y<=max. "
		"Min and max are\nextracted from data if not specified. "
		"Either the number of slots or the\nslot width is given, "
		"with delta=(max-min)/NoP. Slot i contains the count\nfor "
		"min+i*delta <= y < min+(i+1)*delta (i=0..NoP-2). To work "
		"around the\n'Zaun-Latten'-problem, the last slot (i=NoP-1) "
		"also counts for y==max:\nmax-delta <= y <= max.\n ",
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
	int c,i,j;
	int numopt,minopt,maxopt;
	double delta,min,max;
	int number;

	numopt=0; minopt=0; maxopt=0;

	while ((c = getopt_long(argc, argv, "d:n:i:a:h?VWC",
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
		case 'd':
			if (numopt)
				usage();
			if(sscanf(optarg,"%lf",&delta)!=1)
				usage();
			numopt=1;
			break;
		case 'n':
			if (numopt)
				usage();
			if(sscanf(optarg,"%d",&number)!=1)
				usage();
			numopt=2;
			break;
		case 'i':
			if(sscanf(optarg,"%lf",&min)!=1)
				usage();
			minopt=1;
			break;
		case 'a':
			if(sscanf(optarg,"%lf",&max)!=1)
				usage();
			maxopt=1;
			break;
		case 'h':
		case '?':
		default:
			usage();
			break;
		}
	}

	if (!numopt)
		usage();

	SharedData sd;
	int first=1;
	for(j=optind;j<argc;j++) {
		int id=strtol(argv[j],NULL,0);
		
		if (!sd.hasid(id))
			continue;

		int n=sd.n(id);
		double *ny=sd.y(id);

		if (!minopt)
			for (i=1,min=ny[0];i<n;i++)
				if (min>ny[i]) min=ny[i];
		if (!maxopt)
			for (i=1,max=ny[0];i<n;i++)
				if (max<ny[i]) max=ny[i];
 		if (numopt>1)
			delta=(max-min)/number;
		else
			number=(int)ceil((max-min)/delta);
		
		int m=sd.create(number);
		double *mx=sd.x(m);
		double *my=sd.y(m);
		for (i=0;i<number;i++) {
			my[i]=0;
			mx[i]=min+i*delta;
		}
		int s;
		for (i=0;i<n;i++) {
			if ((ny[i]<min)||(ny[i]>max))
				continue;
			s=(int)((ny[i]-min)/delta);
			if (s==number)
				s--;
			my[s]++;
		}
		sd.set_n(m,number);
		sd.add_ident(m,"dshist");
		if (first) {
			first=0;
			printf("new id(s)=");
		} else 
			printf(" ");
		printf("%d",m);
		char *logline;
		if (numopt>1)
			asprintf(&logline,"<*%d*:%d>\t%s "
				 "-n %d -i %g -a %g $%d", 
				 m,id,program_invocation_short_name,
				 number,min,max,id);
		else 
			asprintf(&logline,"<*%d*:%d>\t%s "
				 "-d %g -i %g -a %g $%d", 
				 m,id,program_invocation_short_name,
				 delta,min,max,id);
		sd.log(logline);
	}
	sd.changed();
	printf("\n");
}














