#include <fstream.h>
#include <stdio.h>
#include <SharedData.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "dsn.h"

static struct option const long_options[] =
{
	{"help", no_argument, 0, 'h'},
	{"format", required_argument, 0, 'f'},
	{"verbose", no_argument, 0, 'v'},
	{"version", no_argument, 0, 'V'},
	{"warranty", no_argument, 0, 'W'},
	{"copyright", no_argument, 0, 'C'},
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
void usage(void)
{
	fprintf(stderr,
	      "usage: %s [-f str] ids\n"
		"\n"
		"-f,    --format     format string, default ist \"%%n\"\n"
		"-v,    --verbose    verbose info\n"
		"-h,-?, --help       this message\n"
		"-V,    --version    display version information and exit.\n"
		"-W,    --warranty   display licensing terms.\n"
		"-C,    --copyright  \n\n"
		"The second id-string (ids[1]) of the given ids is analyzed.\n"
		"The following macros may be used in the format string:\n"
		"  %%p    ->  pathname\n"
		"  %%n    ->  filename\n"
		"  %%v    ->  dsh-version\n"
		"  %%i    ->  dsh-subid\n"
		"  %%x    ->  src-x-column\n"
		"  %%y    ->  src-y-column\n"
		"  %%b,%%e ->  basename and extension, if filename looks like "
		"%%b.%%e\n"
		"  %%%%    ->  %%\n",
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
	int dsn_yyparse(void);
	struct yy_buffer_state *dsn_yy_scan_string(const char *);
	void dsn_yy_delete_buffer(yy_buffer_state *);
	void dsn_yy_flush_buffer(yy_buffer_state *);
	yy_buffer_state *yybuf;
	extern FILE *dsn_yyin;
	extern int dsn_yydebug;
	extern struct DsnParam yyparam;
}


int main(int argc,char *argv[])
{
	SharedData hallo;
	int i,j;

	char *format=NULL;
	int verbose=0;
	char **ident;
	char *idstr;
	int identn;

	char *tmp,*tmpp;
	char c;
	int id;


// 	dsn_yydebug=1;

	while ((c = getopt_long(argc, argv, "f:ih?vVWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'f':
			format=strdup(optarg);
			break;
		case 'v':
			verbose=1;
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

	if(!format)
		format="%n";

	for(i=optind;i<argc;i++) {
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

		ident=hallo.ident(id,&identn);
		if (identn<2) 
			continue;
		idstr=ident[1];

		if (verbose)
			printf("\nparsing \"%s\"..\n",idstr);

		yyparam.path=yyparam.name=yyparam.ver=NULL;
		yyparam.ind=yyparam.x=yyparam.y=0;
		yybuf=dsn_yy_scan_string(idstr);
		dsn_yyparse();

		if (verbose) {
			printf("%%p=\"%s\"\n",yyparam.path);
			printf("%%n=\"%s\"\n",yyparam.name);
			printf("%%v=\"%s\"\n",yyparam.ver);
			printf("%%i=%d\n",yyparam.ind);
			printf("%%x=%d\n",yyparam.x);
			printf("%%y=%d\n",yyparam.y);
			printf("format=\"%s\"\n\n=>",format);
		}

		j=0;
		c=format[0];
		while (c) {
			if (c=='%') {
				j++;
				c=format[j];
				switch (c) {
				case 'p':
					if (yyparam.path) 
						printf("%s",yyparam.path);
					break;
				case 'n':
					if (yyparam.name)
						printf("%s",yyparam.name);
					break;
				case 'v':
					if (yyparam.ver)
						printf("%s",yyparam.ver);
					break;
				case 'i':
					if (yyparam.ind)
						printf("%d",yyparam.ind);
					break;
				case 'x':
					if (yyparam.x)
						printf("%d",yyparam.x);
					break;
				case 'y':
					if (yyparam.y)
						printf("%d",yyparam.y);
					break;
				case '%':
					printf("%%");
					break;
				case 'b':
					tmp=strdup(yyparam.name);
					tmpp=strchr(tmp,'.');
					if (tmpp) {
						*tmpp=0;
						if (tmp)
							printf("%s",tmp);
					}
					break;
				case 'e':
					tmp=strdup(yyparam.name);
					tmpp=strchr(tmp,'.');
					if (tmpp) {
						tmpp++;
						if (tmpp)
							printf("%s",tmpp);
					}
					break;
				default:
					break;
				}
			} else {
				printf("%c",c);
			}
			j++;
			c=format[j];
		}
		printf(" ");
		dsn_yy_delete_buffer(yybuf);
		free(yyparam.path);
		free(yyparam.name);
		free(yyparam.ver);
	}
	printf("\n");
	return 0;
}


