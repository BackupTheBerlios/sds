#include <SharedData.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <malloc.h>
#include <ctype.h>
#include <sys/wait.h>
#include <search.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

SharedData *_sd=NULL;

void fatal(const char *templ, ...)
/* abortion of program execution printing information like
 * fprintf(stderr,templ,..) and reporting error code if errno is set */
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

int asprintcat(char **dest, int *dl, int off, const char *fstr, ...)
/* *dest has to point to a malloced space of *dl length (or *dl has to
 * be zero); (fstr, ...) is printed at like
 * asprintf(*dest+off,fstr,...) thereby increasing the malloced space
 * at *dest when needed via realloc. */
{
	char *tmp;
	size_t len;
	va_list ap;

	va_start(ap,fstr);
	vasprintf(&tmp,fstr,ap);
	va_end(ap);
	
	len = strlen(tmp);

	errno=0;
	if (off + len + 1 >(unsigned int) *dl) {
		*dl = 100 + off + len;
		*dest = (char *) realloc (*dest, *dl);
		if(!*dest)
			fatal("realloc error in asprintcat");
	}
	memcpy(*dest+off,tmp,len);
	*(*dest+off+len)='\0';
	free(tmp);
	return len;
}

void *xmalloc(size_t bytes)
{
	errno=0;
	register void *val=malloc(bytes);

	if(val==NULL)
		fatal("Virtual Memory exhausted");
	return val;
}

void * xrealloc (void *ptr, size_t size)
/* realloc with exit when running out of memory */
{
        register void *value = realloc (ptr, size);
        if (value == 0)
                fatal ("Virtual memory exhausted");
        return value;
}

int blanks_in(char *token)
/* checks whether token contains white space characters */
{
	char *tmp;
	for(tmp=token;*tmp;tmp++)
		if(isspace(*tmp))
			return 1;
	return 0;
}

static int compare_pl_id (const void *a, const void *b) {
	const int *ida = (const int *) a;
	const int *idb = (const int *) b;
	int pla=_sd->plev(*ida);
	int plb=_sd->plev(*idb);

	if (pla!=plb)
		return (pla > plb) - (pla < plb);
	else
		return (*ida > *idb) - (*ida < *idb);
}

void sort_pl_id(SharedData* sd, int **ids, int n)
/* sorts the ids, plot level first  */
{
	if (n<2)
		return;

	_sd=sd;
	
	qsort(*ids,n,sizeof(int),compare_pl_id);

	_sd=NULL;
	
}



