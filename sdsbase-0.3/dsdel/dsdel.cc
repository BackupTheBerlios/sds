#include <stdio.h>
#include <SharedData.h>
#include <getopt.h>
#include <string.h>
#include <math.h>
#include "dsdel.h"
#include <errno.h>
#include <stdlib.h>

static struct option const long_options[] =
{
	{"help", no_argument, 0, 'h'},
	{"test"   , required_argument, 0, 't'},
	{"version", no_argument, 0, 'v'},
	{"warranty", no_argument, 0, 'w'},
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
	      "usage: %s [-t exp] ids\n"
		"\n"
		"-t,    --test       mathexp ==|!=|>|<|>=|<= mathexp\n"
		"-h,-?, --help       this message\n"
		"-v,    --version    display version information and exit.\n"
		"-w,    --warranty   display licensing terms.\n", 
		program_invocation_short_name);
	exit(0);
}

void version(void)
{
	fprintf(stderr, "%s version %s\n", program_invocation_short_name,
		verstring);

	exit(0);
}

extern "C" {
	int dsdel_yyparse(void);
	extern FILE *dsdel_yyin;
	extern char bef[];
	extern double cons[];
	extern int bi,ci;
	extern int dsdel_yydebug;
}

int ifx(double *xa,double *ya, int max,
	  char *bef, int bi, double *cons,int,
	  int ind)
{
	int i,j,sti;
	double st[500];
	

	for(i=0,j=0,sti=0;i<bi;i++) {
		switch(bef[i]) {
		case Y_PUT:
			st[sti++]=cons[j++];
			break;
		case Y_PLUS:
			st[sti-2]+=st[sti-1];
			sti--;
			break;
		case Y_MINUS:
			st[sti-2]-=st[sti-1];
			sti--;
			break;
		case Y_MULT:
			st[sti-2]*=st[sti-1];
			sti--;
			break;
		case Y_DIV:
			st[sti-2]/=st[sti-1];
			sti--;
			break;
		case Y_NEG:
			st[sti-1]=-st[sti-1];
			break;
		case Y_PUTIND:
			st[sti++]=ind;
			break;
		case Y_PUTMAXIND:
			st[sti++]=max;
			break;
		case Y_XARR:
			if(st[sti-1]<0)
				st[sti-1]=xa[0];
			else if(st[sti-1]>=max)
				st[sti-1]=xa[max-1];
			else 
				st[sti-1]=xa[(int)st[sti-1]];
			break;
		case Y_YARR:
			if(st[sti-1]<0)
				st[sti-1]=ya[0];
			else if(st[sti-1]>=max)
				st[sti-1]=ya[max-1];
			else 
				st[sti-1]=ya[(int)st[sti-1]];
			break;
		case Y_X:
			st[sti++]=xa[ind];
			break;
		case Y_Y:
			st[sti++]=ya[ind];
			break;
                case Y_SIN:
			st[sti-1]=sin(st[sti-1]);
			break;
		case Y_COS:
			st[sti-1]=cos(st[sti-1]);
			break;
		case Y_TAN:
			st[sti-1]=tan(st[sti-1]);
			break;
		case Y_ASIN:
			st[sti-1]=asin(st[sti-1]);
			break;
		case Y_ACOS:
			st[sti-1]=acos(st[sti-1]);
			break;
		case Y_ATAN:
			st[sti-1]=atan(st[sti-1]);
			break;
		case Y_ATAN2:
			st[sti-2]=atan2(st[sti-2],st[sti-1]);
			sti--;
			break;
		case Y_EXP:
			st[sti-1]=exp(st[sti-1]);
			break;
		case Y_LOG:
			st[sti-1]=log(st[sti-1]);
			break;
		case Y_LOG10:
			st[sti-1]=log10(st[sti-1]);
			break;
		case Y_POW:
			st[sti-2]=pow(st[sti-2],st[sti-1]);
			sti--;
			break;
		case Y_SQRT:
			st[sti-1]=sqrt(st[sti-1]);
			break;
		case Y_CBRT:
			st[sti-1]=cbrt(st[sti-1]);
			break;
		case Y_HYPOT:
			st[sti-2]=hypot(st[sti-2],st[sti-1]);
			sti--;
			break;
		case Y_EXPM1:
			st[sti-1]=expm1(st[sti-1]);
			break;
		case Y_LOG1P:
			st[sti-1]=log1p(st[sti-1]);
			break;
		case Y_SINH:
			st[sti-1]=sinh(st[sti-1]);
			break;
		case Y_COSH:
			st[sti-1]=cosh(st[sti-1]);
			break;
		case Y_TANH:
			st[sti-1]=tanh(st[sti-1]);
			break;
		case Y_ASINH:
			st[sti-1]=asinh(st[sti-1]);
			break;
		case Y_ACOSH:
			st[sti-1]=acosh(st[sti-1]);
			break;
		case Y_ATANH:
			st[sti-1]=atanh(st[sti-1]);
			break;
		case Y_FABS:
			st[sti-1]=fabs(st[sti-1]);
			break;
                case Y_NE:
			st[sti-2]=(st[sti-2]!=st[sti-1]);
			sti--;
 			break;
		case Y_EQ:
			st[sti-2]=(st[sti-2]==st[sti-1]);
			sti--;
			break;
		case Y_LE:
			st[sti-2]=(st[sti-2]<=st[sti-1]);
			sti--;
			break;
		case Y_GE:
			st[sti-2]=(st[sti-2]>=st[sti-1]);
			sti--;
			break;
		case Y_LT:
			st[sti-2]=(st[sti-2]<st[sti-1]);
			sti--;
			break;
		case Y_GT:
			st[sti-2]=(st[sti-2]>st[sti-1]);
			sti--;
			break;
		case Y_AND:
			st[sti-2]=(st[sti-2]&&st[sti-1]);
			sti--;
			break;
		case Y_OR:
			st[sti-2]=(st[sti-2]||st[sti-1]);
			sti--;
			break;
		case Y_NOT:
			st[sti-1]=!(st[sti-1]);
			break;
		case Y_FMOD:
			st[sti-2]=fmod(st[sti-2],st[sti-1]);
			sti--;
			break;
		}
	}
	return (int)st[0];
}

