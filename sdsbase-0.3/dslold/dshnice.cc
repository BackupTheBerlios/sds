#include <fstream.h>
#include <sds/shareddata.h>
#include <getopt.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

static struct option const long_options[] = {
	{"cut",         required_argument, 0, 'c'},
	{"show",        required_argument, 0, 's'},
//	{"renumber",    required_argument, 0, 'n'},
//	{"reregister",  required_argument, 0, 'r'},
	{"overwrite",   no_argument,       0, 'o'},
	{"help",        no_argument,       0, 'h'},
	{"version",     no_argument,       0, 'V'},
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};

char verstring[] = "0.1";

char *program;

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
	      "usage: %s [options] dsh-file(s)\n"
		"\n"
		"-c,    --cut         cut branch\n"
		"-s,    --show        show particular branch\n"
//		"-n,    --renumber    renumber particular branch\n"
//		"-r,    --reregister  register final version under new name\n"
		"-o,    --overwrite   overwrite old dsh-file\n"
		"-h,-?, --help        this message\n"
		"-V,    --version     display version information and exit.\n"
		"-W,    --warranty    display licensing terms.\n"
		"-C,    --copyright  \n\n"
		"Use --show only once and exclusively!\n",
		program);
	exit(0);
}

void copyright(void)
{
	printf("%s version %s, Copyright (C) 1999 Roland Schaefer\n"
	       "%s comes with ABSOLUTELY NO WARRANTY;\n"
	       "This is free software, and you are welcome to"
	       " redistribute it\n"
	       "under certain conditions; type `%s -W' for details.\n",
	       program,verstring,program,program);
	exit(0);
}

void version(void)
{
	fprintf(stderr, "%s version %s\n", program,
		verstring);
	
	exit(0);
}

extern "C" {
extern struct Comment *comments;
extern int n_comments;
}

void add_token(Version **arg, int *narg, int *nargmax, const char *optarg);
void output(FILE *out, DshLine *dshline);
void del_dshlines(DshLine *dshlines, int ndshl, Version *del);
void mark_show_dshlines(DshLine *dshlines, int ndshl,
			Version *show, int level);
int del_match(DshLine *dshline, Version *del);
int show_match(DshLine *dshline, Version *del);
void check_others(DshLine *dshlines,const int ndshl,
		  const char *verbase, const int ver);
int *fetch_matched_show_lines(int *nrdshl,DshLine *dshline, int ndshl);

