#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <search.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <SharedData.h>

#define SD_FILE "SDF"

int dsreg_yyparse(void);
int dsh_yyparse(void);
extern FILE *dsreg_yyin;
extern FILE *dsh_yyin;
extern int dsh_yydebug;

int mode;
FILE *dsh;
idset *wanted, *found, *temps, *perms, *varlist;
char *vername="hallo";

static struct option const long_options[] =
{
	{"help",     no_argument,       0, 'h'},
	{"verbose",  no_argument,       0, 'v'},
	{"keep",     no_argument,       0, 'k'},
	{"load",     no_argument,       0, 'l'},
	{"output",   required_argument, 0, 'o'},
	{"name",     required_argument, 0, 'n'},
	{"version",  no_argument,       0, 'V'},
	{"warranty", no_argument,       0, 'W'},
	{"copyright", no_argument,      0, 'C'},
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
	      "usage: %s [-o out] -n name ids\n"
		"\n"
		"-o,    --output     name of dsh-file\n"
		"-n,    --name       name of version\n"
		"-k,    --keep       do not rename the ids\n"
		"-l,    --load       simulate loading, rename the ids\n"
		"-v,    --verbose    print info \n"
		"-h,-?, --help       this message\n"
		"-V,    --version    display version information and exit.\n"
		"-W,    --warranty   display licensing terms.\n"
		"-C,    --copyright\n",
		program_invocation_short_name);
	exit(0);
}

void version(void)
{
	fprintf(stderr, "%s version %s\n", program_invocation_short_name,
		verstring);

	exit(0);
}

char * basen(const char *name)
/* basename */
{
	char *rcharp;
	
	if(!(rcharp=strrchr(name,'/')))
		rcharp=(char*)name;
	else
		rcharp++;
	return rcharp;
}

int check_dsh_file(const char *name)
/* checks wether a file "name" exist and can be opened with read
 * access or if it can be created (in which case it gets
 * initialized). */
{
	char *mess;
	int fd;

	if(name==NULL)
		return 1;

	if((fd=open(name,O_RDONLY))!=-1) {
		close(fd);
// 		dsh_yydebug=1;
		dsh_yyin=fopen(name,"r");
		rewind(dsh_yyin);
		if (dsh_yyparse()) {
			fprintf(stderr,"SERIOUS WARNING! in dsh-file \"%s\" a "
				"version named \"%s\"\nalready exists! If you "
				"don't change this by hand, the version you\n"
				"registered now will be ignored by dsl!\n",name,
				vername);
			fclose(dsh_yyin);
			return -1;
		}
		fclose(dsh_yyin);
		return 0;

	}
	if((fd=open(name,O_WRONLY|O_CREAT|O_EXCL,0666))!=-1) {
		asprintf(&mess,"/* dsh-file \"%s\" */\n",basen(name));
		write(fd,mess,strlen(mess));
		free(mess);
		close(fd);
		return 0;
	}
	return 1;
}


char *check_path(const char *name) {
	char *path = getenv("SDS_LOAD_PATH");
	char *pe, *pd, *tmp, *test, *ret;
	int fd;

	tmp=pd=strdup(path);
	while(pd) {
		pe=strsep(&pd,":");
		asprintf(&test,"%s/%s",pe,name);
		errno=0;
		if((fd=open(test,O_RDONLY))!=-1) {
			close(fd);
			free(test);
			ret=strdup(pe);
			free(tmp);
			return ret;
		}
		free(test);
	}
	free(tmp);
	return NULL;
}

char *full_path(const char *name) {
	char *ret;
	char *pwd=getcwd(NULL,0);

	if (!name) 
		return pwd;

	if (chdir(name)) {
		free(pwd);
		return NULL;
	}		
	ret=getcwd(NULL,0);
	chdir(pwd);
	free(pwd);
	return ret;
}
	

char *resolve_name(const char *name) {
/* this routine, given the filename of the -o option, tries to figure out 
   which name should be used in a call to dsl. If a matching SDS_LOAD_PATH
   is found, we check if there is another file with the same dsl-name in an
   earlier part of SDS_LOAD_PATH. Then, the dsl-name is expanded. */
	char *path = getenv("SDS_LOAD_PATH");
	char *nd, *np, *nf;
	char *pd, *pe, *pf, *cp;
	char *tmp, *ret=NULL;

	if (!path) {
		fprintf(stderr,"ERROR! Variable \"SDS_LOAD_PATH\" unset!\n");
		return NULL;
	}

	nd=strdup(name);
	np=strrchr(nd,'/');
	if (np) {
		*np='\0';
		nf=full_path(nd);
		ret=strdup(np+1);
		while (np) { // np switches role now!
			tmp=pd=strdup(path);
			while (pd) {
				pe=strsep(&pd,":");
				pf=full_path(pe);
				if (!strcmp(pf,nf)) {
					cp=check_path(ret);
					if(!strcmp(pe,cp)) {
						free(nd);
						free(nf);
						free(pf);
						free(cp);
						free(tmp);
						return ret;
					} else {
						fprintf(stderr,"WARNING! "
							"Shortest load-name \""
							"%s\" for file %s is "
							"not unique!\nSame name"
							" exists in %s. Looking"
							" for alternatives..\n",
							ret,name,cp);
						free(cp);
					}
					
				}
				free(pf);
			}
			free(tmp);
			np=strrchr(nf,'/');
			if (np) {
				*np='\0';
				tmp=ret;
				asprintf(&ret,"%s/%s",np+1,tmp);
				free(tmp);
			}
		}
	} else {
		nf=getcwd(NULL,0);
		ret=nd;
		tmp=pd=strdup(path);
		while (pd) {
			pf=full_path(strsep(&pd,":"));
			if (!strcmp(pf,nf)) {
				free(pf);
				free(nf);
				free(tmp);
				return ret;
			}
		}
		free(tmp);
	}
	free(nf);
	free(ret);
	nd=strdup(name);
	np=strrchr(nd,'/');
	if (np) {
		*np='\0';
		asprintf(&ret,"%s/%s",full_path(nd),np+1);
	} else {
		asprintf(&ret,"%s/%s",getcwd(NULL,0),nd);
	}
	free(nd);
	fprintf(stderr,"WARNING! file \"%s\" not in SDS_LOAD_PATH! Using absolute "
		"filename \"%s\"!\n",name,ret);
	return ret;
}				
				
