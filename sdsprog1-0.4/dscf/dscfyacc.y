%{
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>
#include "dscf_y_l.h"


struct DscfParm yyparam={0,   /* set         */
			 0.0, /* min         */
			 0.0, /* max         */
			 0,   /* nop         */
			 1.1, /* fac         */
			 0.0, /* ftol        */
			 1,   /* iopt        */
			 0,   /* mode        */
			 1.0, /* rms         */
			 1.0, /* pc          */
			 1.0, /* pspan       */
			 0.0, /* rms2        */
			 1.0, /* pc2         */
			 0.0, /* ms          */
			 1,   /* slope_order */
			 0.0, /* tc          */
			 0    /* filter_type */
};


int yyerror(char *s);
int yylex( void );


%}

%token YYERROR_VERBOSE

%union
{
	int ival;
	double fval;
}

%token	T_MIN	T_MAX	T_NOP	T_OK   T_ABORT	 T_IOPT
%token	T_RES   T_REP	T_TR    T_SWAP

%token	T_INT   T_RMS   T_PC    T_PSPAN	 T_RMS2  T_PC2   T_MS	T_SLOR	T_TC
%token  T_FLT   T_FLTY

%token	T_DOUBLE 

%type <ival> T_INT T_FLTY

%type <fval> T_DOUBLE

%%

        line:   exp
                | exp oneexp
                | error
                ; 


	exp:    /* empty */
		| exp oneexp ';'
		;

	oneexp: T_MIN T_DOUBLE  { yyparam.min=$2; yyparam.set|=MIN_SET; }
		| T_MIN T_INT  { yyparam.min=$2; yyparam.set|=MIN_SET; }
		| T_MAX T_DOUBLE  { yyparam.max=$2; yyparam.set|=MAX_SET; }
		| T_MAX T_INT  { yyparam.max=$2; yyparam.set|=MAX_SET; }
		| T_NOP T_INT  { yyparam.nop=$2; yyparam.set|=NOP_SET; }
		| T_IOPT T_INT  { yyparam.iopt=$2; yyparam.set|=OPT_SET; }
                | T_RMS T_DOUBLE  { yyparam.rms=$2; yyparam.set|=RMS_SET; }
		| T_RMS T_INT  { yyparam.rms=$2; yyparam.set|=RMS_SET; }
                | T_PC T_DOUBLE  { yyparam.pc=$2; yyparam.set|=PC_SET; }
		| T_PC T_INT  { yyparam.pc=$2; yyparam.set|=PC_SET; }
                | T_PSPAN T_DOUBLE  { yyparam.pspan=$2; yyparam.set|=PSPAN_SET; }
		| T_PSPAN T_INT  { yyparam.pspan=$2; yyparam.set|=PSPAN_SET; }
		| T_MS T_DOUBLE  { yyparam.ms=$2; yyparam.set|=MS_SET; }
		| T_MS T_INT  { yyparam.ms=$2; yyparam.set|=MS_SET; }
		| T_RMS2 T_DOUBLE  { yyparam.rms2=$2; yyparam.set|=RMS2_SET; }
		| T_RMS2 T_INT  { yyparam.rms2=$2; yyparam.set|=RMS2_SET; }
		| T_PC2 T_DOUBLE  { yyparam.pc2=$2; yyparam.set|=PC2_SET; }
		| T_PC2 T_INT  { yyparam.pc2=$2; yyparam.set|=PC2_SET; }
		| T_SLOR T_INT  { yyparam.slor=$2; yyparam.set|=SLOR_SET; }
		| T_TC T_DOUBLE  { yyparam.tc=$2; yyparam.set|=TC_SET; }
		| T_TC T_INT  { yyparam.tc=$2; yyparam.set|=TC_SET; }
                | T_FLT T_FLTY { yyparam.flty=$2; yyparam.set|=FLTY_SET;}
                | T_SWAP { yyparam.set|=SWAP_SET; }
		| T_TR  { yyparam.set|=TR_SET; }
                | T_OK     { yyparam.mode|=ACCEPT_MODE; }
		| T_ABORT  { yyparam.mode|=ABORT_MODE; }
                | T_RES    { yyparam.mode|=RES_MODE; }
                | T_REP    { yyparam.mode|=REP_MODE; }
		;

%%

int yyerror(char *s)
{
  printf("error: %s\n",s);
  return 0;
}

