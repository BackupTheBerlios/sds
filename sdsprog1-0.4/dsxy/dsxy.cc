#include <SharedData.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>

static struct option const long_options[] = {
	{"replace_x", no_argument,       0, 'r'},
	{"replace_y", no_argument,       0, 'R'},
	{"help",      no_argument,       0, 'h'},
	{"version",   no_argument,       0, 'V'},
	{"warranty",  no_argument,       0, 'W'},
	{"copyright", no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dsxy.cc,v 1.1 2004/02/08 21:14:35 vstein Exp $";

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
	      "usage: %s [-r|-R] id1 id2 [...]\n"
		"\nCreate a dataset with the y-values of id1 as the x-array"
		"and the\ny-values of id2 as the y-array.  The x-arrays of "
		"id1 and id2 have to match.\n"
		"\n"
		"\t-r,    --replace_x  store result in id1\n"
		"\t-R,    --replace_y  store result in id2\n"
		"\t-h,-?, --help       this message\n"
		"\t-V,    --version    display version information and exit.\n"
		"\t-W,    --warranty   display licensing terms.\n"
		"\t-C,    --copyright\n",
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
	int i,j,k,l,c;
	double *xn, *yn;
	double *x1, *y1;
	double *x2, *y2;
        int id1, id2;
	double tol,min,max;

	int replace=0;
	int newid=DS_UNUSED;

	char *logline=NULL;
	int ll=0,off=0;

	while ((c = getopt_long(argc, argv, "h?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'r':
			if (replace)
				usage();
			replace=1;
			break;
		case 'R':
			if (replace)
				usage();
			replace=2;
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

	if((argc-optind)%2) {
		usage();
	} 

	for(i=optind;i<argc;i+=2) {
		
		/* check ids */
		errno=0;
		id1=strtol(argv[i],NULL,0);
		if (errno) {
			fprintf (stderr,"ID Overflow in %s\n",argv[i]);
			return 1;
		}
		if(!sd.hasid(id1)) {
			fprintf(stderr,"No id %d (%s)\n",id1,argv[i]);
			return 1;
		}
		errno=0;
		id2=strtol(argv[i+1],NULL,0);
		if (errno) {
			fprintf (stderr,"ID Overflow in %s\n",argv[i+1]);
			return 1;
		}
		if(!sd.hasid(id2)) {
			fprintf(stderr,"No id %d (%s)\n",id2,argv[i+1]);
			return 1;
		}

		/* check for common x-arrays */
		k=sd.n(id1);
		x1=sd.x(id1);
		y1=sd.y(id1);

		min=max=x1[0];
		for(l=1;l<k;l++) {
			min=(min<x1[l] ? min : x1[l]);
			max=(max>x1[l] ? max : x1[l]);
		}
		tol=fabs(max-min)/2.0/k;
		
		if((unsigned)k!=sd.n(id2)) {
			fprintf(stderr,"All the sets have to have"
				"the same amount of points\n");
			return 2;
		}
		
		x2=sd.x(id2);
		y2=sd.y(id2);
		for(l=0;l<k;l++) {
			if(fabs(x1[l]-x2[l])>tol) {
				fprintf(stderr,"All the sets have"
					" to share their"
					" x-scale\n");
				return 2;
			}
		}

		switch (replace) {
		default:
		case 0:
			newid=sd.create(k);
			xn=sd.x(newid);
			yn=sd.y(newid);
			sd.set_n(newid,k);
			printf("new id=%d\n",newid);
			sd.add_ident(newid,"dsxy");
			off=asprintcat(&logline,&ll,0,"<*%d*:%d,%d>\t%s", 
				       newid, id1, id2, 
				       program_invocation_short_name);
			break;
		case 1:
			xn=x1;
			yn=y1;
			off=asprintcat(&logline,&ll,0,"<%d:%d>\t%s -r", 
				       id1, id2, 
				       program_invocation_short_name);
			break;
		case 2:
			xn=x2;
			yn=y2;
			off=asprintcat(&logline,&ll,0,"<%d:%d>\t%s -R", 
				       id2, id1, 
				       program_invocation_short_name);
			break;
		}
		
		for(j=0;j<k;j++) {
			xn[j]=y1[j];
			yn[j]=y2[j];   /* redundant for replace=2! */
		}
		off+=asprintcat(&logline,&ll,off," $%d $%d", id1, id2);
		
		sd.log(logline);
	}

	sd.changed();
	return 0;

}

