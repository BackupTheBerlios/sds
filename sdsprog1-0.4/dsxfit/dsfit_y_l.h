#ifndef _DSSP_Y_L_H
#define _DSSP_Y_L_H
#include <stdio.h>

#define SET_0   1<<0
#define SET_1   1<<1
#define SET_2   1<<2
#define SET_3   1<<3
#define SET_4   1<<4
#define SET_5   1<<5
#define SET_6   1<<6
#define SET_7   1<<7
#define SET_8   1<<8
#define SET_9   1<<9
#define SET_10  1<<10
#define SET_11  1<<11
#define SET_12  1<<12
#define SET_13  1<<13
#define SET_14  1<<14
#define SET_15  1<<15
#define SET_16  1<<16
#define SET_17  1<<17
#define SET_18  1<<18
#define SET_19  1<<19
#define SET_20  1<<20
#define SET_21  1<<21
#define SET_22  1<<22
#define SET_23  1<<23
#define SET_24  1<<24
#define SET_25  1<<25

#define MIN_SET  1<<0
#define MAX_SET  1<<1
#define NOP_SET  1<<2
#define FTO_SET  1<<3
#define XTO_SET  1<<4
#define GTO_SET  1<<5
#define ITE_SET  1<<6
#define INT_SET  1<<7
#define IOPT_SET 1<<8
#define SIG_SET  1<<9
#define RES_SET  1<<10
#define REP_SET  1<<11
#define TR_SET   1<<12
#define FNC_SET  1<<13

#define INTERAC_MODE 1<<0
#define ABORT_MODE   1<<1
#define ACCEPT_MODE  1<<2
#define REP_MODE     1<<3
#define RES_MODE     1<<4

struct DsFitParm {
	unsigned int set;
	unsigned int parset;
	double min;
	double max;
	unsigned int nop;
	double ftol;
	double xtol;
	double gtol;
	unsigned int itera;
	int mode;
	double pars[26];
	int fit[26];
        char *func;
};

struct SigmaPoint {
	double x;
	double r;
	double e;
	double l;
};

#endif /* DSSP_Y_L_H */







