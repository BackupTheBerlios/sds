#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>
#include <SharedData.h>
#include "util.h"

opidset *create_opidset(idset* mod, idset* news, idset* del) {
	opidset *ret;
	ret = (opidset *) malloc(sizeof(opidset));
	ret->modset=mod; 
	ret->newset=news; 
	ret->delset=del;
	return ret;
}

tagset *create_tag(opidset *ops, idset *deps) {
	tagset *tmp;
	tmp=(tagset *)malloc(sizeof(tagset));
	tmp->opids=ops;
	tmp->depids=deps;
	return tmp;
}


void free_opidset(opidset *ops) {
	if (!ops) return;
	if (ops->modset) free_idset(ops->modset);
	if (ops->newset) free_idset(ops->newset);
	if (ops->delset) free_idset(ops->delset);
	free(ops);
	return;
}

void free_tag(tagset *tag) {
	if (!tag) return;
	if (tag->opids) free_opidset(tag->opids);
	if (tag->depids) free_idset(tag->depids);
	free(tag);
	return;
}