int main(int argc,char **argv)
{
	int overwrite=0;
	Version *del=NULL;
	int ndel=0,ndelmax=0;
	Version *show=NULL;
	int show_mode=0;
	int nshow=0,nshowmax=0;
	int c,i,j,k;
	DshLine *dshlines=NULL;
	int *rdshl=NULL;
	int ndshl;
	int nrdshl=0;
	FILE *out;

	program = basen(argv[0]);

	while ((c = getopt_long(argc, argv, "c:s:oh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;

		case 'c':
			add_token(&del,&ndel,&ndelmax,optarg);
			break;
		case 's':
			if(show_mode)
				usage();
			add_token(&show,&nshow,&nshowmax,optarg);
			show_mode=1;
			break;
		case 'o':
			overwrite=1;
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

	if(show_mode&&(ndel||overwrite))
		usage();

	for(i=optind;i<argc;i++) {
		dshlines=parse_dsh_file(&ndshl,argv[i]);
		if(overwrite) {
			out=fopen(argv[i],"w");
			if(!out) {
				fprintf(stderr,"%s can not be overwriten\n",
					argv[i]);
				continue;
			}
		} else
			out=stdout;

		output_global_comments(out);
		
		for(j=0;j<ndel;j++) {
			del_dshlines(dshlines,ndshl,del+j);
		}

		if(show_mode) {
			mark_show_dshlines(dshlines,ndshl,show,0);
			rdshl=fetch_matched_show_lines(&nrdshl, 
						       dshlines, ndshl);
			for(j=0;j<nrdshl;j++) {
				output(out,dshlines+rdshl[j]);
			}
			free_global_comments();
			free_dsh_line(dshlines,ndshl);
			if(rdshl)
				free(rdshl);
			continue;
		}

		for(j=0;j<ndshl;j++) {
			if(dshlines[j].flag&(DSHL_SHOWED|DSHL_DELETED))
				continue;
			rdshl=NULL;
			nrdshl=0;
			rdshl=fetch_relevant_lines(&nrdshl,dshlines[j].verbase,
						   0,dshlines,ndshl);
			for(k=0;k<nrdshl;k++)
				output(out,dshlines+rdshl[k]);

			if(rdshl)
				free(rdshl);
		}
		if(overwrite)
			fclose(out);
		free_global_comments();
		free_dsh_line(dshlines,ndshl);
	}
	return 0;
}

void add_token(Version **arg, int *narg, int *nargmax, const char *optarg)
{
	char *tmp;

	if(*nargmax<=*narg) {
		*nargmax+=10;
		*arg=(Version *)xrealloc(*arg,sizeof(Version)**nargmax);
	}
	
	if((tmp=strrchr(optarg,'.'))) {
		*tmp=0;
		if(sscanf(tmp+1,"%d",&(*arg)[*narg].ver)!=1) {
			(*arg)[*narg].ver=-1;
			*tmp='.';
		}
		
	} else {
		(*arg)[*narg].ver=-1;
	}
	
	(*arg)[(*narg)++].verbase=strdup(optarg);
}

void output(FILE *out,DshLine *dshline)
{
	int i,j;

	if(dshline->flag&(DSHL_SHOWED|DSHL_DELETED))
		return;

	fprintf(out,"%s.%d",dshline->verbase,dshline->ver);
	
	for(i=0;i<dshline->argc;i++) {
		if(blanks_in(dshline->argv[i]))
			fprintf(out," \"%s\"",dshline->argv[i]);
		else
			fprintf(out," %s",dshline->argv[i]);
	}
	
	fprintf(out,";");

	i=0;
	if(dshline->comc&&(dshline->comv[0]->type==COM_DSLASH)) {
		fprintf(out," %s\n",dshline->comv[0]->token);
		i=1;
	}
	else
		fprintf(out,"\n");

	for(j=0;j<dshline->dependc;j++) {
		fprintf(out,": %s",
			dshline->dependv[j]->source);
		if(dshline->dependv[j]->verbase)
			fprintf(out,"-%s.%d",
				dshline->dependv[j]->verbase,
				dshline->dependv[j]->ver);
		fprintf(out,"\n");
	}

	for(;i<dshline->comc;i++)
		fprintf(out,"%s\n",dshline->comv[i]->token);
	
	dshline->flag|=DSHL_SHOWED;
}

DshLine *dshlines_to_sort_show;

int compare_show(const void *a, const void *b)
{
	int retval=dshlines_to_sort_show[*(int*)b].level-
		dshlines_to_sort_show[*(int*)a].level;
	if(retval)
		return retval;
	return (int)(dshlines_to_sort_show[*(int*)a].ver-
		     dshlines_to_sort_show[*(int*)b].ver);
}

int *fetch_matched_show_lines(int *nrdshl,DshLine *dshlines, int ndshl)
{
	int i;
	int j=0;
	int *rdshl=NULL;

	for(i=0;i<ndshl;i++) {
		if(dshlines[i].flag&DSHL_SHOW) {
			rdshl=(int *)xrealloc(rdshl,sizeof(int)*(++j));
			rdshl[j-1]=i;
		}
	}
	dshlines_to_sort_show=dshlines;
	qsort(rdshl,j,sizeof(int),compare_show);
	*nrdshl=j;
	return rdshl;
}

void mark_show_dshlines(DshLine *dshlines, int ndshl, Version *show, int level)
{
	int i,j;
	regex_t regex;
	regmatch_t regm[2];
	char *fs,*source;

	regcomp(&regex,"dsh-file[[:blank:]]*of[[:blank:]]*\"(.*)\"",
		REG_EXTENDED);

	for(i=0;i<n_comments;i++) {
		if(!regexec(&regex,comments[i].token,2,regm,0)) {
			fs=strndup(comments[i].token+regm[1].rm_so,
				   regm[1].rm_eo-regm[1].rm_so);
			source=basen(fs);
			break;
		}
	}

	regfree(&regex);

	for(i=0;i<ndshl;i++) {
		if(dshlines[i].flag&DSHL_SHOW)
			continue;
		if(show_match(dshlines+i,show)) {
			dshlines[i].flag|=DSHL_SHOW;
			dshlines[i].level=level;
			for(j=0;j<dshlines[i].dependc;j++) {
				if(!strcmp(basen(dshlines[i].dependv[j]
						 ->source),source)) {
					mark_show_dshlines(dshlines,ndshl,
							   dshlines[i].
							   dependv[j],
							   level+1);
				}
			}
		}
	}

	free(fs);
}

void del_dshlines(DshLine *dshlines, int ndshl, Version *del)
{
	int i;

	for(i=0;i<ndshl;i++) {
		if(dshlines[i].flag&DSHL_DELETED)
			continue;
		if(del_match(dshlines+i,del)) {
			dshlines[i].flag|=DSHL_DELETE;
			check_others(dshlines,ndshl,dshlines[i].verbase,
				     dshlines[i].ver);
		}
	}
}

int del_match(DshLine *dshline, Version *del)
{
	if(!strcmp(del->verbase,dshline->verbase)&&del->ver<=dshline->ver)
		return 1;
	return 0;
}

int show_match(DshLine *dshline, Version *show)
{
	if(!show->verbase||!dshline->verbase)
		return 0;
	if(!strcmp(show->verbase,dshline->verbase)&&show->ver>=dshline->ver)
		return 1;
	return 0;
}

void check_others(DshLine *dshlines,const int ndshl,
		  const char *verbase, const int ver)
{
	int i,j;
	regex_t regex;
	regmatch_t regm[2];
	char *source,*fs;
	Version del;

	regcomp(&regex,"dsh-file[[:blank:]]*of[[:blank:]]*\"(.*)\"",
		REG_EXTENDED);

	for(i=0;i<n_comments;i++) {
		if(!regexec(&regex,comments[i].token,2,regm,0)) {
			fs=strndup(comments[i].token+regm[1].rm_so,
				   regm[1].rm_eo-regm[1].rm_so);
			source=basen(fs);
			break;
		}
	}

	regfree(&regex);

	for(i=0;i<ndshl;i++) {
		if(dshlines[i].flag&(DSHL_DELETED|DSHL_SHOWED))
			continue;
		for(j=0;j<dshlines[i].dependc;j++) {
			if(!strcmp(source,
				   basen(dshlines[i].dependv[j]->source))&&
			   dshlines[i].dependv[j]->verbase&&
			   verbase&&
			   !strcmp(verbase,dshlines[i].dependv[j]->verbase)&&
			   ver<=dshlines[i].dependv[j]->ver) {
				del.verbase=dshlines[i].verbase;
				del.ver=dshlines[i].ver;
				del_dshlines(dshlines,ndshl,&del);
			}
		}
	}

	free(fs);
}
