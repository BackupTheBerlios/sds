#include <stdio.h>
#include <SharedData.h>
#include <color.h>
#include <linestyle.h>
#include <symbolstyle.h>
#include <errno.h>
#include <limits.h>
#include <getopt.h>

#define DSLS_OPTION "DSLS_OPTION"

static struct option const long_options[] = {
	{"long",        no_argument,       0, 'l'},
	{"NoP",         no_argument,       0, 'n'},
	{"plevel",      no_argument,       0, 'p'},
	{"linestyle",   no_argument,       0, 'b'},
	{"linewidth",   no_argument,       0, 'u'},
	{"linecolor",   no_argument,       0, 'c'},
	{"symbolstyle", no_argument,       0, 'k'},
	{"symbolsize",  no_argument,       0, 'e'},
	{"symbolcolor", no_argument,       0, 't'},
	{"ident"      , no_argument,       0, 'i'},
	{"Ident"      , required_argument, 0, 'I'},
	{"help",        no_argument,       0, 'h'},
	{"version",     no_argument,       0, 'V'},
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "0.4";

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
		"usage: %s [options] [id(s)]\n"
		"\n"
		"-l,    --long        specify output fields in the\n"
		"                     enviroment variable %s\n"
		"                     using a string of short options\n"
		"\n"
		"    the output fields are ...\n"
//		"-f,    --flags       flags\n"
		"-n,    --NoP         number of points\n"
//		"-v,    --version     version\n"
//		"-o,    --source      source\n"
		"-p,    --plevel      plot level\n"
		"-b,    --linestyle   line style\n"
		"-u     --linewidth   line width\n"
		"-c     --linecolor   line color\n"
		"-k     --symbolstyle symbol style\n"
		"-e     --symbolsize  symbol size\n"
		"-t     --symbolcolor symbol color\n"
		"-i     --ident       the ident strings\n"
		"-I     --Ident       #n; all ident[l<n] strings\n"
		"\n"
		"    the other otpions are ...\n"
		"-h,-?, --help        this message\n"
		"-V,    --Version     display version information and exit.\n"
		"-W,    --warranty    display licensing terms.\n"
		"-C,    --copyright  \n",
		program_invocation_short_name,DSLS_OPTION);

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

void fatal(const char *templ, ...);

void match_sdind(int *id,int nid,SharedData *sd);

int max_sdsn(int *id,int nid,int n);
int max_nofp(int *id,int nid,int n,SharedData *sd);
int max_plev(int *id,int nid,int n,SharedData *sd);
int max_lsty(int *id,int nid,int n,SharedData *sd);
int max_lwid(int *id,int nid,int n,SharedData *sd);
int max_lcol(int *id,int nid,int n,SharedData *sd);
int max_ssty(int *id,int nid,int n,SharedData *sd);
int max_ssiz(int *id,int nid,int n,SharedData *sd);
int max_scol(int *id,int nid,int n,SharedData *sd);
int max_iden(int *id,int nid,int n,int depth,SharedData *sd);

