#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SharedData.h>
//#include <sds/color.h>
//#include <sds/linestyle.h>
//#include <sds/symbolstyle.h>

#include <getopt.h>
#include <stdlib.h>
#include <errno.h>

#define DSSTAT_OPTION "DSSTAT_OPTION"

static struct option const long_options[] = {
	{"long",        no_argument,       0, 'l'},
	{"min_x",	no_argument,       0, 'I'},
        {"max_x",	no_argument,       0, 'A'},
	{"sum_x",	no_argument,       0, 'x'},
	{"sum_x2",	no_argument,       0, 'X'},
	{"mean_x",	no_argument,       0, 'M'},
	{"var_x",	no_argument,       0, 'V'},
	{"rms_x",       no_argument,       0, 'R'},
	{"std_x",	no_argument,       0, 'T'},
	{"min_y",	no_argument,       0, 'i'},
	{"max_y",	no_argument,       0, 'a'},
	{"sum_y",	no_argument,       0, 'y'},
	{"sum_y2",	no_argument,       0, 'Y'},
	{"mean_y",	no_argument,       0, 'm'},
	{"var_y",	no_argument,       0, 'v'},
	{"rms_y",       no_argument,       0, 'r'},
	{"std_y",	no_argument,       0, 't'},
	{"sum_xy",	no_argument,       0, 'c'},
	{"a",           no_argument,       0, 1000 },
	{"b",           no_argument,       0, 1001 },
	{"NoP",		no_argument,       0, 'n'},
	{"help",        no_argument,       0, 'h'},
	{"version",     no_argument,       0, 2000 },
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "0.4";

extern char *program_invocation_short_name;

struct Point {
	double x,y;
	int ind;
};

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
		"usage: %s [options] [ID(s)]\n"
		"\n"
		"-l,    --long      specify output fields in the\n"
		"                   enviroment variable %s\n"
		"                   using a string of short options\n"
		"\n"
		"    the output fields are ...\n"
		"-I     --min_x	    minimum of the x-values\n"
		"-A     --max_x	    maximum of the x-values\n"
		"-x     --sum_x	    sum of the x-values\n"
		"-X     --sum_x2    sum of squares of the x-values\n"
		"-M     --mean_x    mean of the x-values\n"
		"-V     --var_x	    variance of the x-values\n"
		"-R     --rms_x     rms of the x-values\n"
		"-T     --std_x	    standard deviation of the x-values\n"
		"-i     --min_y	    minimum of the y-values\n"
		"-a     --max_y	    maximum of the y-values\n"
		"-y     --sum_y	    sum of the y-values\n"
		"-Y     --sum_y2    sum of squares of the y-values\n"
		"-m     --mean_y    mean of the y-values\n"
		"-v     --var_y	    variance of the y-values\n"
		"-r     --rms_y     rms of the y-values\n"
		"-t     --std_y	    standard deviation of the y-values\n"
		"-c     --sum_xy    sum of product of the x- and y-values\n"
		"       --a         slope of linear regression\n"
		"       --b         constant term in linear regression\n"
		"-n     --NoP	    number of points\n"
		"\n"
		"    the other otpions are ...\n"
		"-h,-?, --help      this message\n"
		"       --version   display version information and exit.\n"
		"-W,    --warranty  display licensing terms.\n"
		"-C,    --copyright\n",
		program_invocation_short_name,DSSTAT_OPTION);

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

void match_sdind(int *id,int nid,SharedData *sd);
int max_sdsn(int *id,int nid,int n,int *use);

