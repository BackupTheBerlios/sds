#include <SharedData.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

static struct option const long_options[] = {
	{"factor",    required_argument, 0, 'f'},
	{"plus",      required_argument, 0, 'p'},
	{"minus",     required_argument, 0, 'm'},
	{"new",       no_argument,       0, 'n'},
	{"help",      no_argument,       0, 'h'},
	{"version",   no_argument,       0, 'V'},
	{"warranty",  no_argument,       0, 'W'},
	{"copyright", no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "$Id: dsadd.cc,v 1.2 2004/02/08 21:19:23 vstein Exp $";

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
	      "usage: %s [-n] [-m|-p|-f nums] ids\n"
		"\n"
		"\t-f,    --factor     comma seperated list of factors in"
		" sum\n"
		"\t-p,    --plus       assume factor=+(1/n), where n is the\n"
		"\t                    number of data files to add\n"
		"\t-m,    --minus      assume factor=-1 all but the first\n"
		"\t-n,    --new        create new data set for the result\n"
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

struct ToAdd {
	int id;
	double factor;
};

int add_got(ToAdd **ta, int *tasi, int ind, double got)
{
	if(ind>=*tasi) {
		*tasi+=10;
		*ta=(ToAdd *)xrealloc(*ta,sizeof(ToAdd)**tasi);
	}
	(*ta)[ind].factor=got;
	return 1;
}

int add_id(ToAdd *ta, int ind,const char *arg,SharedData *sd) 
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
	ta[ind].id=id;
	return 0;
}

int set_factor(const char *arg, ToAdd **ta, int *tasi)
{
	char *tmp,*pd;
	char *tail;
	double got;
	int ind=0;
	
	tmp=strtok((pd=strdup(arg)),",");
	do {
		errno=0;
		got=strtod(tmp,&tail);
		if(errno) {
			fprintf(stderr,"factor (%s) out of range in %s\n",
				tmp,arg);
			return 1;
		}
		if(tmp==tail) {
			fprintf(stderr,"factor (%s) has no valid"
				" floating point syntax in %s\n",
				tmp,arg);
			return 2;
		}
		ind+=add_got(ta,tasi,ind,got);
	} while((tmp=strtok(NULL,",")));
	*ta=(ToAdd*)xrealloc(*ta,sizeof(ToAdd)*ind);
	*tasi=ind;
	return 0;
}

int main(int argc,char **argv)
{
	SharedData sd;
	int i,j,k,l,c;
	int plus=0;
	int minus=0;
	double *x0;
	double *y0;
	double *xm;
	double *ym;
	double *xn;
	double *yn;
	double tol,min,max,factor;
	ToAdd *toadd=NULL;
	int tasi=0;
	int newds=0;
	int newid=DS_UNUSED;

	char *logline=NULL;
	int ll=0,off=0;
	char *factor_arg=NULL;

	while ((c = getopt_long(argc, argv, "f:npmh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'f':
			factor_arg=optarg;
			if(set_factor(optarg,&toadd,&tasi))
				usage();
			break;
		case 'p':
			if(minus)
				usage();
			plus=1;
			break;
		case 'm':
			if(plus)
				usage();
			minus=1;
			break;
		case 'n':
			newds=1;
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

	if(tasi&&tasi!=(argc-optind)) {
		fprintf(stderr,"The number of doubles in option factor has to"
			" match the number of data set toadd!\n");
		usage();
	}

	toadd=(ToAdd*)xrealloc(toadd,sizeof(ToAdd)*(argc-optind));
	if(!tasi) {
		tasi=argc-optind;
		for(i=0;i<tasi;i++)
			toadd[i].factor=1.0;
	}
	for(i=0,j=optind;j<argc;i++,j++) 
		if(add_id(toadd,i,argv[j],&sd))
			usage();

	if(plus)
		for(i=0;i<tasi;i++)
			toadd[i].factor=1.0/(double)tasi;

	if(minus)
		for(i=1;i<tasi;i++)
			toadd[i].factor=-1.0;

	/* what to do:
	 * --check consitency
	 * --create if new
	 * --do the math
	 */

	if(tasi==0)
		usage();
	
	k=sd.n(toadd[0].id);
	x0=sd.x(toadd[0].id);
	y0=sd.y(toadd[0].id);

	min=max=x0[0];
	for(l=1;l<k;l++) {
		min=(min<x0[l] ? min : x0[l]);
		max=(max>x0[l] ? max : x0[l]);
	}
	tol=fabs(max-min)/2.0/k;
	for(i=1;i<tasi;i++) {
		if((unsigned)k!=sd.n(toadd[i].id)) {
			fprintf(stderr,"All the sets have to have"
				"the same amount of points\n");
			return 1;
		}
		xm=sd.x(toadd[i].id);
		for(l=0;l<k;l++) {
			if(fabs(x0[l]-xm[l])>tol) {
				fprintf(stderr,"All the sets have"
					" to share theire"
					" x-scale\n");
				return 2;
			}
		}
	}	

	factor=toadd[0].factor;

	if(newds) {
		idset *ids=NULL;
		char *idstr=NULL;
		newid=sd.create(k);
		xm=sd.x(newid);
		ym=sd.y(newid);
		for(i=0;i<k;i++) {
			xm[i]=x0[i];
			ym[i]=y0[i]*factor;
		}
		sd.set_n(newid,k);
		printf("new id=%d\n",newid);
		sd.add_ident(newid,"dsadd");
		for(i=0;i<tasi;i++)
			ids=add_to_set(ids,toadd[i].id,0);
		sort_set(ids);
		idstr = string_set(ids);
		off=asprintcat(&logline,&ll,0,"<*%d*:%s>\t%s", newid,
			       idstr, program_invocation_short_name);
	} else {
		idset *ids=NULL;
		char *idstr=NULL;
		xm=x0;
		ym=y0;
		for(i=0;i<k;i++)
			ym[i]*=factor;
		for(i=1;i<tasi;i++)
			ids=add_to_set(ids,toadd[i].id,0);
		if(tasi>1) {
			sort_set(ids);
			idstr = string_set(ids);
			off=asprintcat(&logline,&ll,0,"<%d:%s>\t%s",
				       toadd[0].id,idstr,
				       program_invocation_short_name);
		} else
			off=asprintcat(&logline,&ll,0,"<%d>\t%s",
				       toadd[0].id, 
				       program_invocation_short_name);
		sd.add_ident(toadd[0].id,"dsadd");
	}

	for(i=1;i<tasi;i++) {
		xn=sd.x(toadd[i].id);
		yn=sd.y(toadd[i].id);
		factor=toadd[i].factor;
		for(l=0;l<k;l++)
			ym[l]+=yn[l]*factor;
	}

	if(factor_arg)
		off+=asprintcat(&logline,&ll,off," -f \"%s\"",factor_arg);

	if(plus)
		off+=asprintcat(&logline,&ll,off," -p");

	if(minus)
		off+=asprintcat(&logline,&ll,off," -m");
	
	if(newds)
		off+=asprintcat(&logline,&ll,off," -n");

	for(i=0;i<tasi;i++)
		off+=asprintcat(&logline,&ll,off," $%d",toadd[i].id);

	sd.log(logline);

   	sd.changed();
	return 0;
}

