#include <stdio.h>
#include <SharedData.h>
#include <getopt.h>
#include <string.h>
#include <color.h>
#include <linestyle.h>
#include <symbolstyle.h>
#include <errno.h>
#include <stdarg.h>

extern char *program_invocation_short_name;

char *rainbowcolor[] = {
	"Blue",
	"Red",
	"Green",
	"RawSienna",
	"Goldenrod",
	"Bittersweet",
	"Magenta",
	"Orchid",
	"PineGreen",
	"Tan",
	0
};

static struct option const long_options[] = {
	{"plev",        required_argument, 0, 'p'},
	{"linestyle",   required_argument, 0, 'b'},
	{"linewidth",   required_argument, 0, 'u'},
	{"linecolor",   required_argument, 0, 'c'},
	{"symbolstyle", required_argument, 0, 'k'},
	{"symbolsize",  required_argument, 0, 'e'},
	{"symbolcolor", required_argument, 0, 't'},
	{"ident",       required_argument, 0, 'i'},
	{"add_ident",   required_argument, 0, 'a'},
	{"rainbow",     required_argument, 0, 'r'},
	{"quiet",       no_argument,       0, 'q'},
	{"log",         no_argument,       0, 'l'},
	{"help",        no_argument,       0, 'h'},
	{"version",     no_argument,       0, 'V'},
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "0.4";

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
	int i,j,k;
	fprintf(stderr,
	      "usage: %s [options] ids\n"
		"\n"
		"-p,    --plev        plot level\n"
		"-b,    --linestyle   name of line style\n"
		"-u,    --linewidth   \n"
		"-c,    --linecolor   color name\n"
		"-k,    --symbolstyle name of symbol style\n"
		"-e,    --symbolsize  \n"
		"-t,    --symbolcolor color name\n"
		"-i,    --ident       set ident string\n"
		"-a     --add_ident   add ident string\n"
		"-r,    --rainbow     use in conjunction with option -p\n"
		"-l,    --log         create logentry\n"
		"-q,    --quiet       do not create logentry\n"
		"-h,-?, --help        this message\n"
		"-V,    --version     display version information and exit.\n"
		"-W,    --warranty    display licensing terms.\n"
		"-C,    --copyright  \n\n",
		program_invocation_short_name);
	fprintf(stderr,"colorname is one of the following:\n  ");
	k=maxcolor();
	for(i=0,j=2;i<k;j+=strlen(colors[i].name)+1,i++) {
		if(j+strlen(colors[i].name)>79) {
			fprintf(stderr,"\n  ");
			j=2;
		}
		fprintf(stderr,"%s ",colors[i].name);
	}
	fprintf(stderr,"\nlinestyle is one of the following:\n  ");
	k=maxlinestyle();
	for(i=0,j=2;i<k;j+=strlen(linestyle[i])+1,i++) {
		if(j+strlen(linestyle[i])>79) {
			fprintf(stderr,"\n  ");
			j=2;
		}
		fprintf(stderr,"%s ",linestyle[i]);
	}
	fprintf(stderr,"\nsymbolstyle is one of the following:\n  ");
	k=maxsymbolstyle();
	for(i=0,j=2;i<k;j+=strlen(symbolstyle[i])+1,i++) {
		if(j+strlen(symbolstyle[i])>79) {
			fprintf(stderr,"\n  ");
			j=2;
		}
		fprintf(stderr,"%s ",symbolstyle[i]);
	}
	fprintf(stderr,"\n");
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

int asprintcat(char **dest, int *dl, int off, const char *fstr, ...);
void fatal(const char *templ, ...);

int main(int argc,char *argv[])
{
	SharedData hallo;
	int id;

	char *lc=NULL,*sc=NULL,*ls=NULL,*sst=NULL;
	int colori=0,ssi=0,r=0;

	char **idents=NULL;
	int identn=1;
	int newident=0;

	int pl=-2;
	int lw=-1;

	int log=0;

	int isst,ils,c,i,max;
	int changed=0;

	while ((c = getopt_long(argc, argv, "p:b:u:c:k:e:t:i:a:rqlh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'p':
			if(sscanf(optarg,"%d",&pl)!=1)
				usage();
//			if(pl>99)
//				pl=99;
//			if(pl<-1)
//				pl=-1;
			break;
		case 'b':
			ls=optarg;
			break;
		case 'u':
			if(sscanf(optarg,"%d",&lw)!=1)
				usage();
			if(lw<0)
				lw=0;
			if(lw>9)
				lw=9;
			break;
		case 'c':
			lc=optarg;
			break;
		case 'k':
			sst=optarg;
			break;
		case 'e':
			if(sscanf(optarg,"%d",&ssi)!=1)
				usage();
			if(ssi<1)
				ssi=1;
			if(ssi>10)
				ssi=10;
			break;
		case 't':
			sc=optarg;
			break;
		case 'i':
			newident=1;
			identn=2;
			idents=(char **)realloc(idents,2*sizeof(char *));
			idents[0]=strdup(optarg);
			idents[1]=NULL;
			break;
		case 'a':
			identn++;
			idents=(char **)realloc(idents,identn*sizeof(char *));
			idents[identn-2]=strdup(optarg);
			idents[identn-1]=NULL;
			break;
		case 'r':
			r=1;
			break;
		case 'q':
			log=0;
			break;
		case 'l':
			log=1;
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

	if(sst) {
		max=maxsymbolstyle();
		for(i=0;i<max;i++)
			if(!strcmp(sst,symbolstyle[i])) {
				isst=i;
				break;
			}
		if(i==max) {
			fprintf(stderr,"name of symbolstyle (%s) unknown!\n",
				sst);
			usage();
		}
	}

	if(ls) {
		max=maxlinestyle();
		for(i=0;i<max;i++)
			if(!strcmp(ls,linestyle[i])) {
				ils=i;
				break;
			}
		if(i==max) {
			fprintf(stderr,"name of linestyle (%s) unknown!\n",
				ls);
			usage();
		}
	}

	if(lc&&color_ind(lc)<0) {
		fprintf(stderr,"name of linecolor (%s) unknown!\n",
			lc);
		usage();
		
	}

	if(sc&&color_ind(sc)<0) {
		fprintf(stderr,"name of symbolcolor (%s) unknown!\n",
			sc);
		usage();
		
	}

	for(i=optind;i<argc;i++) {
		char *logline=NULL;
		int ll=0,off=0;
		int j;

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
		off=asprintcat(&logline,&ll,0,"<%d>\t%s -l",id,
			       program_invocation_short_name);
		if(pl>-2) {
			hallo.set_plev(id,pl);
			off+=asprintcat(&logline,&ll,off," -p %d",pl);
		}
		if(r) {
			if(!rainbowcolor[colori])
				colori=0;
			hallo.set_linecolor(id,
					    rainbowcolor[colori]);
			off+=asprintcat(&logline,&ll,off," -c %s",
					rainbowcolor[colori++]);
		}
		if (idents) {
			if (newident) {
				hallo.set_ident(id,(const char **)idents);
				off+=asprintcat(&logline,&ll,off," -i \"%s\"",
						idents[0]);
			} else 
				for (j=0;j<(identn-1);j++) {
					hallo.add_ident(id,idents[j]);
					off+=asprintcat(&logline,&ll,off,
							" -a \"%s\"",
							idents[j]);
				}
		}
		if(ls) {
			hallo.set_linestyle(id,ils);
			off+=asprintcat(&logline,&ll,off," -b %s",ls);
		}
		if(lw>-1) {
			hallo.set_linewidth(id,lw);
			off+=asprintcat(&logline,&ll,off," -u %d",lw);
		}
		if(lc) {
			hallo.set_linecolor(id,lc);
			off+=asprintcat(&logline,&ll,off," -c %s",lc);
		}
		if(sst) {
			hallo.set_symbolstyle(id,isst);
			off+=asprintcat(&logline,&ll,off," -k %s",sst);
		}
		if(ssi) {
			hallo.set_symbolsize(id,ssi);
			off+=asprintcat(&logline,&ll,off," -e %d",ssi);
		}
		if(sc) {
			hallo.set_symbolcolor(id,sc);
			off+=asprintcat(&logline,&ll,off," -t %s",sc);
		}
		off+=asprintcat(&logline,&ll,off," $%d",id);
		if (log) hallo.log(logline);
		free(logline);
		changed=1;
	}

	if (idents) {
		for (i=0;i<identn;i++) free(idents[i]);
		free(idents);
	}

	if(changed)
		hallo.changed();
        return 0;
}