int main(int argc, char *argv[])
{
	char *dsstat_option;
	int i,j,c;
	int sdsn;
	int min_x=0;
	int max_x=0;
	int sum_x=0;
	int sum_xx=0;
	int mean_x=0;
	int var_x=0;
	int rms_x=0;
	int std_x=0;
	int min_y=0;
	int max_y=0;
	int sum_y=0;
	int sum_yy=0;
	int mean_y=0;
	int var_y=0;
	int rms_y=0;
	int std_y=0;
	int sum_xy=0;
	int ax=0;
	int bx=0;
	int nop=0;
	int lo=0;
	int number_of_options;
	int number_of_ids=0;
	char *tail;

	SharedData sd;
	int *sdind,indsize;

	while ((c = getopt_long(argc, argv, "lIAxXMVRTiayYmvrtnc h?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'l' :
			lo=1;
			break;
		case 'I':
			min_x=1;
			break;
		case 'A':
			max_x=1;
			break;
		case 'x':
			sum_x=1;
			break;
		case 'X':
			sum_xx=1;
			break;
		case 'M':
			mean_x=1;
			break;
		case 'V':
			var_x=1;
			break;
		case 'R':
			rms_x=1;
			break;
		case 'T':
			std_x=1;
			break;
		case 'i':
			min_y=1;
			break;
		case 'a':
			max_y=1;
			break;
		case 'y':
			sum_y=1;
			break;
		case 'Y':
			sum_yy=1;
			break;
		case 'm':
			mean_y=1;
			break;
		case 'v':
			var_y=1;
			break;
		case 'r':
			rms_y=1;
			break;
		case 't':
			std_y=1;
			break;
		case 1000:
			ax=1;
			break;
		case 1001:
			bx=1;
			break;
		case 'n':
			nop=1;
			break;
		case 'c':
			sum_xy=1;
			break;
		case 2000:
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

	if(lo) {
		if((dsstat_option=getenv(DSSTAT_OPTION))) {
			while(*dsstat_option) {
				switch(*dsstat_option) {
				case 'I':
					min_x=1;
					break;
				case 'A':
					max_x=1;
					break;
				case 'x':
					sum_x=1;
					break;
				case 'X':
					sum_xx=1;
					break;
				case 'M':
					mean_x=1;
					break;
				case 'V':
					var_x=1;
					break;
				case 'R':
					rms_x=1;
					break;
				case 'T':
					std_x=1;
					break;
				case 'i':
					min_y=1;
					break;
				case 'a':
					max_y=1;
					break;
				case 'y':
					sum_y=1;
					break;
				case 'Y':
					sum_yy=1;
					break;
				case 'm':
					mean_y=1;
					break;
				case 'v':
					var_y=1;
					break;
				case 'r':
					rms_y=1;
					break;
				case 't':
					std_y=1;
					break;
				case 'n':
					nop=1;
					break;
				case 'c':
					sum_xy=1;
					break;
				default:
					fprintf(stderr,"wrong field"
						" specification in %s=%s",
						DSSTAT_OPTION,
						getenv(DSSTAT_OPTION));
					usage();
				}
				dsstat_option++;
			}
		} else {
			fprintf(stderr,"No enviroment variable %s!\n",
				DSSTAT_OPTION);
			usage();
		}
	}


	number_of_options=min_x+max_x+sum_x+sum_xx+mean_x+var_x+rms_x+std_x+
		min_y+max_y+sum_y+sum_yy+mean_y+var_y+rms_y+std_y+
		ax+bx+nop+sum_xy;
	
	if(optind==argc)
		sdind=sd.record_state(&indsize);
	else {
		sdind=(int*)xmalloc(sizeof(int)*(argc-optind));
		for(indsize=0,j=optind;j<argc;indsize++,j++) {
			errno=0;
			sdind[indsize]=strtol(argv[j],&tail,0);
			if(errno||*tail!=0) {
				fprintf(stderr,"%s not an integer\n",argv[j]);
				indsize--;
			}
		}
		match_sdind(sdind,indsize,&sd);
	}
 
	sdsn=max_sdsn(sdind,indsize,strlen("id"),&number_of_ids);


	if((!number_of_options)||(!number_of_ids))
		return 0;

	if(number_of_options>1) {
		if(number_of_ids>1)
			printf("%-*s",sdsn,"id");
		if(min_x)
			printf(" %-41s","                 min_x");
		if(max_x)
			printf(" %-41s","                 max_x");
		if(sum_x)
			printf(" %-14s","     sum_x");
		if(sum_xx)
			printf(" %-14s","    sum_x2");
		if(mean_x)
			printf(" %-14s","    mean_x");
		if(var_x)
			printf(" %-14s","     var_x");
		if(rms_x)
			printf(" %-14s","     rms_x");
		if(std_x)
			printf(" %-14s","     std_x");
		if(min_y)
			printf(" %-41s","                 min_y");
		if(max_y)
			printf(" %-41s","                 max_y");
		if(sum_y)
			printf(" %-14s","     sum_y");
		if(sum_yy)
			printf(" %-14s","    sum_y2");
		if(mean_y)
			printf(" %-14s","    mean_y");
		if(var_y)
			printf(" %-14s","     var_y");
		if(rms_y)
			printf(" %-14s","     rms_y");
		if(std_y)
			printf(" %-14s","     std_y");
		if(sum_xy)
			printf(" %-14s","    sum_xy");
		if(ax)
			printf(" %-14s","       a");
		if(bx)
			printf(" %-14s","       b");
		if(nop)
			printf(" %-7s","  NoP");
		printf("\n");
		if(min_x||min_y||max_x||max_y) {
			if(number_of_ids>1)
				printf("%-*s",sdsn," ");
			if(min_x)
				printf(" %-41s","   ind"
				       "          x             y");
			if(max_x)
				printf(" %-41s","   ind"
				       "           x              y");
			if(sum_x)
				printf(" %-14s"," ");
			if(sum_xx)
				printf(" %-14s"," ");
			if(mean_x)
				printf(" %-14s"," ");
			if(var_x)
				printf(" %-14s"," ");
			if(rms_x)
				printf(" %-14s"," ");
			if(std_x)
				printf(" %-14s"," ");
			if(min_y)
				printf(" %-41s","   ind"
				       "           x              y");
			if(max_y)
				printf(" %-41s","   ind"
				       "           x              y");
			printf("\n");
		}
	}

	for(i=0;i<indsize;i++) {
		if(sdind[i]==DS_UNUSED)
			continue;
		int n=sd.n(sdind[i]);
		double *x=sd.x(sdind[i]);
		double *y=sd.y(sdind[i]);
		double  x_sum,xx_sum,y_sum,yy_sum,xy_sum,x_a,x_b,det;
		Point	x_min,x_max,y_min,y_max;
		double  x_mean=0.0,
			y_mean=0.0,
			x_var=0.0,
			y_var=0.0,
			x_std=0.0,
			y_std=0.0;
		if(!n) {
			x_sum=0.0;
			xx_sum=0.0;
			xy_sum=0.0;
			y_sum=0.0;
			yy_sum=0.0;
			x_min.x=NAN;
			x_min.y=NAN;
			x_min.ind=0;
			x_max.x=NAN;
			x_max.y=NAN;
			x_max.ind=0;
			y_min.x=NAN;
			y_min.y=NAN;
			y_min.ind=0;
			y_max.x=NAN;
			y_max.y=NAN;
			y_max.ind=0;
		} else {
			x_sum=x[0];
			xx_sum=x[0]*x[0];
			xy_sum=x[0]*y[0];
			y_sum=y[0];
			yy_sum=y[0]*y[0];
			x_min.x=x[0];
			x_min.y=y[0];
			x_min.ind=0;
			x_max.x=x[0];
			x_max.y=y[0];
			x_max.ind=0;
			y_min.x=x[0];
			y_min.y=y[0];
			y_min.ind=0;
			y_max.x=x[0];
			y_max.y=y[0];
			y_max.ind=0;
		}
		for(j=1;j<n;j++) {
			if(x_min.x>x[j]) {
				x_min.x=x[j];
				x_min.y=y[j];
				x_min.ind=j;
			}
			if(y_min.y>y[j]) {
				y_min.x=x[j];
				y_min.y=y[j];
				y_min.ind=j;
			}
			if (x_max.x<x[j]) {
				x_max.x=x[j];
				x_max.y=y[j];
				x_max.ind=j;
			}
			if(y_max.y<y[j]) {
				y_max.x=x[j];
				y_max.y=y[j];
				y_max.ind=j;
			}
			x_sum+=x[j];
			xx_sum+=x[j]*x[j];
			y_sum+=y[j];
			yy_sum+=y[j]*y[j];
			xy_sum+=x[j]*y[j];
		}
		if(n>0) {
			x_mean=x_sum/(double)n;
			y_mean=y_sum/(double)n;
			x_var=xx_sum/(double)n-x_mean*x_mean;
			y_var=yy_sum/(double)n-y_mean*y_mean;
			det=xx_sum/n-x_mean*x_mean;
			if(fabs(det)>1e-200) {
				x_a=(xy_sum/n-x_mean*y_mean)/det;
				x_b=(xx_sum/n*y_mean-
				     x_mean*xy_sum/n)/det;
			} else {
				x_a=NAN;
				x_b=NAN;
			}
		}
		if(n>1) {
			x_std=sqrt((xx_sum-x_sum*x_mean)/(double)(n-1));
			y_std=sqrt((yy_sum-y_sum*y_mean)/(double)(n-1));
		}
		if(number_of_ids>1)
			printf("%*d",sdsn,sdind[i]);

		if(min_x)
			printf(" [%7d]=(%14e,%14e)",
			       x_min.ind,x_min.x,x_min.y);
		if(max_x)
			printf(" [%7d]=(%14e,%14e)",
			       x_max.ind,x_max.x,x_max.y);
		if(sum_x)
			printf(" %14e",x_sum);
		if(sum_xx)
			printf(" %14e",xx_sum);
		if(mean_x)
			printf(" %14e",x_mean);
		if(var_x)
			printf(" %14e",x_var);
		if(rms_x)
			printf(" %14e",sqrt(x_var));
		if(std_x)
			printf(" %14e",x_std);

		if(min_y)
			printf(" [%7d]=(%14e,%14e)",
			       y_min.ind,y_min.x,y_min.y);
		if(max_y)
			printf(" [%7d]=(%14e,%14e)",
			       y_max.ind,y_max.x,y_max.y);
		if(sum_y)
			printf(" %14e",y_sum);
		if(sum_yy)
			printf(" %14e",yy_sum);
		if(mean_y)
			printf(" %14e",y_mean);
		if(var_y)
			printf(" %14e",y_var);
		if(rms_y)
			printf(" %14e",sqrt(y_var));
		if(std_y)
			printf(" %14e",y_std);

		if(sum_xy)
			printf(" %14e",xy_sum);

		if(ax)
			printf(" %14e",x_a);
		if(bx)
			printf(" %14e",x_b);

		if(nop)
			printf(" %7d",n);

		printf("\n");
	}
	return 0;
}

void match_sdind(int *id,int nid,SharedData *sd)
{
	int i;
	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED&&!sd->hasid(id[i]))
			id[i]=DS_UNUSED;
}

int max_sdsn(int *id,int nid,int n, int *use)
{
	char *s;
	int i;

	for(i=0,*use=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		(*use)++;
		asprintf(&s,"%d",id[i]);
		n=MAX(n,(int)strlen(s));
		free(s);
	}

	return n;
}
