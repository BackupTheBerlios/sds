#include <stdio.h>
#include <SharedData.h>
#include <color.h>
#include <linestyle.h>
#include <symbolstyle.h>
#include <errno.h>
#include <limits.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

static struct option const long_options[] = {
	{"NoP",         required_argument, 0, 'n'},
	{"plevel",      required_argument, 0, 'p'},
	{"linestyle",   required_argument, 0, 'b'},
	{"linewidth",   required_argument, 0, 'u'},
	{"linecolor",   required_argument, 0, 'c'},
	{"symbolstyle", required_argument, 0, 'k'},
	{"symbolsize",  required_argument, 0, 'e'},
	{"symbolcolor", required_argument, 0, 't'},
	{"ident"      , required_argument, 0, 'i'},
	{"Ident"      , required_argument, 0, 'I'},
	{"reverse",     no_argument,       0, 'r'},
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
		"usage: %s [options] [range]\n"
		"\n"
		"    fields to match are ...\n"
		"-n,    --NoP         range\n"
		"-p,    --plevel      range\n"
		"-b,    --linestyle   regexp\n"
		"-u     --linewidth   range\n"
		"-c     --linecolor   regexp\n"
		"-k     --symbolstyle regexp\n"
		"-e     --symbolsize  range\n"
		"-t     --symbolcolor regexp\n"
		"-i     --ident       regexp (searches in ident[0])\n"
		"-I     --Ident       n (searches in strings"
		" of ident up to level #n\n"
		"                        or with no numerical argument in all"
		" levels.)\n"
		"-r     --reverse     reverse match\n"
		"\n"
		"\tA \"regexp\" searches in the coresponding text field.\n"
		"\tA \"range\" is a sting of the form: #n or #n-#m"
		" or #n- or -#m or\n" 
		"\ta comma seperated list of these items.\n"
		"\n"
		"    other otpions are ...\n"
		"-h,-?, --help        this message\n"
		"-V,    --Version     display version information and exit.\n"
		"-W,    --warranty    display licensing terms.\n"
		"-C,    --copyright  \n",
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

#define MAX(a,b) ((a) > (b) ? (a) : (b))

void fatal(const char *templ, ...);

void match_sdsn(int *id,int nid,const char *range);
void match_nofp(int *id,int nid,const char *range,SharedData *sd);
void match_plev(int *id,int nid,const char *range,SharedData *sd);
void match_lwid(int *id,int nid,const char *range,SharedData *sd);
void match_ssiz(int *id,int nid,const char *range,SharedData *sd);

void match_lsty(int *id,int nid, const char *rtm, SharedData *sd);
void match_lcol(int *id,int nid, const char *rtm, SharedData *sd);
void match_ssty(int *id,int nid, const char *rtm, SharedData *sd);
void match_scol(int *id,int nid, const char *rtm, SharedData *sd);
void match_iden(int *id,int nid, const char *rtm, int level, SharedData *sd);

int main(int argc, char *argv[])
{
	SharedData sd;
	int *sdind,indsize;
	idset indset;
	idset *rset, oset;

	int i,c;
	int nofp=0;
	int plev=0;
	int lsty=0;
	int lwid=0;
	int lcol=0;
	int ssty=0;
	int ssiz=0;
	int scol=0;
	int iden=0;
	int rev=0;
	const char *nofp_arg;
	const char *plev_arg;
	const char *lsty_arg;
	const char *lwid_arg;
	const char *lcol_arg;
	const char *ssty_arg;
	const char *ssiz_arg;
	const char *scol_arg;
	const char *iden_arg;
	
	int out=0;

	while ((c = getopt_long(argc, argv, "n:p:b:u:c:k:e:t:i:I:rh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'n':
			nofp=1;
			nofp_arg=optarg;
			break;
		case 'p':
			plev=1;
			plev_arg=optarg;
			break;
		case 'b':
			lsty=1;
			lsty_arg=optarg;
			break;
		case 'u':
			lwid=1;
			lwid_arg=optarg;
			break;
		case 'c':
			lcol=1;
			lcol_arg=optarg;
			break;
		case 'k':
			ssty=1;
			ssty_arg=optarg;
			break;
		case 'e':
			ssiz=1;
			ssiz_arg=optarg;
			break;
		case 't':
			scol=1;
			scol_arg=optarg;
			break;
		case 'i':
			if(!iden)
				iden=INT_MAX;
			iden_arg=optarg;
			break;
		case 'I':
			iden=strtol(optarg,NULL,0);
			break;
		case 'r':
			rev=1;
			oset.ids=sd.record_state(&(oset.n));
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

	sdind=sd.record_state(&indsize);
	for(i=optind;i<argc;i++)
		match_sdsn(sdind,indsize,argv[i]);

	if(nofp)
		match_nofp(sdind,indsize,nofp_arg,&sd);

	if(plev)
		match_plev(sdind,indsize,plev_arg,&sd);

	if(lsty)
		match_lsty(sdind,indsize,lsty_arg,&sd);

	if(lwid)
		match_lwid(sdind,indsize,lwid_arg,&sd);

	if(lcol)
		match_lcol(sdind,indsize,lcol_arg,&sd);

	if(ssty)
		match_ssty(sdind,indsize,ssty_arg,&sd);

	if(ssiz)
		match_ssiz(sdind,indsize,ssiz_arg,&sd);

	if(scol)
		match_scol(sdind,indsize,scol_arg,&sd);

	if(iden)
		match_iden(sdind,indsize,iden_arg,iden,&sd);

	indset.ids=sdind;
	indset.n=indsize;
	if (rev) { // reverse match
		rset=exclude_sets(&oset, &indset);
		indset.ids=rset->ids;
		indset.n=rset->n;
	}
	sort_set(&indset);
	for(i=0;i<indset.n;i++)
		if(indset.ids[i]!=DS_UNUSED) {
			if(out)
				printf(" ");
			printf("%d",indset.ids[i]);
			out=1;
		}
	if(out)
		printf("\n");
	else
		printf("0\n");
	return 0;
}

int isinrange(int i,const char *range) {
        char *tmp,*pd;
	int j,k;
	
	tmp=strtok((pd=strdup(range)),",");
	do {
		char *tail;
		j=-1;
		while(isspace(*tmp)) tmp++;
		if(!*tmp)
			fatal("wrong range specification (%s)\n",
			      range);
		if(*tmp=='-') {
			tail=tmp;
			j=0;
		} else {
			errno=0;
			j=strtol(tmp,&tail,0);
			if(errno||tmp==tail)
				fatal("wrong range specification (%s)\n",
				      range);
		}
		while(isspace(*tail)) tail++;
		if(*tail) {
			if(*tail!='-')
				fatal("wrong range specification (%s)\n",
				      range);
			else tail++;
			while(isspace(*tail)) tail++;
			if(!*tail)
				k=INT_MAX;
			else {
				errno=0;
				k=strtol(tail,&tmp,0);
				if(errno||tail==tmp||k<0)
					fatal("wrong range specification"
					      " (%s)\n", range);
			}
		} else 
			k=j;
		if(i>=j&&i<=k) {
			free(pd);
			return 1;
		}
	} while((tmp=strtok(NULL,",")));

	free(pd);
	return 0;
}

void match_sdsn(int *id,int nid,const char *range)
{
	int i;
	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED&&!isinrange(id[i],range))
			id[i]=DS_UNUSED;
}

void match_nofp(int *id,int nid,const char *range,SharedData *sd)
{
	int i;
	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED&&!isinrange(sd->n(id[i]),range))
			id[i]=DS_UNUSED;
}

void match_plev(int *id,int nid,const char *range,SharedData *sd)
{
	int i;
	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED&&!isinrange(sd->plev(id[i]),range))
			id[i]=DS_UNUSED;
}

void match_lwid(int *id,int nid,const char *range,SharedData *sd)
{
	int i;
	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED&&!isinrange(sd->linewidth(id[i]),range))
			id[i]=DS_UNUSED;
}

