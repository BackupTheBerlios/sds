#include <stdio.h>
#include <g2c.h>
#include <math.h>

extern "C" {
extern int dgam_(doublereal *a, doublereal *x, 
		 real *acc, doublereal *g, 
		 doublereal *gstar, integer *iflg, 
		 integer *iflgst);
extern doublereal dgamit_(doublereal *a, doublereal *x);
}

doublereal gammaq(doublereal a,doublereal x)
{
	return (doublereal)1.0-dgamit_(&a,&x)*(doublereal)pow(x,a);

}


main()
{
	real acc;
	doublereal a,x,g,gstar;
	integer iflg,iflgst;
	
	while(!feof(stdin)) {
		scanf("%lf%lf%f",&a,&x,&acc);
		dgam_(&a,&x,&acc,&g,&gstar,&iflg,&iflgst);
		printf("a=%f x=%f acc=%f\n",a,x,acc);
		printf("gamaq=%g (dgam)  oder %g(dgamit)\n"
		       "gamap=%g (gstar) oder %g(dgamit)\n"
		       "delta=%g oder %g \n",
		       g,gammaq(a,x),gstar,dgamit_(&a,&x),
		       gammaq(a,x)-g,dgamit_(&a,&x)-gstar);
		printf("error codes: iflg= %ld, iflgst= %ld\n",iflg,iflgst);
	}

	return 0;
}
