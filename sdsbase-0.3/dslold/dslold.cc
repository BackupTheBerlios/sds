#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <regex.h>
#include <ctype.h>
#include "dsh_y_l.h"
#include <SharedData.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SDS_SOURCE_PATH "SDS_SOURCE_PATH"
#define SDS_LOAD_PATH   "SDS_LOAD_PATH"

#define VER_UNSET -2

extern "C" {
int dsh_yyparse(void);

extern struct DshLine *dshlines;
extern int n_dshlines;
extern int max_ind_dshlines;
extern int n_comments;
extern int ncom;
extern int max_ind_comments;
extern struct Comment *comments;
}

char *look_for_source(const char *name)
/* This routine looks for readable files in the paths described by the
 * enviroment variable SDS_SOURCE_PATH and returns for the first file
 * found the corresponding token from SDS_SOURCE_PATH. If "name"
 * begins with a "/" the name given is supposed to be an absolute file
 * name. The address returned has to be freed explicitely when not
 * needed any longer. When no file is found a NULL pointer is
 * returned.  */
{
	char *path,*pd;
	char *tmp,*dtest,*test;
	int fd;

	if(name[0]=='/') {
		asprintf(&test,"%s",name);
		errno=0;
		if((fd=open(test,O_RDONLY))!=-1) {
			close(fd);
			free(test);
			return strdup("");
		}
		free(test);
		return NULL;
	}
	
	path=getenv(SDS_SOURCE_PATH);
	if(!path)
		fatal("Environment variable \"%s\" "
		      "has no value!\n",SDS_SOURCE_PATH);

	pd=tmp=strdup(path);
	while(tmp) {
		asprintf(&dtest,"%s",strsep(&tmp,":"));
		asprintf(&test,"%s/%s",dtest,name);
		errno=0;
		if((fd=open(test,O_RDONLY))!=-1) {
			close(fd);
			free(test);
			free(pd);
			return dtest;
		}
		free(dtest);
		free(test);
	}
	free(pd);
	return NULL;
}

char *look_for_load(const char *name)
/* This routine looks for readable files in the paths described by the
 * enviroment variable SDS_LOAD_PATH and returns the full file name of
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

	path=getenv(SDS_LOAD_PATH);
	if(!path)
		fatal("Environment variable \"%s\" "
		      "has no value!\n",SDS_LOAD_PATH);

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

char *check_dsh_file(const char *dir, const char *rname)
/* checks wether a file "name.dsh" exist and can be opend with read
 * access or if it can be created (in which case it gets
 * initialized). The full file name of the dsh file is returned. The
 * returned adress has to be freed if not longer needed. */
{
	char *test,*mess;
	int fd;

	if(rname==NULL)
		return NULL;
	asprintf(&test,"%s/%s.dsh",dir,rname);
	if((fd=open(test,O_RDONLY))!=-1) {
		close(fd);
		return test;
	}
	if((fd=open(test,O_WRONLY|O_CREAT|O_EXCL,0666))!=-1) {
		asprintf(&mess,"/* dsh-file of \"%s\" */\n",rname);
		write(fd,mess,strlen(mess));
		free(mess);
		close(fd);
		return test;
	}
	free(test);
	return NULL;
}

DshLine *parse_dsh_file(int *n, const char *file_name)
/* Returned is the address of an array of DshLine containing the
 * information of the n (as retured by variable n) valid dsh lines in
 * the file "file_name" */
{
	extern FILE *dsh_yyin;

	dshlines=NULL;
	max_ind_dshlines=n_dshlines=0;
	dsh_yyin=fopen(file_name,"r");

	if(!dsh_yyin) {
		*n=0;
		return NULL;
	}
	
	dsh_yyparse();

	fclose(dsh_yyin);

	*n=n_dshlines;
	return dshlines;
}

void free_dsh_line(DshLine *dshline,int ndsh)
/* free the memory allocated during the parse process of dsh files */
{
	int i,j;
	for(i=0;i<ndsh;i++) {
		for(j=0;dshline[i].argv[j];j++)
			free(dshline[i].argv[j]);
		free(dshline[i].argv);
		for(j=0;j<dshline[i].comc;j++) {
			free(dshline[i].comv[j]->token);
			free(dshline[i].comv[j]);
		}
		free(dshline[i].comv);
		for(j=0;j<dshline[i].dependc;j++) {
			if(dshline[i].dependv[j]->source)
				free(dshline[i].dependv[j]->source);
			if(dshline[i].dependv[j]->verbase)
				free(dshline[i].dependv[j]->verbase);
			free(dshline[i].dependv[j]);
		}
		free(dshline[i].dependv);
	}
	if(dshline)
		free(dshline);
}

