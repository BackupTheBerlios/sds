// This may look like C code, but it is really -*- C++ -*-
#ifndef _SHAREDDATA_h
#define _SHAREDDATA_h

#include <stdio.h>

#ifndef Void_t
#define Void_t      void
#endif

#ifndef INTERNAL_SIZE_T
#define INTERNAL_SIZE_T size_t
#endif

struct malloc_state;
typedef malloc_state *mstate;

struct malloc_chunk;
typedef malloc_chunk* mchunkptr;

#define LOGOFF 0
#define LOGON  1
#define TOGGLELOG 2
#define TELLLOG 3

#define DS_UNUSED -1

class DataSet {
	friend class SharedData;
	int id;
	double *x;		// x-array
	double *y;		// y-array
	unsigned int nu;	// Maximal number of points
	unsigned int nused;	// Actual number of points
	char linestyle;
	unsigned int linecolor;
	unsigned int linewidth;
	char symbolstyle;
	unsigned int symbolcolor;
	unsigned int symbolsize;
	unsigned int plev;	// Plot level
//	unsigned int statef;	// State of the data
	char **ident;		// auxilary information
};

class SharedData {
 public:
	mstate avp;
 private:
	FILE *fp;
 public:
	SharedData(void);
	~SharedData();
	int           hasid(int id);
	double        *x(int id);
	double        *y(int id);
	unsigned int   n(int id);
	unsigned int   nmax(int id);
	unsigned int   linestyle(int id);
	unsigned int   linewidth(int id);
	unsigned int   linecolor(int id);
	unsigned int   symbolstyle(int id);
	unsigned int   symbolsize(int id);
	unsigned int   symbolcolor(int id);

	unsigned int   set_n(int id, unsigned int n);
	unsigned int   set_linestyle(int id, int type);
	unsigned int   set_linewidth(int id, unsigned int w);
	int   set_linecolor(int id, const char* cname);
	unsigned int   set_symbolstyle(int id, int type);
	unsigned int   set_symbolsize(int id, unsigned int s);
	int   set_symbolcolor(int id, const char* cname);
	
	int            plev(int id);
	int            set_plev(int id,int n);
	
	char         **ident(int id, int *n);
	int            add_ident(int id, const char *fstr,...);
	int            set_ident(int id, const char **annos);

	int            create(int size);
	int            newid(int id);
	int            resize(int id,int size);
	int            remove(int id);

	int            log(const char* message);
	int            logstate(int on_off=2);

        DataSet       *sds(int *n);
	int           *record_state(int *n);
	int           *what_s_new(int *old,int oldn, int *newn);

	int           changed(unsigned int *state=NULL);
 private:
	void    format(const char *);
	int idtosdsi(int id);
	int set_ident(DataSet *,const char **);
	int add_ident(DataSet *,const char *);
	int delete_ident(DataSet *);
 public:
	void    mstats();
	int     mallopt(int, int);
	struct mallinfo mallinfo(void);
 private:
	Void_t *xmalloc(size_t);
	Void_t *malloc(size_t);
	void    free(Void_t*);
	Void_t *xrealloc(Void_t*, size_t);
	Void_t *realloc(Void_t*, size_t);
	Void_t *calloc(size_t, size_t);
	int     mtrim(size_t);
	size_t  musable(Void_t*);
	Void_t *sysmalloc(INTERNAL_SIZE_T nb);
	void    malloc_consolidate();
	void    malloc_init_state(mstate av);
	void   *morecore(int size);
#if DEBUG
	void do_check_chunk(mchunkptr p);
	void do_check_free_chunk(mchunkptr p);
	void do_check_inuse_chunk(mchunkptr p);
	void do_check_remalloced_chunk(mchunkptr p, INTERNAL_SIZE_T s);
	void do_check_malloced_chunk(mchunkptr p, INTERNAL_SIZE_T s);
	void do_check_malloc_state(void);
#endif

};

void fatal(const char *templ, ...);
/* abortion of program execution printing information as 
 * fprintf(stderr,templ,..) and reporting errno code if any */

int asprintcat(char **dest, int *dl, int off, const char *fstr, ...);
/* *dest has to point to a malloced space of *dl length; (fstr, ...) is
 * printed at like asprintf(*dest+off,fstr,...) thereby increasing the
 * malloced space at *dest when needed via realloc. */

void *xmalloc(size_t);
/* malloc with exit when running out of memory */

void * xrealloc (void *ptr, size_t size);
/* realloc with exit when running out of memory */

int blanks_in(char *token);
/* checks whether token contains white space characters */

typedef struct  { 
	int n;
	int *ids;
} idset;

void sort_pl_id(SharedData* sd, int **ids, int n);
/* sorts the ids, plot level first  */

idset *init_set(int size);
/* creades idset and allocates size ids */ 

idset *add_to_set(idset *set, int i, int m);
/* adds ids from i to i+m to the idset, creating it if necessary */

idset *join_sets(idset *aset, idset *bset);
/* creates new idset and cats the ids of aset and bset to it */

void append_set(idset **aset, idset *bset);
/* appends the ids of bset to the ids of aset, creating aset if
 * necessary */

void free_idset(idset *set);

// static int compare_ids (const void *a, const void *b);
/* needed for sorting */

void sort_set(idset *set);
/* sorts the ids of set and throws out repeated ids */

int match_sets(idset *aset, idset *bset);
/* returns how many elements of bset->ids are in aset->ids */

int find_in_set(idset *set, int id);
/* returns an id corresponding to the position of id in set->ids,
 * whith a return value of 1 meaning id==set->ids[0], and 0 meaning no
 * match. */

idset *transpose_set(idset *index, idset* set);
/* returns the idset filled with the positions of the set->ids[] in
 * index->ids.  */

idset *exclude_sets(idset *aset, idset *bset);
/* returns an idset with all ids that are in aset->ids but not in
 * bset->ids */

char *string_set(idset *set);
void print_set(FILE *out,idset *set);

int exec_command(char **command, SharedData **);

// inline char *eatupspace(char *buf);

// inline char *eatupnospace(char *buf); 

// static char *parsecomment(char *line);


int parsel(const char *line,double *tx,double *ty,int xcol,int ycol);
/* simple line parser to lexically analyze a line for containing
 * doubles in column xcol and ycol. In case of match those doubles are
 * returned in tx and ty respectively. As column separator white space
 * chars are used. */

#endif  // _SHAREDDATA_h