int main(int argc, char *argv[])
{
	SharedData sd;
	int *sdind,indsize;
	char **cpp;
	
	char *dsls_option;
	int j,k,l,c;
	int sdsn;
	int nofp=0;
	int plev=0;
	int lsty=0;
	int lwid=0;
	int lcol=0;
	int ssty=0;
	int ssiz=0;
	int scol=0;
	int iden=0;
	int lo=0;
	int ide;
	char *tail;
	
	while ((c = getopt_long(argc, argv, "nplbucketiI:h?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'n':
			nofp=1;
			break;
		case 'p':
			plev=1;
			break;
		case 'l':
			lo=1;
			break;
		case 'b':
			lsty=1;
			break;
		case 'u':
			lwid=1;
			break;
		case 'c':
			lcol=1;
			break;
		case 'k':
			ssty=1;
			break;
		case 'e':
			ssiz=1;
			break;
		case 't':
			scol=1;
			break;
		case 'i':
			if(!iden)
				iden=INT_MAX;
			break;
		case 'I':
			iden=strtol(optarg,NULL,0);
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

	if(lo) {
		char *tail;
		if((dsls_option=getenv(DSLS_OPTION))) {
			while(*dsls_option) {
				switch(*dsls_option) {
				case 'n':
					nofp=1;
					break;
				case 'p':
					plev=1;
					break;
				case 'b':
					lsty=1;
					break;
				case 'u':
					lwid=1;
					break;
				case 'c':
					lcol=1;
					break;
				case 'k':
					ssty=1;
					break;
				case 'e':
					ssiz=1;
					break;
				case 't':
					scol=1;
					break;
				case 'i':
					if(!iden)
						iden=1;
					break;
				case 'I':
					errno=0;
					iden=strtol(dsls_option+1,&tail,0);
					if(errno) {
						fprintf(stderr,"error in"
							" parsing %s=%s"
							" for option \"I\"\n",
							DSLS_OPTION,
							getenv(DSLS_OPTION));
						usage();
					}
					if(dsls_option+1==tail)
						iden=INT_MAX;
					break;
				default:
					fprintf(stderr,"wrong field"
						" specification in %s=%s",
						DSLS_OPTION,
						getenv(DSLS_OPTION));
					usage();
				}
				dsls_option++;
			}
		} else {
			fprintf(stderr,"No enviroment variable %s!\n",
				DSLS_OPTION);
			usage();
		}
	}

	if(optind==argc) {
		sdind=sd.record_state(&indsize);
		sort_pl_id(&sd,&sdind,indsize);
	} else {
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
 
	if(!(nofp||plev||lsty||lwid||lcol||ssty||ssiz||scol||iden))
		iden=INT_MAX;

	sdsn=max_sdsn(sdind,indsize,strlen("id"));

	if(nofp)
		nofp=max_nofp(sdind,indsize,strlen("NoP"),&sd);

	if(plev)
		plev=max_plev(sdind,indsize,strlen("PlL"),&sd);

	if(lsty)
		lsty=max_lsty(sdind,indsize,strlen("lstyle"),&sd);

	if(lwid)
		lwid=max_lwid(sdind,indsize,strlen("lw"),&sd);

	if(lcol)
		lcol=max_lcol(sdind,indsize,strlen("lcolor"),&sd);

	if(ssty)
		ssty=max_ssty(sdind,indsize,strlen("sstyle"),&sd);

	if(ssiz)
		ssiz=max_ssiz(sdind,indsize,strlen("SS"),&sd);

	if(scol)
		scol=max_scol(sdind,indsize,strlen("scolor"),&sd);

	if(iden)
		ide=max_iden(sdind,indsize,strlen("ident"),iden,&sd);

	printf("%*s",sdsn,"id");
	if(nofp)
		printf(" %*s",nofp,"NoP");
	if(plev)
		printf(" %*s",plev,"PlL");
	if(lsty)
		printf(" %-*s",lsty,"lstyle");
	if(lwid)
		printf(" %*s",lwid,"LW");
	if(lcol)
		printf(" %-*s",lcol,"lcolor");
	if(ssty)
		printf(" %-*s",ssty,"sstyle");
	if(ssiz)
		printf(" %*s",ssiz,"SS");
	if(scol)
		printf(" %-*s",scol,"scolor");
	if(iden)
		// printf(" %-*s",ide,"ident");
		printf(" %-s","ident");
	printf("\n");

	for(j=0;j<indsize;j++) {
		if(sdind[j]==DS_UNUSED)
			continue;
		printf("%*d",sdsn,sdind[j]);
		
		if(nofp)
			printf(" %*d",nofp,sd.n(sdind[j]));
		if(plev)
			printf(" %*d",plev,sd.plev(sdind[j]));
		if(lsty)
			printf(" %-*s",lsty,
			       linestyle[sd.linestyle(sdind[j])]);
		if(lwid)
			printf(" %*d",lwid,sd.linewidth(sdind[j]));
		if(lcol)
			printf(" %-*s",lcol,
			       color_name(sd.linecolor(sdind[j])));
		if(ssty)
			printf(" %-*s",ssty,
			       symbolstyle[sd.symbolstyle(sdind[j])]);
		if(ssiz)
			printf(" %*d",ssiz,sd.symbolsize(sdind[j]));
		if(scol)
			printf(" %-*s",scol,
			       color_name(sd.symbolcolor(sdind[j])));
		if(iden) {
			if((cpp=sd.ident(sdind[j],&k))) {
				if(cpp[0])
					// printf(" %-*s",ide,cpp[0]);
					printf(" %s",cpp[0]);
				if(iden>1) {
					for(k=1,l=1;cpp[k]&&l<iden;k++,l++) {
						printf("\n%-*s",sdsn, "");
						if(nofp)
							printf(" %-*s", 
							       nofp, "");
						if(plev)
							printf(" %*s",
							       plev, "");
						if(lsty)
							printf(" %-*s",
							       lsty, "");
						if(lwid)
							printf(" %*s",
							       lwid, "");
						if(lcol)
							printf(" %-*s",
							       lcol, "");
						if(ssty)
							printf(" %-*s",
							       ssty, "");
						if(ssiz)
							printf(" %*s",
							       ssiz, "");
						if(scol)
							printf(" %-*s",
							       scol, "");
						// printf(" %-*s",ide, cpp[k]);
						printf(" %s", cpp[k]);
					}
				}
			}
		}
		printf("\n");
	}
	return 0;
}


int max_sdsn(int *id,int nid,int n)
{
	char *s;
	int i;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		asprintf(&s,"%d",id[i]);
		n=MAX(n,(int)strlen(s));
		free(s);
	}

	return n;
}

int max_nofp(int *id,int nid,int n,SharedData *sd)
{
	char *s;
	int i;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		asprintf(&s,"%d",sd->n(id[i]));
		n=MAX(n,(int)strlen(s));
		free(s);
	}

	return n;
}

int max_plev(int *id,int nid,int n,SharedData *sd)
{
	char *s;
	int i;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		asprintf(&s,"%d",sd->plev(id[i]));
		n=MAX(n,(int)strlen(s));
		free(s);
	}

	return n;
}

int max_lsty(int *id,int nid,int n,SharedData *sd)
{
	int i;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		n=MAX(n,(int)strlen(linestyle[sd->linestyle(id[i])]));
	}
	return n;
}

