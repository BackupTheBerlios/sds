#include <SharedData.h>

typedef struct  { 
	int n;
	int *ids;
} idset;


idset *init_set(int size);
/* creades idset and allocates size ids */ 

idset *add_to_set(idset *set, int i, int m);
/* adds ids from i to i+m to the idset, creating it if necessary */

idset *join_sets(idset *aset, idset *bset);
/* creates new idset and cats the ids of aset and bset to it*/

void append_set(idset **aset, idset *bset);
/* appends the ids of bset to the ids of aset, creating aset if necessary*/

void free_idset(idset *set);

int compare_ids (const void *a, const void *b);
/* needed for sorting */

void sort_set(idset *set);
/* sorts the ids of set and throws out repeated ids */

int match_sets(idset *aset, idset *bset);
/* returns how many elements of bset->ids are in aset->ids */

int find_in_set(idset *set, int id);
/* returns an id corresponding to the position of id in set->ids,
   whith a return value of 1 meaning id==set->ids[0], and 0 meaning 
   no match.
*/

idset *transpose_set(idset *index, idset* set);
/* returns the idset filled with the positions of the set->ids[] in 
   index->ids.
*/

idset *exclude_sets(idset *aset, idset *bset);
/* returns an idset with all ids that are in aset->ids but not in bset->ids */

void string_set(char **out,idset *set);
void print_set(FILE *out,idset *set);

int exec_command(char **command, SharedData **);

inline char *eatupspace(char *buf);

inline char *eatupnospace(char *buf); 

char *parsecomment(char *line);

int parsel(const char *line,double *tx,double *ty,int xcol,int ycol);