void output_global_comments(FILE *out)
/* output global comments found in the parse process of dsh files to
 * stream out */
{
	int i,j;
	if(n_comments) {
		fprintf(out,"/*");
		j=comments[0].type;
	}
	for(i=0;i<n_comments;i++) {
		if(j<comments[i].type) {
			j=comments[i].type;
			fprintf(out,"*/\n\n/*");
		}
		fprintf(out,"%s",comments[i].token);
	}
	if(n_comments)
		fprintf(out,"*/\n\n");
}

void free_global_comments(void)
/* free memory allocated for global comments during the parse process
 * of dsh files */
{
	int i;
	for(i=0;i<n_comments;i++)
		free(comments[i].token);
	if(comments)
		free(comments);
	n_comments=max_ind_comments=ncom=0;
	comments=NULL;
}

void fatal(const char *templ, ...)
{
	char *tmp;
	va_list ap;

	va_start(ap,templ);
	vasprintf(&tmp,templ,ap);
	va_end(ap);

	if(errno)
		perror(tmp);
	else
		fprintf(stderr,"FATAL: %s\n",tmp);
	free(tmp);
	exit(1);
}

int parse_full_name(const char *full_name, 
		    char **rname, char **verbase, int *ver)
/* lexically analyze full_name, split tokens and return them in
 * freshly allocated memory */
{
	char *tmp1,*tmp2;

	*rname=NULL;
	*verbase=NULL;
	*ver=VER_UNSET;

	if((tmp1=strrchr(full_name,'-'))==NULL)
		return 1;
	*tmp1=0;
	*rname=strdup(full_name);
	*tmp1='-';

	tmp1++;
	if((tmp2=strrchr(tmp1,'.'))==NULL)
		return 2;

	*tmp2=0;
	*verbase=strdup(tmp1);
	*tmp2='.';
	
	tmp2++;
	if(sscanf(tmp2,"%d",ver)!=1)
		return 3;

	return 0;
}

DshLine *dshlines_to_sort;

int compare_rdshl(const void *a, const void *b)
{
	return (int)(dshlines_to_sort[*(int*)a].ver-
		     dshlines_to_sort[*(int*)b].ver);
}

int *fetch_relevant_lines(int *nrdshl,
			  const char *verbase, int ver,
			  DshLine *dshlines, int ndsh)
/* this function can be used only after parsing a dsh file: it is
 * going to return an integer array of indices into the array of
 * DshLine stuctures which corresponds to verbase. If none of the
 * matching DshLine elements corresponds to ver, NULL is returned.
 * Let rdshl be the returned integer array.  This array of indices is
 * ordered so that dhsline[i].ver==i.  In case this relation can
 * not be fulfilled because the dsh file is corrupt a NULL pointer is
 * returned.  The returned integer pointer should be freed if not
 * needed anymore.  */
{
	int i;
	int j=0;
	int *rdshl=NULL;
	
	if(!verbase) {
		*nrdshl=0;
		return NULL;
	}

	for(i=0;i<ndsh;i++) {
		if(strcmp(verbase,dshlines[i].verbase)==0) {
			rdshl=(int *)xrealloc(rdshl,sizeof(int)*(++j));
			rdshl[j-1]=i;
		}
	}
	if(j<=ver) {
		if(rdshl)
			free(rdshl);
		return NULL;
	}

	dshlines_to_sort=dshlines;
	qsort(rdshl,j,sizeof(int),compare_rdshl);
		
	for(i=0;i<j;i++) {
		if(dshlines[rdshl[i]].ver!=i) {
			free(rdshl);
			return NULL;
		}
	}
	*nrdshl=j;
	return rdshl;
}

void * xmalloc (size_t size)
{
	errno=0;
	register void *value = malloc (size);
	if (value == 0)
		fatal ("Virtual memory exhausted");
	return value;
}

void * xrealloc (void *ptr, size_t size)
{
	register void *value = realloc (ptr, size);
	if (value == 0)
		fatal ("Virtual memory exhausted");
	return value;
}

char *progs[]={
	{"dsIUfit"},
	{"dsaco"},
	{"dsdel"},
	{"dseinige"},
	{"dsfilter"},
	{"dscf"},
	{"dsfit"},
	{"dsfunc"},
	{"dsint"},
	{"dspower"},
	{"dssc"},
	{"dssort"},
	{"dsspline"},
	{"dswl"}
};

