#include <color.h>
#include <linestyle.h>
#include <symbolstyle.h>
#include <SharedData.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

static struct option const long_options[] = {
	{"save",        no_argument,       0, 's'},
	{"dump",        no_argument,       0, 'd'},
        {"prec",        required_argument, 0, 'p'},
	{"output",      required_argument, 0, 'o'},
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
	      "usage: %s [opts] id(s)\n"
		"\n"
		"-s,    --save        use an ascii format\n"
		"-d,    --dump        use a binary format\n"
                "-p,    --prec        precision in ascii output\n"
                "                     (0: use %%a in format string)\n"
		"-o,    --output      write to file\n"
		"-h,-?, --help        this message\n"
		"-V,    --version     display version information and exit.\n"
		"-W,    --warranty    display licensing terms.\n"
		"-C,    --copyright  \n"
		"\n"
		" %s dumps or saves the data in a shared data file\n"
		" using a binary or ascii format respectively.\n",
		program_invocation_short_name,
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
	SharedData sd;
	int i,j,k,c;
	int save=0,dump=0;
	char *on=NULL;
	double *x;
	double *y;
	int max;
	char **ident;
	int prec=6;

	int id;
	FILE *out;
	char *formstr;

	if(strcmp(argv[0],"dsd")==0)
		dump=1;
	if(strcmp(argv[0],"dss")==0)
		save=1;


	while ((c = getopt_long(argc, argv, "o:p:sdh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 's':
			if(dump)
				fprintf(stderr,"switching to save mode\n");
			dump=0;
			save=1;
			break;
		case 'd':
			if(save)
				fprintf(stderr,"switching to dump mode\n");
			save=0;
			dump=1;
			break;
		case 'o':
			on=optarg;
			break;
		case 'p':
			prec=strtol(optarg,NULL,0);
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

	if(on) {
		out=fopen(on,"w");
		if(!out) {
			fprintf(stderr,"output opening error on \"%s\"\n",
				on);
			usage();
		}
	} else
		out=stdout;
	
	if(!prec) {
		asprintf(&formstr,"%%a %%a\n");
	} else {
		asprintf(&formstr,"%%.%dg %%.%dg\n",prec,prec);
	}

	if(save==dump)
		fatal("Please use either the --save or the --dump option\n"
		      "or name this programm dss (for saving)"
		      " or dsd (for dumping).\n");


	for(i=optind;i<argc;i++) {
		errno=0;
		id=strtol(argv[i],NULL,0);
		if (errno) {
			fprintf (stderr,"ID Overflow in %s\n",argv[i]);
			continue;
		}
		if(!sd.hasid(id)) {
			fprintf(stderr,"No id %d (%s)\n",id,argv[i]);
			continue;
		}
		
		x=sd.x(id);
		y=sd.y(id);
		max=sd.n(id);
		ident=sd.ident(id,&k);
		if(dump)
			fprintf(out,"/*\n *\tbinary data of id %d:\n",id);
		else
			fprintf(out,"/*\n *\tdata of id %d:\n",id);

		fprintf(out," *\n *\tsize=%d\n *\n",max);
		for(j=0;j<k;j++)
			fprintf(out," *\tident[%d]=\"%s\"\n",j,ident[j]);

		fprintf(out," *\n *\tlinestyle=%s\n",
			linestyle[sd.linestyle(id)]);
		fprintf(out," *\tlinecolor=%s\n",color_name(sd.linecolor(id)));
		fprintf(out," *\tlinewidth=%d\n",sd.linewidth(id));
		fprintf(out," *\n *\tsymbolstyle=%s\n",
			symbolstyle[sd.symbolstyle(id)]);
		fprintf(out," *\tsymbolcolor=%s\n",
			color_name(sd.symbolcolor(id)));
		fprintf(out," *\tsymbolsize=%d\n",sd.symbolsize(id));
		fprintf(out," *\n *\tplotlevel=%d\n",sd.plev(id));

		fprintf(out," */\n");
		if(dump) {
			fprintf(out,"%c",'\032');
			fwrite(x,sizeof(double),max,out);
			fwrite(y,sizeof(double),max,out);
			fprintf(out,"\n");
		} else {
			for(j=0;j<max;j++) 
				fprintf(out,formstr,x[j],y[j]);
		}
	}
	return 0;
}