void remove(double *x, double *y, int max, int start, int i);

int main(int argc,char *argv[])
{
	SharedData hallo;
	int i,j,k,jmax,kmax;
	int start;
	int changed=0;

	char *xbz=(char *)0;
	int c;
	double *x,*y;
	double *xa,*ya;
	int id;

	bi=ci=0;
// 	dsdel_yydebug=1;

	while ((c = getopt_long(argc, argv, "t:h?vw",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 't':
			dsdel_yyin=tmpfile();
			fprintf(dsdel_yyin,"%s\n",(xbz=optarg));
			rewind(dsdel_yyin);
			if(dsdel_yyparse())
				exit(1);
			fclose(dsdel_yyin);
			break;
		case 'w':
			warranty();
			break;
		case 'v':
			version();
			break;
		case 'h':
		case '?':
		default:
			usage();
			break;
		}
	}

	if(!bi)
		usage();

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

		x=hallo.x(id);
		y=hallo.y(id);
		jmax=kmax=hallo.n(id);
		xa=(double *)xmalloc(sizeof(double)*kmax);
		ya=(double *)xmalloc(sizeof(double)*kmax);
		memcpy(xa,x,sizeof(double)*kmax);
		memcpy(ya,y,sizeof(double)*kmax);
		j=k=0;
		while(j<jmax) {
			while(k<kmax&&!ifx(xa,ya,kmax,bef,bi,cons,ci,k)) {
				j++;
				k++;
			}
			if(k==kmax)
				break;
			start=j;
			while(k<kmax&&ifx(xa,ya,kmax,bef,bi,cons,ci,k)) {
				j++;
				k++;
			}
			remove(x,y,jmax,start,j);
			jmax-=j-start;
			j=start;
		}
		hallo.set_n(id,jmax);
		free(xa);
		free(ya);
		changed=1;
		if(blanks_in(xbz))
			off+=asprintcat(&logline,&ll,off," -t \"%s\"",xbz);
		else
			off+=asprintcat(&logline,&ll,off," -t %s",xbz);
		off+=asprintcat(&logline,&ll,off," $%d",id);
		hallo.log(logline);
		free(logline);
	}
	if(changed)
		hallo.changed();

	return 0;
}

void remove(double *x, double *y, int max, int start, int i)
{
	int bytes=sizeof(double)*(max-i);
	memmove(x+start,x+i,bytes);
	memmove(y+start,y+i,bytes);
}