char *need_R[]={
	{"dsIUac"},
	{"dsIUfit"},
	{"dsIUheat"},
	{"dsIUrad"},
	{"dsIVac"},
	{"dsIVacheat"},
	{"dsIVfit"},
	{"dsIVheat"},
	{"dscf"},
	{"dsfit"},
	{"dspeak"},
	{"dsspline"},
	{"dswl"}
};

int exec_command(DshLine *dshlines, int n, int id)
{
	int i,j,np=sizeof(progs)/sizeof(char *);
	int nnR=sizeof(need_R)/sizeof(char *);
	int status;
	pid_t pid;
	
	for(i=0;i<np;i++) {
		if(!strcmp(progs[i],dshlines[n].argv[0]))
			break;
	}
	if(i==np) {
		fprintf(stderr,"Calls to \"%s\" cannot be handled\n",
			dshlines[n].argv[0]);
		return DS_UNUSED;
	}

	if(strcmp(dshlines[n].argv[dshlines[n].argc-1],"$@")==0) {
		free(dshlines[n].argv[dshlines[n].argc-1]);
		asprintf(&dshlines[n].argv[dshlines[n].argc-1],
			 "%d",id);
	}

	for(i=0;i<nnR;i++) {
		if(!strcmp(need_R[i],dshlines[n].argv[0])) {
			fprintf(stderr,"Adding option -R");
			dshlines[n].argv=(char **)xrealloc(dshlines[n].argv,
							   sizeof(char *)*
							   ((++dshlines[n].
							     argc)+1));
			dshlines[n].argv[dshlines[n].argc]=NULL;
			asprintf(&dshlines[n].argv[dshlines[n].argc-1],
				 "-R");
		}
	}

	fprintf(stderr," -> ");
	for(j=0;dshlines[n].argv[j];j++)
		printf("%s ", dshlines[n].argv[j]);
	printf("\n");

	pid = fork ();
	if (pid == 0) {
		/* This is the child process.  Execute the shell command. */
		execvp (dshlines[n].argv[0],dshlines[n].argv);
		_exit (EXIT_FAILURE);
	}
	else if (pid < 0)
		/* The fork failed.  Report failure.  */
		status = -1;
	else { /* This is the parent process. */ 
		if (waitpid (pid, &status, 0) != pid) /* Wait for the
						       * child to
						       * complete. */
			status = -1;
	}
	return status;
}

char *load_progs[]={
	{"dsl"},
	{"dssc"}
};

int exec_load_command(DshLine *dshlines, int n, const char *rname)
{
	SharedData *sd;
	int status,i;
	pid_t pid;
	

	int *oid,*nid;
	int noid,nnid;

	int newid=DS_UNUSED;
	
	int j,nlp=sizeof(load_progs)/sizeof(char *);

	for(i=0;i<nlp;i++) {
		if(!strcmp(load_progs[i],dshlines[n].argv[0]))
			break;
	}

	if(i==nlp) {
		fprintf(stderr,"A call to \"%s\" would not lead to a new id,"
			" would it?\n",dshlines[n].argv[0]);
		return newid;
	}
	
	for(i=0;dshlines[n].argv[i];i++) {
		if(!strcmp(dshlines[n].argv[i],"-q")) {
		        fprintf(stderr,"deleting \"%s\"",dshlines[n].argv[i]);
			free(dshlines[n].argv[i]);
			for(j=i;dshlines[n].argv[j];j++)
				dshlines[n].argv[j]=dshlines[n].argv[j+1];
			dshlines[n].argc--;
		}
	}

	if(!strcmp("dssc",dshlines[n].argv[0])) {
		for(i=0;dshlines[n].argv[i];i++) {
			if(!strcmp(dshlines[n].argv[i],"-o")) {
				fprintf(stderr," deleting \"%s\" and \"%s\"",
					dshlines[n].argv[i],
					dshlines[n].argv[i+1]);
				free(dshlines[n].argv[i]);
				free(dshlines[n].argv[i+1]);
				for(j=i+1;dshlines[n].argv[j];j++)
					dshlines[n].argv[j-1]=
						dshlines[n].argv[j+1];
				dshlines[n].argc-=2;
			}
		}
	}

	if(!strcmp("dsl",dshlines[n].argv[0])) {
		if(dshlines[n].dependc!=1) {
			fprintf(stderr,"To hard to gess how to call \"%s\"\n",
				dshlines[n].argv[0]);
			
		} else if(dshlines[n].dependv[0]->verbase) {
			fprintf(stderr," calling dslold instead of dsl");
			free(dshlines[n].argv[0]);
			asprintf(&dshlines[n].argv[0],
				 "%s","dslold");
		}
		if(strcmp(dshlines[n].argv[dshlines[n].argc-1],"$@")==0) {
			free(dshlines[n].argv[dshlines[n].argc-1]);
			asprintf(&dshlines[n].argv[dshlines[n].argc-1],
				 "%s",rname);
		}
	}
	
	fprintf(stderr," -> ");

	for(j=0;dshlines[n].argv[j];j++)
		printf("%s ", 
		       dshlines[n].argv[j]);
	printf("\n");

	sd=new SharedData;
	pid = fork ();
	if (pid == 0) { /* This is the child process. Execute the
			 * shell command. */
		execvp (dshlines[n].argv[0],dshlines[n].argv);
		_exit (EXIT_FAILURE);
	}
	else if (pid < 0) /* The fork failed. Report failure. */
		status = -1;
	else { /* This is the parent process. */ 
		/* First of all: get a list of matching sds */
		oid=sd->record_state(&noid);
		delete sd;
		/* Wait for the child to complete. */
		if (waitpid (pid, &status, 0) != pid)
			status = -1;
		sd=new SharedData;
		nid=sd->what_s_new(oid,noid,&nnid);
		if(nnid!=1){
			for(i=0;dshlines[n].argv[i];i++)
				    fprintf(stderr,"%s ",dshlines[n].argv[i]);
			fprintf(stderr,"\n");
			fprintf(stderr,"command does not lead to exactly"
				" one but %d new data sets\n",nnid);
		} else
			newid=nid[0];
		if(nnid)
			free(nid);
		if(noid)
			free(oid);
		delete sd;
	}
	return newid;
}