int main(int argc,char **argv)
{
	char *log=NULL;
	char *logstr;
	char **tmp;
	char ** ids;
	int idn;
	char *permstr;
	char *dshname=NULL;
	char *dshbname=NULL;
	char *dshlname;
	char *dshpname;
	int c,i,j;
	int tofile=0;
	int rename=0;
	int verbose=0;
	SharedData hallo; // locking SD_FILE
	    
 	if(!(log=getenv(SD_FILE))) 
 		fatal("No environmet variable %s\n",SD_FILE);
	asprintf(&log,"%s.log",log);
	dsreg_yyin = fopen(log,"r");
	free(log);

	while ((c = getopt_long(argc, argv, "o:n:klvh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'o':
			dshname=strdup(optarg);
			tofile=1;
			break;
		case 'n':
			vername=strdup(optarg);
			break;
		case 'v':
			verbose=1;
			break;
		case 'k':
			rename=0;
			break;
		case 'l':
			rename=1;
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

	if (tofile) {
		i=check_dsh_file(dshname);
		if ((i==-1)&&rename) {
			fprintf(stderr,"I will not rename this thing!\n");
			rename=0;
		}
		if ((i>0) || !(dsh=fopen(dshname,"a"))) {
			fprintf(stderr,"something went wrong with %s!\n", 
				dshname);
			return 1;
		}
		rewind(dsh);
	} else 
		dsh=stdout;

	// lists of ids to register
	wanted = init_set(0);
	found = init_set(0);
	temps = init_set(0);
	perms = init_set(0);

	for(i=optind;i<argc;i++) {
		int id;
		errno=0;
		id=strtol(argv[i],NULL,0);
		if (errno) {
			fprintf (stderr,"ID Overflow in %s\n",argv[i]);
			continue;
		}
		wanted = add_to_set(wanted,id,0);
	}

	if (!wanted->n) return 0;

	do {
		mode=0;
		
		if (verbose) {
			printf("looking for: ");
			print_set(stdout,wanted);
		}

		dsreg_yyparse();

		sort_set(wanted);
		sort_set(found);
		sort_set(temps);
		perms=exclude_sets(found,temps);
		sort_set(perms);

		if (verbose) {
			printf("\nfound: ");
			print_set(stdout,found);
			printf("\nmarked as temporary ids: ");
			print_set(stdout,temps);
			printf("\nnon-temporary ids: ");
			print_set(stdout,perms);
			printf("\nfound %d new dependencies\n",mode);
			if (mode) printf("another round needed!\n\n");
		}
		
		rewind(dsreg_yyin);

	} while (mode);

	if (!perms->n) {
		rename = 0;
		fprintf(stderr,"SERIOUS WARNING!! All the ids you are trying "
			"to register seem to be removed!\nYou will have to "
			"modify the dsh-file by hand, otherwise dsl will "
			"fail!\n");
	}

	mode=-1;
	varlist = join_sets(perms,temps);
	
	if (varlist->n) {
		if (verbose) {
			printf("collecting tags.. \n");
			if (tofile)
				printf("writing output to file \"%s\"..\n",
				       dshname);
		}
		fprintf(dsh,"\n%s : ",vername);
		print_set(dsh,transpose_set(varlist,perms));
		fprintf(dsh," {\n");
		dsreg_yyparse();
		fprintf(dsh,"}\n");
	}

	if (tofile && rename ) {
		for (i=0;i<perms->n;i++) 
			if (!hallo.hasid(perms->ids[i])) {
				rename=0;
				fprintf(stderr,"ERROR trying to rename ids: "
					"id %d does not exist in SDF!\n",
					perms->ids[i]);
				break;
			}
		if (rename) {
			dshbname = basen(dshname);
			dshlname = resolve_name(dshname);
			dshpname = check_path(dshlname);
			printf("new id(s)=");
			for (i=0;i<perms->n;i++) {
				asprintf(&logstr,"<+%d+>\tdsrm $%d",
					 perms->ids[i], perms->ids[i]);
				hallo.log(logstr);
				free(logstr);
				perms->ids[i]=hallo.newid(perms->ids[i]);
				printf("%d ",perms->ids[i]);
				ids=hallo.ident(perms->ids[i],&idn);
				if (idn<2)
					idn=2;
				tmp=(char **)malloc((idn+1)*sizeof(char *));
				asprintf(&tmp[0],"%s::%s",dshlname,vername);
				asprintf(&tmp[1],"%s/%s::%s:%d",dshpname,
					 dshlname,vername,i+1);
				for (j=2;j<idn;j++)
						tmp[j]=strdup(ids[j]);
				tmp[idn]=NULL;
				hallo.set_ident(perms->ids[i],
					       (const char **) tmp);
				for (j=0;j<idn;j++)
					free(tmp[j]);
				free(tmp);
			}
			printf("\n");
			permstr=string_set(perms);
			asprintf(&logstr,"<*%s*>\tdsl %s::%s",permstr,
					 dshlname,vername);
			hallo.log(logstr);
			free(logstr);
		}
	}
	if (tofile) fclose(dsh);
	fclose(dsreg_yyin);
	return 0;
} 

