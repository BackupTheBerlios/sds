%{

#include <math.h>
#include "y.tab.h"

%}

%option noyywrap
%option nounput

%%

i[ \t]*= {return (T_MIN);}
[x]?min[ \t]*= {return (T_MIN);}
[x]?minimum[ \t]*= {return (T_MIN);}

a[ \t]*= {return (T_MAX);}
[x]?max[ \t]*= {return (T_MAX);}
[x]?maximum[ \t]*= {return (T_MAX);}

n[ \t]*= {return (T_NOP);}
NoP[ \t]*= {return (T_NOP);}
Number[ \t]+of[ \t]+Points[ \t]*= {return (T_NOP);}

rms[ \t]*= {return (T_RMS); }
RMS[ \t]*= {return (T_RMS); }
pc[ \t]*= {return (T_PC); }
Pc[ \t]*= {return (T_PC); }
pspan[ \t]*= {return (T_PSPAN); }
Pspan[ \t]*= {return (T_PSPAN); }

rms2[ \t]*= {return (T_RMS2); }
RMS2[ \t]*= {return (T_RMS2); }
pc2[ \t]*= {return (T_PC2); }
Pc2[ \t]*= {return (T_PC2); }

swap[ \t]ucf {return (T_SWAP); }
swap_ucf     {return (T_SWAP); }


ms[ \t]*= {return (T_MS); }

slope_order[ \t]*= {return (T_SLOR); }
so[ \t]*= {return (T_SLOR); }

tc[ \t]*= {return (T_TC); }

f(i)?lt(er)?(_type)?[ \t]*= {return (T_FLT); }

ok {return (T_OK);}
accept {return (T_OK);}

tr {return (T_TR);}
tog(gle)?_?res(iduum)? {return (T_TR);}

res(iduum)? {return (T_RES);}

rep(lace)? {return (T_REP);}

abort {return (T_ABORT);}

iopt[ \t]*= {return (T_IOPT);}
opt[ \t]*= {return (T_IOPT);}

rec(tangle)?   { dscfl_yylval.ival=1 ; return(T_FLTY); }
exp(onential)? { dscfl_yylval.ival=0 ; return(T_FLTY); }

[+-]?[0-9]+ {
               sscanf(yytext,"%d",&dscfl_yylval.ival); 
               return(T_INT); }

[+-]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][+-]?[0-9]{1,3})? {
               sscanf(yytext,"%lf",&dscfl_yylval.fval); 
               return(T_DOUBLE); }

[[:blank:]]*
";" { return ';' ; }