idset *init_set(int size)
/* creades idset and allocates size ids */ 
{
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

idset *add_to_set(idset *set, int i, int m)
/* adds ids from i to i+m to the idset, creating it if necessary */
{
	int j;
	if (m<0) { 
		m *= -1; 
		i -= m; 
	}
	m++;
	if (set) {
		set->ids = (int *) realloc(set->ids,
					   (set->n+m)*sizeof(int));
		for (j=0;j<m;j++) 
			set->ids[set->n+j] = i + (int) j;
		set->n += m;
		return set; 
	} else {
		idset *tmpset;
		tmpset = (idset *) malloc(sizeof(idset));
		tmpset->ids = (int *) malloc(m*sizeof(int));
		for (j=0;j<m;j++)
			tmpset->ids[j] = i + (int) j;
		tmpset->n = m;
		return tmpset;
	}
}

idset *join_sets(idset *aset, idset *bset)
/* creates new idset and cats the ids of aset and bset to it */
{
	int i;
	idset *ret=NULL;

	if (aset) {
		ret = (idset *)  malloc(sizeof(idset));
		ret->ids =  (int *) malloc(aset->n*sizeof(int));
		for (i=0;i<aset->n;i++) 
			ret->ids[i]=aset->ids[i];
		ret->n = aset->n;
		if (bset) {
			ret->ids = (int *) realloc(ret->ids,
						   (aset->n+bset->n)*
						   sizeof(int));
			for (i=0;i<bset->n;i++) 
				ret->ids[aset->n+i]=bset->ids[i];
			ret->n += bset->n;
		}
	} else {
		if (bset) {
			ret =  (idset *) malloc(sizeof(idset));
			ret->ids =  (int *) malloc(bset->n*sizeof(int));
			for (i=0;i<bset->n;i++)
				ret->ids[i]=bset->ids[i];
			ret->n = bset->n;
		}
	}
	return ret;
}

void append_set(idset **aset, idset *bset)
/* appends the ids of bset to the ids of aset, creating aset if necessary */
{
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

void free_idset(idset *set)
{
	if (!set) return;
	if (set->ids) free(set->ids);
	free(set);
	return;
}

static int compare_ids (const void *a, const void *b) {
	const int *da = (const int *) a;
	const int *db = (const int *) b;
	
	return (*da > *db) - (*da < *db);
}

void sort_set(idset *set)
/* sorts the ids of set and throws out repeated ids */
{
	int i,j;
	
	if ((!set)||(set->n<2))
		return;
	qsort(set->ids,set->n,sizeof(int),compare_ids);
	
	for (i=1,j=0;i<set->n;i++) {
		if (set->ids[i-1]==set->ids[i])
			j++;
		else 
			if (j) 
				set->ids[i-j]=set->ids[i];
	}
	if (j)
		set->n -= j;
	
}

int match_sets(idset *aset, idset *bset) 
/* returns how many elements of bset->ids are in aset->ids */
{
	int i,ret=0;
	if (!(aset&&bset))
		return 0;
	for (i=0;i<bset->n;i++)
		if (lfind(bset->ids+i,aset->ids,(size_t *) &(aset->n),
			  sizeof(int), compare_ids))
			ret++;
	return ret;
}

int find_in_set(idset *set, int id)
/* returns an id corresponding to the position of id in set->ids,
 * whith a return value of 1 meaning id==set->ids[0], and 0 meaning no
 * match. */
{
	int ret;
	int *pos;
	if (!set)
		return 0;
	pos = (int *) lfind(&id,set->ids,(size_t *) &(set->n),
			    sizeof(int),  compare_ids);
	if (!pos) 
		return 0;
	ret = (int) (pos - set->ids + 1);
	return ret;
}

idset *transpose_set(idset *index, idset *set)
/* returns the idset filled with the positions of the set->ids[] in
 * index->ids.  */
{
	int i;
	int id=0;
	idset *ret=NULL;
	if ((!set)||(!index))
		return NULL;
	for (i=0;i<set->n;i++) {
		id = find_in_set(index,set->ids[i]);
		if (id)
			ret=add_to_set(ret,id,0);
	}
	return ret;
}

idset *exclude_sets(idset *aset, idset *bset) 
/* returns an idset with all ids that are in aset->ids but not in
 * bset->ids */
{
	int i,j;
	idset *tmp;
	if (!(aset&&bset)) return NULL;
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
		    
char *string_set(idset *set)
{
	int i,j;
	int off=0,ll=0;
	char *out=NULL;

	if ((!set)||(set->n==0))
		return NULL;

	off=asprintcat(&out,&ll,0,"%d",set->ids[0]);


	for (i=1,j=0;i<set->n;i++) {
		if (set->ids[i]==(set->ids[i-1]+1))
			j++;
		else {
			if (j) {
				if (j>1)
					off+=asprintcat(&out,&ll,off,
							"-%d",set->ids[i-1]);
				else 
					off+=asprintcat(&out,&ll,off,",%d",
							set->ids[i-1]);
				j=0;
			}
			off+=asprintcat(&out,&ll,off,",%d",set->ids[i]);
		}
	}
	if (j) {
		if (j>1) 
			off+=asprintcat(&out,&ll,off,"-%d",set->ids[set->n-1]);
		else 
			off+=asprintcat(&out,&ll,off,",%d",set->ids[set->n-1]);
	}
	return out;
}


void print_set(FILE *out,idset *set) 
{
	int i,j;

	if ((!set)||(set->n==0))
		return;
	if (set->n==1) {
		fprintf(out,"%d",set->ids[0]);
		return;
	}
	fprintf(out,"%d",set->ids[0]);
	for (i=1,j=0;i<set->n;i++) {
		if (set->ids[i]==(set->ids[i-1]+1))
			j++;
		else {
			if (j) {
				if (j>1)
					fprintf(out,"-%d",set->ids[i-1]);
				else 
					fprintf(out,",%d",set->ids[i-1]);
				j=0;
			}
			fprintf(out,",%d",set->ids[i]);
		}
	}
	if (j) {
		if (j>1)
			fprintf(out,"-%d",set->ids[set->n-1]);
		else 
			fprintf(out,",%d",set->ids[set->n-1]);
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

static inline char *eatupspace(char *buf) 
/* returns first non white space charater in buf */
{
	char *tmp=buf;
	while(*tmp&&isspace((int)*tmp)) tmp++;
	return tmp;
}

static inline char *eatupnospace(char *buf) 
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

static char *parsecomment(char *line)
/* strips comments from an input stream
 * comments looks like:
 * i)  '/' followed by '/' to end of line
 * ii) '/' followed by '*' to occurence of '*' followed by  '/' which 
 *     could occure in any argument to a following call of parsecomment 
 */
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
