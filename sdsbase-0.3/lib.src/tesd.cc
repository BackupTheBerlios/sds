#include <SharedData.h>
#include <stdlib.h>

void usage(void) {
	printf(" Usage: tesd create #n or\n"
	       "        tesd delete id or\n"
	       "        tesd show   id or\n"
	       "        tesd resize id #n or\n"
	       "        tesd annotate id string or\n"
	       "        tesd togglelog or\n"
	       "        tesd log string or\n"
	       "        tesd what_s_new #n\n"
		);
	exit(1);
}

int main(int argc, char *argv[])
{
	SharedData hallo;
	double *x;
	double *y;
	int i,j,k,id;
	int *rst,nrst;
	char **ident,*item;

	if(argc<2||argc>4)
		usage();

	switch(*argv[1]) {
	case 'c':
		i=strtol(argv[2],NULL,0);
		id=hallo.create(i);
		x=hallo.x(id);
		y=hallo.y(id);
		for(j=0;j<i;j++) {
			x[j]=j;
			y[j]=i-j;
		}
		hallo.set_n(id,i);
		printf("id=%d\n",id);
		return 0;
	case 'd':
		id=strtol(argv[2],NULL,0);
		i=hallo.remove(id);
		printf("id=%d removed (%d)\n",id,i);
		return 0;
	case 's':
		id=strtol(argv[2],NULL,0);
		x=hallo.x(id);
		y=hallo.y(id);
		i=hallo.n(id);
		for(k=0;k<i;k++)
			printf("%d %f %f\n", k, x[k], y[k]);
		ident=hallo.ident(id,&i);
		if(ident) {
			for(item=ident[k=0];item;item=ident[++k])
				printf("ident[%d]=\"%s\"\n",k,item);
			printf("total number of items: found: %d,"
			       " printed:%d\n",
			       i,k);
		}
		return 0;
	case 'r':
		id=strtol(argv[2],NULL,0);
		i=strtol(argv[3],NULL,0);
		j=hallo.n(id);
		hallo.resize(id,i);
		x=hallo.x(id);
		y=hallo.y(id);
		for(k=j;k<i;k++) {
			x[k]=k-j;
			y[k]=i-k;
		}
		hallo.set_n(id,i);
		return 0;
	case 'a':
		id=strtol(argv[2],NULL,0);
		hallo.add_ident(id,argv[3]);
		return 0;
	case 't':
		printf("logstate was %d ",
		       hallo.logstate(TOGGLELOG));
		printf(" and is now %d\n",
		       hallo.logstate(TELLLOG));
		return 0;
	case 'l':
		hallo.log(argv[2]);
		return 0;
	case 'w':
		i=strtol(argv[2],NULL,0);
		j=strtol(argv[3],NULL,0);
		rst=hallo.record_state(&nrst);
		for(k=0;k<j;k++) {
			id=hallo.create(i);
			printf("id=%d\n",id);
		}
		rst=hallo.what_s_new(rst,nrst,&nrst);
		printf("new ids: ");
		for(i=0;i<nrst;i++)
			printf("%d ",rst[i]);
		printf("\n");
		return 0;
	default:
		usage();
	}
	return 0;
}