int max_lwid(int *id,int nid,int n,SharedData *sd)
{
	char *s;
	int i;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		asprintf(&s,"%d",sd->linewidth(id[i]));
		n=MAX(n,(int)strlen(s));
		free(s);
	}

	return n;
}

int max_lcol(int *id,int nid,int n,SharedData *sd)
{
	int i;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		n=MAX(n,(int)strlen(color_name(sd->linecolor(id[i]))));
	}
	return n;
}

int max_ssty(int *id,int nid,int n,SharedData *sd)
{
	int i;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		n=MAX(n,(int)strlen(symbolstyle[sd->symbolstyle(id[i])]));
	}
	return n;
}

int max_ssiz(int *id,int nid,int n,SharedData *sd)
{
	char *s;
	int i;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		asprintf(&s,"%d",sd->symbolsize(id[i]));
		n=MAX(n,(int)strlen(s));
		free(s);
	}

	return n;
}

int max_scol(int *id,int nid,int n,SharedData *sd)
{
	int i;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		n=MAX(n,(int)strlen(color_name(sd->symbolcolor(id[i]))));
	}
	return n;
}

int max_iden(int *id,int nid,int n,int depth,SharedData *sd)
{
	char **s;
	int i,j,k,l;

	for(i=0;i<nid;i++) {
		if(id[i]==DS_UNUSED)
			continue;
		if((s=sd->ident(id[i],&j)))
			for(k=0,l=0;s[k]&&l<depth;k++)
				n=MAX(n,(int)strlen(s[k]));
	}
	return n;
}

void match_sdind(int *id,int nid,SharedData *sd)
{
	int i;
	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED&&!sd->hasid(id[i]))
			id[i]=DS_UNUSED;
}
