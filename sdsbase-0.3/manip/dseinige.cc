#include <SharedData.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>

static struct option const long_options[] =
{
	{"help", no_argument, 0, 'h'},
	{"step", required_argument, 0, 'p'},
	{"offset", required_argument, 0, 'q'},
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
	      "usage: %s -p step [-q offset] id(s)\n"
		"\n"
		"-p,    --step\n"
		"-q,    --offset\n"
		"-h,-?, --help       this message\n"
		"\n"
		"keep every step-th data point starting with offset\n"
		"default offset is 0\n",
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

void einige(SharedData *sd, int id,int p, int q)
{
	double *x=sd->x(id);
	double *y=sd->y(id);
	int i, j;
	int n=sd->n(id);
	
	for (i=0, j=q;j<n;i++,j+=p) {
	  x[i]=x[j];
	  y[i]=y[j];
	}
	  
	sd->set_n(id,i);
}

int main(int argc,char **argv)
{
	SharedData hallo;
	int i,c;
	int step, offset=0;
	int step_unset=1;
	int off_unset=1;
	int changed=0;
	int id;
	
	while ((c = getopt_long(argc, argv, "p:q:h?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'p':
			step=atoi(optarg);
			step_unset=0;
			break;
		case 'q':
			offset=atoi(optarg);
			off_unset=0;
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

	if (step_unset) {
	  fprintf(stderr,"paramter --step is required! \n");
	  usage();
	}

	if (step<=0) {
	  fprintf(stderr,"paramter step has to be positive! \n");
	  usage();
	}

	if (offset<0) {
	  fprintf(stderr,"paramter offset must not be negative! \n");
	  usage();
	}

	if (offset>=step) {
	  fprintf(stderr,"paramter offset has to be smaller than step! \n");
	  usage();
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

		einige(&hallo,id,step,offset);
		if (off_unset)
			off+=asprintcat(&logline,&ll,off," -p %d",step);
		else
			off+=asprintcat(&logline,&ll,off," -p %d -q %d",
					step,offset);
		off+=asprintcat(&logline,&ll,off," $%d",id);
		hallo.log(logline);
		free(logline);
		changed=1;
	}
	if(changed)
		hallo.changed();
	return 0;
}

