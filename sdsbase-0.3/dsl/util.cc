#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <search.h>
#include <string.h>
#include <ctype.h>
#include <SharedData.h>
#include "util.h"

idset *init_set(int size) {
	idset *ret=NULL;
	ret = (idset *) malloc(sizeof(idset));
	if (size) {
		ret->ids = (int *) malloc(size*sizeof(int));
		ret->n = size;
	} else {
		ret->n=0; 
		ret->ids=NULL;
	}
	return ret;
}


idset *add_to_set(idset *set, int i, int m) {
	int j;
	if (m<0) { m *= -1; i -= m; }
	m++;
	if (set) {
		set->ids = (int *) realloc(set->ids,
					      (set->n+m)*sizeof(int));
		for (j=0;j<m;j++) set->ids[set->n+j] = i + (int) j;
		set->n += m;
		return set; 
	} else {
		idset *tmpset;
		tmpset = (idset *) malloc(sizeof(idset));
		tmpset->ids = (int *) malloc(m*sizeof(int));
		for (j=0;j<m;j++) tmpset->ids[j] = i + (int) j;
		tmpset->n = m;
		return tmpset;
	}
}

idset *join_sets(idset *aset, idset *bset) {
	int i;
	idset *ret=NULL;

	if (aset) {
		ret = (idset *)  malloc(sizeof(idset));
		ret->ids =  (int *) malloc(aset->n*sizeof(int));
		for (i=0;i<aset->n;i++) ret->ids[i]=aset->ids[i];
		ret->n = aset->n;
		if (bset) {
			ret->ids = (int *) realloc(ret->ids,
					   (aset->n+bset->n)*sizeof(int));
			for (i=0;i<bset->n;i++) 
				ret->ids[aset->n+i]=bset->ids[i];
			ret->n += bset->n;
		}
	}
	else {
		if (bset) {
			ret =  (idset *) malloc(sizeof(idset));
			ret->ids =  (int *) malloc(bset->n*sizeof(int));
			for (i=0;i<bset->n;i++)	ret->ids[i]=bset->ids[i];
			ret->n = bset->n;
		}
	}
	return ret;
}

void append_set(idset **aset, idset *bset) {
	int i;
	if (!bset) return;

	if (*aset)
		(*aset)->ids= (int *) realloc((*aset)->ids,
				  ((*aset)->n+bset->n)*sizeof(int));
	else {
		(*aset)= (idset *) malloc(sizeof(idset));
		(*aset)->ids= (int *) malloc(bset->n*sizeof(int));
		(*aset)->n=0;
	}
	
	for (i=0;i<bset->n;i++) 
		(*aset)->ids[(*aset)->n+i]=bset->ids[i];
	(*aset)->n+=bset->n;
	
	return;
}



void free_idset(idset *set) {
	if (!set) return;
	if (set->ids) free(set->ids);
	free(set);
	return;
}


int compare_ids (const void *a, const void *b) {
	const int *da = (const int *) a;
	const int *db = (const int *) b;
	
	return (*da > *db) - (*da < *db);
}


void sort_set(idset *set) {
	int i,j;
	
	if (set->n<2) return;
	qsort(set->ids,set->n,sizeof(int),compare_ids);
	
	for (i=1,j=0;i<set->n;i++) {
		if (set->ids[i-1]==set->ids[i])
			j++;
		else 
			if (j) set->ids[i-j]=set->ids[i];
	}
	if (j) set->n -= j;
	
}

int match_sets(idset *aset, idset *bset) {
	int i,ret=0;
	if (!(aset&&bset)) return 0;
	for (i=0;i<bset->n;i++)
		if (lfind(bset->ids+i,aset->ids,(size_t *) &(aset->n),
			  sizeof(int), compare_ids))
			ret++;
	return ret;
}

int find_in_set(idset *set, int id) {
	int ret;
	int *pos;
	pos = (int *) lfind(&id,set->ids,(size_t *) &(set->n),
			       sizeof(int),  compare_ids);
	if (!pos) return 0;
	ret = (int) (pos - set->ids + 1);
	return ret;
}

idset *transpose_set(idset *index, idset *set) {
	int i;
	int id=0;
	idset *ret=NULL;
	for (i=0;i<set->n;i++) {
		id = find_in_set(index,set->ids[i]);
		if (id) ret=add_to_set(ret,id,0);
	}
	return ret;
}
	

idset *exclude_sets(idset *aset, idset *bset) {
	int i,j;
	idset *tmp;
	if (!(aset&&bset)) return 0;
	tmp=(idset *) malloc(sizeof(idset));
	tmp->ids=(int *) malloc(aset->n*sizeof(int));
	tmp->n=aset->n;
	for (i=0,j=0;i<aset->n;i++) 
		if (!(lfind(aset->ids+i,bset->ids,(size_t *) &(bset->n),
			    sizeof(int), compare_ids)))
			tmp->ids[j++]=aset->ids[i];
	tmp->n=j;
	return tmp;
}
		    
