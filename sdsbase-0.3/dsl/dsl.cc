#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <SharedData.h>

extern char *program_invocation_short_name;

static struct option const long_options[] = {
	{"plotlevel",   required_argument, 0, 'p'},
	{"recurse",     required_argument, 0, 'r'},
	{"recurse_forever",  no_argument,  0, 'R'},
	{"verbose",     no_argument,       0, 'v'},
	{"help",        no_argument,       0, 'h'},
	{"version",     no_argument,       0, 'V'},
	{"warranty",    no_argument,       0, 'W'},
	{"copyright",   no_argument,       0, 'C'},
	{0, 0, 0, 0}
};


extern FILE *dsh_yyin;
int dsh_yyparse(void);
void dsh_yyrestart(FILE *);
extern int dsh_yydebug;

extern FILE *dat_yyin;
int dat_yyparse(void);
void dat_yyrestart(FILE *);
extern int dat_yydebug;

char verstring[] = "0.04";

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
	        "usage: %s [-p #] [-x #] [-y #] source_name(s)\n"
	        "   or: %s [-p #] [-R|-r #] dsh_file::name(s)\n"
		"\n"
		"Loads data to file given by the environment variable SDF,\n"
		"searching for source_names in SOURCE_PATH and for "
		"dsh_files in DSH_PATH,\n respectively."
	        "A list of new IDs is printed on stdout.\n"
		"\n"
		"-x,                       column number of x-values\n"
		"-y,                       column number of y-values\n"
		"-p,    --plotlevel        load data to given plotlevel\n"
		"-R,    --recurse_forever  create log entries recursively\n"
		"-r,    --recurse          create log entries recursively "
		" up to given level\n"
		"-v,    --verbose\n"
		"-h,-?, --help             this message\n"
		"-V,    --version          display version information "
		"and exit.\n"
		"-W,    --warranty         display licensing terms.\n"
		"-C,    --copyright  \n",
		program_invocation_short_name,program_invocation_short_name);
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

int load_dat(char *dat, int xcol, int ycol);

int load_dsh(char *dsh, char *name);


char *name;
int gotit;
SharedData *sdf;
idset *retids=NULL;
int recurse=0;
int verbose=0;
int srcid;
int srctype;
int srcn;

