#ifndef _DSH_Y_L_H
#define _DSH_Y_L_H

#define COM_DSLASH	1
#define COM_HASH	2

#define DSHL_DELETE  0x01
#define DSHL_DELETED 0x01
#define DSHL_SHOW    0x02
#define DSHL_SHOWED  0x04

struct Comment{
	int type;
	char *token;
};

struct DshLine{
	char *verbase;
	int ver;
	char **argv;
	int argc;
	struct Comment **comv;
	int comc;
	struct Version **dependv;
	int dependc;
	int level;
	char flag;
};

struct Version{
	char *source;
	char *verbase;
	int ver;
};

#endif // _DSH_Y_L_H