void string_set(char **out,idset *set) {
	int i,j;
	int off=0,ll=0;

	if ((!set)||(set->n==0)) return;
	if (set->n==1) {
		asprintf(out,"%d",set->ids[0]);
		return;
	}
	off=asprintcat(out,&ll,0,"%d",set->ids[0]);
	for (i=1,j=0;i<set->n;i++) {
		if (set->ids[i]==(set->ids[i-1]+1)) j++;
		else {
			if (j) {
				if (j>1) off+=asprintcat(out,&ll,off,
							 "-%d",set->ids[i-1]);
				else off+=asprintcat(out,&ll,off,",%d",
						     set->ids[i-1]);
				j=0;
			}
			off+=asprintcat(out,&ll,off,",%d",set->ids[i]);
		}
	}
	if (j) {
		if (j>1) off+=asprintcat(out,&ll,off,"-%d",set->ids[set->n-1]);
		else off+=asprintcat(out,&ll,off,",%d",set->ids[set->n-1]);
	}
	return;
}


void print_set(FILE *out,idset *set) {
	int i,j;

	if ((!set)||(set->n==0)) return;
	if (set->n==1) {
		fprintf(out,"%d",set->ids[0]);
		return;
	}
	fprintf(out,"%d",set->ids[0]);
	for (i=1,j=0;i<set->n;i++) {
		if (set->ids[i]==(set->ids[i-1]+1)) j++;
		else {
			if (j) {
				if (j>1) fprintf(out,"-%d",set->ids[i-1]);
				else fprintf(out,",%d",set->ids[i-1]);
				j=0;
			}
			fprintf(out,",%d",set->ids[i]);
		}
	}
	if (j) {
		if (j>1) fprintf(out,"-%d",set->ids[set->n-1]);
		else fprintf(out,",%d",set->ids[set->n-1]);
	}
	return;
}

int exec_command(char **command, SharedData **hallo)
{
	int status;
	pid_t pid;
	int file;

	pid = fork ();
	if (pid == 0) {
		/* This is the child process.  Execute the shell command. */
		file = TEMP_FAILURE_RETRY(open("/dev/null", O_WRONLY));
		dup2 (file, STDOUT_FILENO);
		TEMP_FAILURE_RETRY (close (file));

		execvp(command[0],command);
		_exit (EXIT_FAILURE);
	}
	else if (pid < 0)
		/* The fork failed.  Report failure.  */
		status = -1;
	else { /* This is the parent process. */ 
		delete *hallo;
		if (waitpid (pid, &status, 0) != pid) /* Wait for the
						       * child to
						       * complete. */
			status = -1;
		*hallo = new SharedData;
	}
	return status;
}

inline char *eatupspace(char *buf) 
/* returns first non white space charater in buf */
{
	char *tmp=buf;
	while(*tmp&&isspace((int)*tmp)) tmp++;
	return tmp;
}

inline char *eatupnospace(char *buf) 
/* returns first white space charater in buf */
{
	char *tmp=buf;
	while(*tmp&&!isspace((int)*tmp)) tmp++;
	return tmp;
}

#define COMMENTSTART '/'
#define COMMENTLINE '/'
#define COMMENTBLOCK '*'
#define COMMENTEND "*/"

char *parsecomment(char *line)
{
  char *tmp,*cm;
  static int cf=0;

  if(cf) {
    tmp=strstr(line, COMMENTEND);
    if (!tmp) {
      *line=0;
      return line;
    } else {
      cf=0;
      return parsecomment(tmp+2);
    }
  }
  
  cm=strchr(line, COMMENTSTART);

  if(cm) {
    if(*(cm+1) == COMMENTLINE)
      *cm=0;
    else if (*(cm+1) == COMMENTBLOCK) {
      cf=1;
      tmp=parsecomment(cm+2);
      memmove(cm,tmp,strlen(tmp)+1);
      parsecomment(cm);
    }
  }
  return line;
}

int parsel(const char *line,double *tx,double *ty,int xcol,int ycol)
/* simple line parser to lexically analyze a line for containing
 * doubles in column xcol and ycol. In case of match those doubles are
 * returned in tx and ty respectively. As column separator white space
 * chars are used. */
{
	char *tmp=parsecomment((char *) line);
	
	double *st,*nd;
	int sm,la,i;

	tmp=eatupspace(tmp);

	if(xcol<ycol) {
		st=tx;
		nd=ty;
		sm=xcol-1;
		la=ycol-1;
	}
	else {
		st=ty;
		nd=tx;
		sm=ycol-1;
		la=xcol-1;
	}
	
	for(i=0;i<sm&&*tmp;i++) {
		tmp=eatupnospace(tmp);
		tmp=eatupspace(tmp);
	}
	
	if(!(*tmp)||sscanf(tmp,"%lf",st)!=1)
		return 0;

	for(;i<la&&*tmp;i++){
		tmp=eatupnospace(tmp);
		tmp=eatupspace(tmp);
	}

	if(!(*tmp)||sscanf(tmp,"%lf",nd)!=1)
		return 0;
	
	return 2;
}
