#ifndef _DSSP_Y_L_H
#define _DSSP_Y_L_H

#define MIN_SET    1<<0
#define MAX_SET    1<<1
#define NOP_SET    1<<2
#define SFA_SET    1<<3
#define ORD_SET    1<<4
#define INT_SET    1<<5
#define OPT_SET    1<<6
#define SIG_SET    1<<7
#define RES_SET    1<<8
#define REP_SET    1<<9
#define TR_SET     1<<10

#define INTERAC_MODE 1<<0
#define ABORT_MODE   1<<1
#define ACCEPT_MODE  1<<2
#define REP_MODE     1<<3
#define RES_MODE     1<<4

struct DssplParm {
	int set;
	double min;
	double max;
	int nop;
	double sfac;
	int order;
	int iopt;
	int mode;
};

struct SigmaPoint {
	double x;
	double r;
	double e;
	double l;
};

#endif /* DSSP_Y_L_H */