int load(const char* name)
{
	char *rname=NULL;
	char *load_name=NULL;
	char *dir=NULL;
	char *verbase=NULL;
	char *file_name=NULL;
	char *dsh_file=NULL;
	DshLine *dshlines=NULL;
	int *rdshl=NULL;
	int ver;
	int ndshl=0;
	int nrdshl=0;
	int i;
	int load_errno;

	int newid;

	if(parse_full_name(name,&rname,&verbase,&ver)) {
		fprintf(stderr,"use the new dsl program"
			" for loading \"%s\"!\n",
			name);
		load_errno=1;
		goto quit;
	}
	if(!(dir=look_for_source(rname))) {
		load_errno=2; /* There is no source file named
			       * source_file! */
		goto quit;
	}
	asprintf(&file_name,"%s/%s",dir,rname);
	if(!(dsh_file=check_dsh_file(dir,rname))) {
		load_errno=3; /* A dsh file corresponding to the file
			       * file_name cannot be found and even
			       * not be created.
			       */
		goto quit;
	}
	dshlines=parse_dsh_file(&ndshl,dsh_file);
	{
		if(!(rdshl=fetch_relevant_lines(&nrdshl,verbase,
						ver,dshlines,ndshl))) {
			load_errno=4; /* The requested version is not
				       * described by the dsh file */
			goto quit;
		}
//  		for(j=0;dshlines[rdshl[0]].argv[j];j++)
//  			printf("%s ", 
//  			       dshlines[rdshl[0]].argv[j]);
//  		printf("\n");
		if((newid=exec_load_command(dshlines,rdshl[0],rname))==
		   DS_UNUSED) {
			fprintf(stderr,"loading \"%s\" not succesfull\n",
				name);
			return newid;
		}
		
		i=1;

		for(;i<=ver;i++) { /* Follow the instruction in the
				    * dsh file: */
//  			for(j=0;dshlines[rdshl[i]].argv[j];j++)
//  				printf("%s ", dshlines[rdshl[i]].argv[j]);
//  			printf("\n");
			exec_command(dshlines,rdshl[i],newid);
		}
	}

 quit:
	if(dir)
		free(dir);
	if(file_name)
		free(file_name);
	if(rname)
		free(rname);
	if(load_name)
		free(load_name);
	if(verbase)
		free(verbase);
	if(dsh_file)
		free(dsh_file);
	free_dsh_line(dshlines,ndshl);
	if(rdshl)
		free(rdshl);

	printf("new id=%d\n",newid);
	return newid;
}


int main(int argc, char *argv[])
{
	int i;

	for(i=1;i<argc;i++)
		load(argv[i]);

	return 0;
}