int main(int argc,char **argv)
{
	int x=1,y=2;
	int c,i,j;
	int pl=0;
	int set_pl=0;
	int status=0;

	while ((c = getopt_long(argc, argv, "x:y:p:Rr:vh?VWC",
				long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'x':
			if(sscanf(optarg,"%d",&x)!=1)
				usage();
			break;
		case 'y':
			if(sscanf(optarg,"%d",&y)!=1)
				usage();
			break;
		case 'p':
			if(sscanf(optarg,"%d",&pl)!=1)
				usage();
			set_pl=1;
			break;
		case 'v':
			verbose=1;
			break;
		case 'r':
			if (sscanf(optarg,"%d",&recurse)!=1)
				usage();
			break;
		case 'R':
			recurse=-1;
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

	sdf = new SharedData;

	for(i=optind;i<argc;i++) {
		
		if (retids)
			free_idset(retids);
		
		if ((name=strstr(argv[i],"::"))) {
			char *dsh=strndup(argv[i],(name-argv[i]));

			name+=2;

			if (verbose) 
				fprintf(stderr,"trying to load version \"%s\" "
					"from dsh-file %s\n",name,dsh);

			if (!load_dsh(dsh,name)) 
				for (j=0;j<retids->n;j++) {
					printf("%d ",retids->ids[j]);
					if (set_pl) 
						sdf->set_plev(retids->ids[j],
							      pl);
				}
			else {
				fprintf(stderr,"something went wrong with "
					"%s::%s !\n",dsh,name);
				status++;
			}
		} else {
			srcid=0;
			if (verbose)
				fprintf(stderr,"trying to load data from file "
					"\"%s\"\n", argv[i]);
			if (!load_dat(argv[i],x,y)) {
				for (j=0;j<retids->n;j++) {
					printf("%d ",retids->ids[j]);
					if (set_pl) 
						sdf->set_plev(retids->ids[j],
							      pl);
				}
			} else {
				fprintf(stderr,"something went wrong with "
					"%s !\n",argv[i]);
				status++;
			}
		}
	}
	printf("\n");
	return status;

}


char *look_for_load(const char *name, const char *pathvar) 
/* This routine looks for readable files in the paths described by the
 * enviroment variable pathvar and returns the full file name of
 * the first file found. If "name" begins with a "/" the name given is
 * supposed to be an absolute file name. The address returned has to be
 * freed explicitely when not needed any longer. When no file is found
 * a NULL pointer is returned.  */
{
	char *path,*pd;
	char *tmp,*test;
	int fd;

	if(name[0]=='/') {
		asprintf(&test,"%s",name);
		errno=0;
		if((fd=open(test,O_RDONLY))!=-1) {
			close(fd);
			return test;
		}
		free(test);
		return NULL;
	}

	path=getenv(pathvar);
	if(!path)
		fatal("Environment variable \"%s\" "
		      "has no value!\n",pathvar);

	if (verbose) 
		fprintf(stderr,"looking for %s in %s=%s\n",name,pathvar,path);

	pd=tmp=strdup(path);
	while(tmp) {
		asprintf(&test,"%s/%s",strsep(&tmp,":"),name);
		errno=0;
		if((fd=open(test,O_RDONLY))!=-1) {
			close(fd);
			free(pd);
			return test;
		}
		free(test);
	}
	free(pd);
	return NULL;
}


int load_dsh(char *dsh, char *name) {
	int ret=0;
	int i,j,ols;
	char *log=NULL;
	char **tmp;
	char *ids=NULL;
	char *dshl=NULL;
	char **idents;
	int identn;
	
// 	dsh_yydebug=1;

	dshl = look_for_load(dsh,"SDS_LOAD_PATH");
	if (!dshl || !(dsh_yyin=fopen(dshl,"r"))) return 1;
	if (verbose)
		fprintf(stderr,"opening file %s for reading\n",dshl);
	rewind(dsh_yyin);
	dsh_yyrestart(dsh_yyin);
	gotit=0;

	if (!recurse) ols = sdf->logstate(LOGOFF);
	if (!dsh_yyparse()) {
		if (!gotit) {
			fprintf(stderr,"No \"%s\" in file \"%s\"!\n",name,
				dshl);
			ret=1;
		}
		else if (!recurse) {
			ids=string_set(retids);
			asprintf(&log,"<*%s*>\tdsl %s::%s",ids,dsh,name);
			free(ids);
			for (j=0;j<retids->n;j++) {
				idents=sdf->ident(retids->ids[j],&identn);
				if (identn<2)
					identn=2;
				tmp=(char **)malloc((identn+1)*sizeof(char *));
				asprintf(&tmp[0],"%s::%s",dsh,name);
				asprintf(&tmp[1],"%s::%s:%d",dshl,name,j+1);
				for (i=2;i<identn;i++)
						tmp[i]=strdup(idents[i]);
				tmp[identn]=NULL;
				sdf->set_ident(retids->ids[j],
					       (const char **) tmp);
				for (i=0;i<identn;i++)
					free(tmp[i]);
				free(tmp);
			}
		}
	} else {
		ret=1;
	}			
	free(dshl);
	fclose(dsh_yyin);
	if (!recurse) sdf->logstate(ols);
	if (log) sdf->log(log);
	return ret;
}

int load_dat(char *dat, int xcol, int ycol) {
	double *x, *y;
      
	static char *line=NULL;
	static unsigned ln=0;

	int i=0, s=10000;
	double tx, ty;

	char *datl=NULL;
	char *ident;

	size_t sz;
	
	long int pos;

	datl = look_for_load(dat,"SDS_SOURCE_PATH");
	if (!datl || !(dat_yyin=fopen(datl,"r"))) return 1;
	if (verbose)
		fprintf(stderr,"opening file %s for reading\n",datl);
	rewind(dat_yyin);
	pos=0;
	retids=init_set(0);
//	dat_yyrestart(dat_yyin);

	while (!(feof(dat_yyin))) {

		srcid=sdf->create(0);
		srctype=0;
		srcn=0;

		pos=ftell(dat_yyin);

		if (verbose && pos)
			printf("continuing at filepos=%ld\n",pos);

		if (dat_yyparse()||fseek(dat_yyin,pos,SEEK_SET)) {
			fclose(dat_yyin);
			sdf->remove(srcid);
			return 1;
		}

		if (srctype==1) { // saved as ASCII
			if (srcn<1) {
				sdf->remove(srcid);
				fclose(dat_yyin);
				return 1;
			}
			
			if (verbose)
				fprintf(stderr,"found ASCII data "
					"produced by dss\n");
			
			x=sdf->x(srcid);
			y=sdf->y(srcid);
			
			i=0;
			while((getdelim(&line,&ln,'\n',dat_yyin))!=-1){
				if(parsel(line,&tx,&ty,1,2)==2) {
					x[i]=tx;
					y[i++]=ty;
					if (i==srcn) break;
				}
			}
			sdf->set_n(srcid,srcn);
			add_to_set(retids,srcid,0);
		} else if (srctype==2) { //saved as binary
			if (srcn<1) {
				sdf->remove(srcid);
				fclose(dat_yyin);
				return 1;
			}

			if (verbose)
				fprintf(stderr,"found binary data "
					" produced by dsd\n");

			x=sdf->x(srcid);
			y=sdf->y(srcid);

			i=0;
			sz = (size_t) srcn;
			while((i=fgetc(dat_yyin))!=EOF&&(i!=032));
			errno=0;
			if(sz!=fread(x,sizeof(double),sz,dat_yyin)||
			   sz!=fread(y,sizeof(double),sz,dat_yyin)) {
				return 1; 
			}
			
			sdf->set_n(srcid,srcn);
			add_to_set(retids,srcid,0);
		} else if (!pos) { // unknown ASCII data at begin of file
			sdf->resize(srcid,s);
			x=sdf->x(srcid);
			y=sdf->y(srcid);
			if (verbose)
				fprintf(stderr,"found raw data.. "
					"picking columns "
					"x=%d, y=%d\n", xcol, ycol);
			
			while((getdelim(&line,&ln,'\n',dat_yyin))!=-1){
				if(parsel(line,&tx,&ty,xcol,ycol)==2) {
					x[i]=tx;
					y[i++]=ty;
					if(i==s) {
						s+=10000;
						sdf->resize(srcid,s);
						x=sdf->x(srcid);
						y=sdf->y(srcid);
					}
				}
			}
			if (verbose) 
				fprintf(stderr,"read %d datapoints\n",i);
			sdf->resize(srcid,i);
			sdf->set_n(srcid,i);
			sdf->add_ident(srcid,dat);
			asprintf(&ident,"%s: x=%d y=%d",datl, xcol,ycol);
			sdf->add_ident(srcid,ident);
			free(ident);
			asprintf(&ident,"<*%d*>\tdsl -x %d -y %d %s",srcid,xcol,ycol,
				 dat);
			sdf->log(ident);
			free(ident);
			sdf->changed();
			fclose(dat_yyin);
			add_to_set(retids,srcid,0);
			return 0;
		} else {   // we reached the end..
			if (verbose) 
				printf("end of dss/dsd data..\n");
			sdf->remove(srcid);
			break;

		}
		
	}

	asprintf(&ident,"<*%s*>\tdsl %s",string_set(retids),dat);
	sdf->log(ident);
	free(ident);

	sdf->changed();
	fclose(dat_yyin);

	return 0;

}


