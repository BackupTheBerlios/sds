#ifndef _DSSP_Y_L_H
#define _DSSP_Y_L_H


#define MIN_SET   1<<0
#define MAX_SET   1<<1
#define NOP_SET   1<<2
#define FAC_SET   1<<3
#define TOL_SET   1<<4
#define INT_SET   1<<5
#define OPT_SET   1<<6
#define RMS_SET   1<<7
#define PC_SET    1<<8
#define PSPAN_SET 1<<9
#define RES_SET   1<<10
#define REP_SET   1<<11
#define TR_SET    1<<12
#define MS_SET    1<<13
#define SLOR_SET  1<<14
#define TC_SET    1<<15
#define RMS2_SET  1<<16
#define PC2_SET   1<<17
#define SWAP_SET  1<<18
#define FLTY_SET  1<<19

#define INTERAC_MODE 1<<0
#define ABORT_MODE   1<<1
#define ACCEPT_MODE  1<<2
#define REP_MODE     1<<3
#define RES_MODE     1<<4

struct DscfParm {
	int set;
	double min;
	double max;
	int nop;
	double fac;
	int ftol;
	int iopt;
	int mode;
	double rms;
	double pc;
	double pspan;
	double rms2;
	double pc2;
	double ms;
	int slor;
	double tc;
	int flty;
};

#endif /* DSSP_Y_L_H */
