#include <SharedData.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

static struct option const long_options[] =
{
	{"help", no_argument, 0, 'h'},
	{"level", required_argument, 0, 'l'},
	{"mean",  required_argument, 0, 'm'},
	{"rms",   required_argument, 0, 'r'},
	{"minx",  required_argument, 0, 'i'},
	{"maxx",  required_argument, 0, 'a'},
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
	      "usage: %s [options] id(s)\n"
		"\n"
		"-l,    --level \n"
		"-i,    --minx\n"
		"-a,    --maxx\n"
		"-r,    --rms        if not given rms will be calculated"
		" from the data\n"
		"-m,    --mean       if not given mean will be calculated" 
		" from the data\n"
		"-h,-?, --help       this message\n"
		"\n"
		"Moves data points (or hole peaks) which deviates more than\n"
		"level*rms from mean so as to remove glitches.\n"
		"USE this program only on ORDERED datasets!\n",
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

int deglitch(SharedData *sd, int id, double *weight, double level,
	     double minx, double maxx, double rms, double mean)
{
	double *x=sd->x(id);
	double *y=sd->y(id);
	double m,mm,cutoff,var;
	int i,mini,maxi, j,degl=0;
	int n=sd->n(id);
	int min,max,be,en,inc;
	double rx,ry,dy_dx;

	*weight=0.0;

	if(x[0]>x[n-1]){
		be=n-1;
		en=0;
		inc=-1;
		if(isnan(minx)) {
			minx=x[n-1];
		}
		if(isnan(maxx)) {
			maxx=x[0];
		}
		//check integrity
		for(i=0;i<n-1;i++) {
			if(x[i]<x[i+1])
				return 0;
		}
	} else {
		en=n-1;
		be=0;
		inc=1;
		if(isnan(minx)) {
			minx=x[0];
		}
		if(isnan(maxx)) {
			maxx=x[n-1];
		}
		//check integrity
		for(i=0;i<n-1;i++) {
			if(x[i]>x[i+1])
				return 0;
		}
	}
	
	for(i=be;i!=en;i+=inc) {
		if(x[i]>minx)
			break;
	}
	min=i;
	for(;i!=en;i+=inc) {
		if(x[i]>maxx)
			break;
	}
	max=i;
		
	if(min>max){
		i=max;
		max=min;
		min=i;
	}
	
	if((max-min)<3)
		return 0;
	
	if(isnan(rms)||isnan(mean)) {
		// calculate rms and mean:
		for(i=min,m=mm=0.0;i<max;i++) {
			m+=y[i];
			mm+=y[i]*y[i];
		}
		if(isnan(mean))
			mean=m/(double)n;
		if(isnan(rms)) {
			rms=sqrt(mm/(double)n-mean*mean);
		}
	}
	var=rms*rms;
	cutoff=var*level*level;

	for(i=min;i<max;i++) {
		while(pow(y[i]-mean,2.0)<cutoff&&i<max)
			i++;
		if(i<max) {
			while(pow(y[i]-mean,2)>var&&i>0)
				i--;
			mini=i++;
			while(pow(y[i]-mean,2)>var&&i<n)
				i++;
			maxi=i;
			if(!isfinite(dy_dx=((ry=y[maxi])-y[mini])/
				 ((rx=x[maxi])-x[mini])))
				dy_dx=0;
			for(j=mini;j<maxi;j++) {
				y[j]=ry-(rx-x[j])*dy_dx;
				degl++;
			}
			*weight+=fabs(x[maxi]-x[mini]);
		}
	}
		   
	*weight=fabs(x[n]-x[0])/(fabs(x[n]-x[0])-*weight);
	
	return degl;
}

int main(int argc,char **argv)
{
	SharedData hallo;
	int i,c;
	double level=4.0;
	int changed=0;
	int id;
	int result;
	char *tmp;
	double min=NAN,max=NAN,rms=NAN,mean=NAN;
	double weight;
	
	while ((c = getopt_long(argc, argv, "l:i:a:m:r:h?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'l':
			level=strtod(optarg,&tmp);
			if(*tmp!=0)
				usage();
			break;
		case 'm':
			mean=strtod(optarg,&tmp);
			if(*tmp!=0)
				usage();
			break;
		case 'r':
			rms=strtod(optarg,&tmp);
			if(*tmp!=0)
				usage();
			break;
		case 'i':
			min=strtod(optarg,&tmp);
			if(*tmp!=0)
				usage();
			break;
		case 'a':
			max=strtod(optarg,&tmp);
			if(*tmp!=0)
				usage();
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


	if (level<1.0) {
	  fprintf(stderr,"paramter level has to be greater than 1.0! \n");
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

		result=deglitch(&hallo,id,&weight,level,min,max,rms,mean);
		if (result) {
			off+=asprintcat(&logline,&ll,off," -l %.2g",level);
			if(!isnan(min))
				off+=asprintcat(&logline,&ll,off,
						" -i %g",min);
			if(!isnan(max))
				off+=asprintcat(&logline,&ll,off,
						" -a %g",max);
			if(!isnan(rms))
				off+=asprintcat(&logline,&ll,off,
						" -r %g",rms);
			if(!isnan(mean))
				off+=asprintcat(&logline,&ll,off,
						" -m %g",mean);
			off+=asprintcat(&logline,&ll,off," $%d",id);
			off+=asprintcat(&logline,&ll,off,"\n# weight= %g",
					weight);
			hallo.log(logline);
			free(logline);
			changed=1;
		}
		else
			fprintf(stderr,"Nop on id %d (%s)\n",id,argv[i]);
	}
	if(changed)
		hallo.changed();
	return 0;
}

