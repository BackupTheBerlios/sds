#include <SharedData.h>

#define DELETAG 0x1
#define CREATAG 0x2

typedef struct {
	idset *modset;
	idset *newset;
	idset *delset;
} opidset;

typedef struct {
	opidset *opids;
	idset *depids;
} tagset;


opidset *create_opidset(idset* mod, idset* news, idset* del);

tagset *create_tag(opidset *ops, idset *deps);
/* creates a tagset with opset ops, depset dep */

void free_opidset(opidset *ops);

void free_tag(tagset *tag);