void match_ssiz(int *id,int nid,const char *range,SharedData *sd)
{
	int i;
	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED&&!isinrange(sd->symbolsize(id[i]),range))
			id[i]=DS_UNUSED;
}

void match_lsty(int *id,int nid, const char *rtm, SharedData *sd)
{
	regex_t regex;
	int i;

	regcomp(&regex,rtm,REG_EXTENDED|REG_NOSUB);

	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED &&
		   regexec(&regex,linestyle[sd->linestyle(id[i])],0,NULL,0))
			id[i]=DS_UNUSED;
	regfree(&regex);
}

void match_lcol(int *id,int nid, const char *rtm, SharedData *sd)
{
	regex_t regex;
	int i;

	regcomp(&regex,rtm,REG_EXTENDED|REG_NOSUB);

	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED &&
		   regexec(&regex,
			   color_name(sd->linecolor(id[i])),0,NULL,0))
			id[i]=DS_UNUSED;
	regfree(&regex);
}

void match_ssty(int *id,int nid, const char *rtm, SharedData *sd)
{
	regex_t regex;
	int i;

	regcomp(&regex,rtm,REG_EXTENDED|REG_NOSUB);

	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED &&
		   regexec(&regex,symbolstyle[sd->symbolstyle(id[i])]
			   ,0,NULL,0))
			id[i]=DS_UNUSED;
	regfree(&regex);
}

void match_scol(int *id,int nid, const char *rtm, SharedData *sd)
{
	regex_t regex;
	int i;

	regcomp(&regex,rtm,REG_EXTENDED|REG_NOSUB);

	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED &&
		   regexec(&regex,
			   color_name(sd->symbolcolor(id[i])),0,NULL,0))
			id[i]=DS_UNUSED;
	regfree(&regex);
}

void match_iden(int *id,int nid, const char *rtm, int level, SharedData *sd)
{
	regex_t regex;
	int i,j,k,l,match;
	char **cpp;

	regcomp(&regex,rtm,REG_EXTENDED|REG_NOSUB);

	for(i=0;i<nid;i++)
		if(id[i]!=DS_UNUSED&&(cpp=sd->ident(id[i],&k))) {
			match=0;
			for(j=0,l=0;cpp[j]&&l<level;j++,l++)
				if(!regexec(&regex,cpp[j],0,NULL,0)) {
					match=1;
					break;
				}
			if(!match)
				id[i]=DS_UNUSED;
		} else 
			id[i]=DS_UNUSED;
	regfree(&regex);
}
