#include <SharedData.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

static struct option const long_options[] = {
	{"help",      no_argument,       0, 'h'},
	{"version",   no_argument,       0, 'V'},
	{"warranty",  no_argument,       0, 'W'},
	{"copyright", no_argument,       0, 'C'},
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
	      "usage: %s ids\n"
		"\n"
		"\t-h,-?, --help       this message\n"
		"\t-V,    --version    display version information and exit.\n"
		"\t-W,    --warranty   display licensing terms.\n"
		"\t-C,    --copyright\n\n"
		"cat all ids in one new set\n",
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


int add_id(idset **ta, const char *arg, SharedData *sd) 
{
	int id;
	char *tail;
	errno=0;
	id=strtol(arg,&tail,0);
	if(errno) {
		fprintf(stderr,"range error in %s (should be an id!)\n",arg);
		return 1;
	}
	if(tail==arg) {
		fprintf(stderr," \"%s\" should be an id!)\n",arg);
		return 2;
	}
	if(!sd->hasid(id)) {
		fprintf(stderr,"No id=%d (%s)\n",id,arg);
		return 3;
	}
	*ta=add_to_set(*ta,id,0);
	return 0;
}


int main(int argc,char **argv)
{
	SharedData sd;
	int i,j,k,n,c;
	double *xm;
	double *ym;
	double *xn;
	double *yn;
	idset *tocat=NULL;

	int newid=DS_UNUSED;

	char *logline,*logid=NULL;
	int li=0,oi=0;

	char *idstr=NULL;

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


	for(j=optind;j<argc;j++) 
		if(add_id(&tocat,argv[j],&sd))
			usage();

	for(j=0,k=0;j<tocat->n;j++)
		k+=sd.n(tocat->ids[j]);

	newid=sd.create(k);
	xm=sd.x(newid);
	ym=sd.y(newid);
	for(j=0,k=0;j<tocat->n;j++) {
		xn=sd.x(tocat->ids[j]);
		yn=sd.y(tocat->ids[j]);
		n=sd.n(tocat->ids[j]);
		for (i=0;i<n;i++,k++) {
			xm[k]=xn[i];
			ym[k]=yn[i];
		}
	}
	sd.set_n(newid,k);
	printf("new id=%d\n",newid);
	sd.add_ident(newid,"dscat");
	for(j=0;j<tocat->n;j++)
		oi+=asprintcat(&logid,&li,oi," $%d",tocat->ids[j]);
	sort_set(tocat);
	idstr = string_set(tocat);
	asprintf(&logline,"<*%d*:%s>\t%s%s", newid,
		 idstr, program_invocation_short_name,logid);

	sd.log(logline);

   	sd.changed();
	return 0;
}

