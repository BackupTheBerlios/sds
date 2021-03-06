#include <SharedData.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static struct option const long_options[] = {
	{"help",        no_argument,       0, 'h'},
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
	      "usage: %s IDs\n"
		"\n"
		"-h,-?, --help        this message\n"
		"-V,    --version     display version information and exit.\n"
		"-W,    --warranty    display licensing terms.\n"
		"-C,    --copyright  \n",
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

int sort2(int n,void *sortarr,size_t sizesort,
	  void *refarr ,size_t sizeref,
	  int (*comp)(void *a,void *b))
{
	int l,j,ir,i;
	void *sortd, *refd;
	
	
	if(!(sortd=malloc(sizesort+sizeref))) {
		fprintf(stderr,"sort2 -1\n");
		return -1;
        }
	refd=(void *)((char *)sortd+sizesort);

	l=n/2;
	ir=n-1;

	while(1) {
		if(l>0) {
			l--;

			memcpy(
				sortd,
				(void *)((char *)sortarr+l*sizesort),
				sizesort
				);
			memcpy(
				refd,
				(void *)((char *)refarr+l*sizeref),
				sizeref
				);
		} else {
			memcpy(
				sortd,
				(void *)((char *)sortarr+ir*sizesort),
				sizesort
				);
			memcpy(
				refd,
				(void *)((char *)refarr+ir*sizeref),
				sizeref
				);
			memcpy(
				(void *)((char *)sortarr+ir*sizesort),
				sortarr,
				sizesort
				);
			memcpy(
				(void *)((char *)refarr+ir*sizeref),
				refarr,
				sizeref
				);
			ir--;
			if(ir==0) {
				memcpy(
					sortarr,
					sortd,
					sizesort
					);
				memcpy(
					refarr,
					refd,
					sizeref
					);
				free(sortd);
				return 0;
			}
		}
		i=l;
		j=2*l+1;
		while(j<=ir) {
			if(j<ir)
				if(comp((void *)((char *)sortarr+j*sizesort),
					(void *)((char *)sortarr+(j+1)*sizesort)
					)<0
					)
					j++;
			if(comp(sortd,
				(void *)((char *)sortarr+j*sizesort)
				)<0
				) {
				memcpy(
					(void *)((char *)sortarr+i*sizesort),
					(void *)((char *)sortarr+j*sizesort),
					sizesort
					);
				memcpy(
					(void *)((char *)refarr+i*sizeref),
					(void *)((char *)refarr+j*sizeref),
					sizeref
					);
				i=j;
				j=2*j+1;
			} else
				j=ir+1;
		}
		memcpy(
			(void *)((char *)sortarr+i*sizesort),
			sortd,
			sizesort
			);
		memcpy(
			(void *)((char *)refarr+i*sizeref),
			refd,
			sizeref
			);
        }
}

int fcmp(void *a,void *b)
{
	if((*(double *)a)<(*(double *)b))
		return -1;
	if((*(double *)a)>(*(double *)b))
		return 1;
	
	return 0;
}


int main(int argc,char **argv)
{
	SharedData hallo;
	int i,max,c;

	int id;
	int changed;

	while ((c = getopt_long(argc, argv, "h?VWC",
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

		max=hallo.n(id);
		sort2(max,hallo.x(id),sizeof(double),
		      hallo.y(id),sizeof(double),fcmp);
		off+=asprintcat(&logline,&ll,off," $%d",id);
		hallo.log(logline);
		free(logline);
		changed=1;
	}
	if(changed)
		hallo.changed();

	return 0;
}

