#include <stdio.h>
#include <SharedData.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static struct option const long_options[] = {
	{"input",       required_argument, 0, 'i'},
	{"points",      required_argument, 0, 'P'},
	{"exec",	required_argument, 0, 'e'},
	{"plotlevel",   required_argument, 0, 'p'},
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
	      "usage: %s [-i file] [-p level]\n"
		"\n"
		"-i,    --input       input from a file, not from stdin\n"
		"-P,    --points      pairs of values\n"
		"-e,    --exec        execute comand, using its output to get"
		" data\n"
		"-p,    --plotlevel   put dataset on plotlevel\n"
		"-h,-?, --help        this message\n"
		"-V,    --version     display version information and exit.\n"
		"-W,    --warranty    display licensing terms.\n"
		"-C,    --copyright  \n"
		"\nreads pairs of numbers from stdin, a file or string,\n"
		" and puts them in a dataset.\n",
		program_invocation_short_name);
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


int main(int argc,char **argv)
{
	SharedData *hallo;
	char *command=NULL;
	char *line,*tp;
	double x,y;
	FILE *input=stdin;
	int pl=0,len;
	int id;
	char *points=NULL;
	
	int c;
	int i=0,size=0,stop=0;
	unsigned n=50;
	line=(char *)malloc(sizeof(char)*n);

	char *logline=NULL;
	int ll=0,off;

	while (!stop&&(c = getopt_long(argc, argv, "e:i:p:hP:?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'i':
			if(!(input=fopen(optarg,"r")))
				usage();
			break;
		case 'P':
			if(command) {
				fprintf(stderr," -P and -e must not used at"
					" the same time!\n");
				usage();
			}
			points=strdup(optarg);
			break;
		case 'e':
			if(points) {
				fprintf(stderr," -P and -e must not used at"
					" the same time!\n");
				usage();
			}
			command=strdup(optarg);
			stop=1;
			break;
		case 'p':
			if(sscanf(optarg,"%d",&pl)!=1)
				usage();
			break;
		case 'V':
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

	if(command) {
		for(i=optind;i<argc;i++) {
			len=strlen(command)+strlen(argv[i])+2;
			command=(char*)xrealloc(command,sizeof(char)*len);
			strcat(command," ");	
			strcat(command,argv[i]);
		}
		errno=0;
		input=popen(command,"r");
		if(!input) {
			fprintf(stderr,"%s: error executeing \"%s\": %s\n",
				program_invocation_short_name,
				command, strerror(errno));
			exit(EXIT_FAILURE);
		}
	} else if(optind>argc)
		usage();

	i=0;	

	if(points) {
		tp=line=points;
		while(*line) {
			if(isspace(*line))
				*line=' ';
			line++;
		}
		x=strtod(points,&line);
		points=line;
		y=strtod(points,&line);
		if(points==line)
			usage();
		hallo=new SharedData();
		id=hallo->create(10000);
		size=10000;
		hallo->add_ident(id,"dsg");
		hallo->set_plev(id,pl);
		printf("new id=%d\n",id);
		while(1) {
			if(i==size) {
				size+=10000;
				hallo->resize(id,size);
			}
			points=line;
			hallo->x(id)[i]=x;
			hallo->y(id)[i++]=y;
			x=strtod(points,&line);
			if(points==line) {
				hallo->set_n(id,i);
				hallo->changed();
				off=asprintcat(&logline,&ll,0,"<*%d*> %s", id,
					       program_invocation_short_name);
				off+=asprintcat(&logline,&ll,off," -P \"%s\"",
						tp);
				hallo->log(logline);
				delete hallo;
				return 0;
			}
			points=line;
			y=strtod(points,&line);
			if(points==line) {
				hallo->remove(id);
				delete hallo;
				usage();
			}
		}		
        }

	while(!(getdelim(&line,&n,'\n',input)<0)) { /* catch EoF */
		if(sscanf(line,"%lf%lf",&x,&y)!=2)
			continue;
		if(i==0) {
			hallo=new SharedData();
			id=hallo->create(10000);
			size=10000;
			hallo->add_ident(id,"dsg");
			hallo->set_plev(id,pl);
			printf("new id=%d\n",id);
			if(command) {
				off=asprintcat(&logline,&ll,0,"<*%d*> %s", id,
					       program_invocation_short_name);
				off+=asprintcat(&logline,&ll,off," -e \"%s\"",
						command);
				hallo->log(logline);
				hallo->add_ident(id,command);
			}
			delete hallo;
		}
		if(i==size) {
			hallo=new SharedData();
			size+=10000;
			hallo->resize(id,size);
			hallo->x(id)[i]=x;
			hallo->y(id)[i++]=y;
			hallo->set_n(id,i);
			hallo->changed();

			delete hallo;
		}
		else {			
			hallo=new SharedData();
			hallo->x(id)[i]=x;
			hallo->y(id)[i++]=y;
			hallo->set_n(id,i);
			hallo->changed();
			delete hallo;
		}
	}
	return 0;
}
