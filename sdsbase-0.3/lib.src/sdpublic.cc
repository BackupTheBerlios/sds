#include "SharedData_pub.h"

#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <color.h>
#include <linestyle.h>
#include <symbolstyle.h>
#include <stdarg.h>

void fatal(const char *templ, ...);

SharedData::SharedData(void)
{
	char *fi;
	char *seg;
	static const char *check=IDENTIFIER;
	int i,size;
	struct flock fl={F_WRLCK,SEEK_SET,0,0};
	int     sdsize=0;
	DataSet *sds;
	const char *cp;
	char cc;

	if(!(fi=getenv(SD_FILE)))
		fatal("No enviromet variable %s\n",SD_FILE);

	errno=0;
	fp=fopen(fi,"r+");

	if(fp==NULL&&errno==ENOENT) {
		format(fi);
		if(fp==NULL)
			fatal("%s:=%s",SD_FILE,fi);
		sdsize=DEFAULT_SDSIZE;
	} else {
		fcntl(fileno(fp),F_SETLKW,&fl);
		setvbuf(fp,NULL,_IONBF,0);
	}

	for(cp=check;*cp;cp++) {
		fread(&cc,sizeof(char),1,fp);
		if(*cp!=cc)
			fatal("%s:=%s has no valied identifier %s",
			      SD_FILE,fi,check);
	}

	fread(&size,sizeof(int),1,fp);

	seg=(char*)mmap((void*)ADDR,size,
			PROT_READ|PROT_WRITE,MAP_FIXED|MAP_SHARED,
			fileno(fp),0);

	if(seg!=(char*)ADDR)
		fatal("%s could not be mapped to 0x%x",fi,ADDR);

	avp=(mstate)(seg+strlen(IDENTIFIER));
	if(sdsize) {
		avp->sdsize=sdsize;
		avp->sds=sds=(DataSet*)xmalloc(sizeof(DataSet)*avp->sdsize);
		avp->logstate=1;
		avp->nid=1;
		avp->state=0;
		for(i=0;i<sdsize;i++) {
			sds[i].id=DS_UNUSED;
		}
	}
}

SharedData::~SharedData()
{
	munmap((void*)ADDR,avp->size);
	if(fp)
		fclose(fp);
}

int SharedData::hasid(int id)
{
	if(idtosdsi(id)==DS_UNUSED)
		return 0;
	return 1;
}

double *SharedData::x(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return NULL;

	return sds[sdsi].x;
}

double *SharedData::y(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return NULL;

	return sds[sdsi].y;
}

unsigned int   SharedData::n(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return 0;
	
	return sds[sdsi].nused;
}

unsigned int   SharedData::nmax(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return 0;
	
	return sds[sdsi].nu;
}

unsigned int  SharedData::linestyle(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return maxlinestyle();
	
	return sds[sdsi].linestyle;
}

unsigned int  SharedData::linewidth(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return 0;
	
	return sds[sdsi].linewidth;
}

unsigned int  SharedData::linecolor(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return maxcolor();
	
	return sds[sdsi].linecolor;
}

unsigned int  SharedData::symbolstyle(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return maxsymbolstyle();
	
	return sds[sdsi].symbolstyle;
}

unsigned int  SharedData::symbolsize(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return 0;
	
	return sds[sdsi].symbolsize;
}

unsigned int  SharedData::symbolcolor(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return maxcolor();
	
	return sds[sdsi].symbolcolor;
}

unsigned int SharedData::set_n(int id,unsigned int n)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return 0;

	sds+=sdsi;

	if(n>sds->nu)
		sds->nused=sds->nu;
	else
		sds->nused=n;

	return sds->nused;
}

unsigned int SharedData::set_linestyle(int id, int type)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return maxlinestyle();

	if(type>=maxlinestyle())
		type=maxlinestyle();
	if(type<0)
		type=0;

	return sds[sdsi].linestyle=type;
}

unsigned int SharedData::set_linewidth(int id, unsigned int w)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return 0;

	return sds[sdsi].linewidth=w;
}

int SharedData::set_linecolor(int id, const char* cname)
{
	int sdsi;
	DataSet *sds=avp->sds;
	int ci=color_ind(cname);
	
	if(ci==(sdsi=maxcolor())||(sdsi=idtosdsi(id))==DS_UNUSED)
		return sdsi;
	
	return sds[sdsi].linecolor=ci;
}

unsigned int SharedData::set_symbolstyle(int id, int type)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return maxlinestyle();

	if(type>=maxsymbolstyle())
		type=maxsymbolstyle();
	if(type<0)
		type=0;

	return sds[sdsi].symbolstyle=type;
}

unsigned int SharedData::set_symbolsize(int id, unsigned int s)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return 0;

	return sds[sdsi].symbolsize=s;
}

int SharedData::set_symbolcolor(int id, const char* cname)
{
	int sdsi;
	DataSet *sds=avp->sds;
	int ci=color_ind(cname);
	
	if(ci==(sdsi=maxcolor())||(sdsi=idtosdsi(id))==DS_UNUSED)
		return sdsi;
	
	return sds[sdsi].symbolcolor=ci;
}

int SharedData::plev(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return DS_UNUSED;
	
	return sds[sdsi].plev;
}

int SharedData::set_plev(int id,int n)
{
	int sdsi,opl;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return DS_UNUSED;
	
	opl= sds[sdsi].plev;
	sds[sdsi].plev=n;

	return opl;
}

DataSet *SharedData::sds(int *n){
	*n=avp->sdsize;
	return avp->sds;
}

char **SharedData::ident(int id, int *n)
{
	int sdsi;
	char *c,**ident;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED)
		return NULL;
	
	ident=sds[sdsi].ident;

	if(ident)
		for(c=ident[*n=0];c;c=ident[++*n]);
	
	return ident;
}

int SharedData::add_ident(int id, const char *fstr, ...)
{
	int sdsi,val;
	char *tmp;
	va_list ap;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED) {
		return DS_UNUSED;
	}

	va_start(ap,fstr);
	vasprintf(&tmp,fstr,ap);
	va_end(ap);

	val=add_ident(sds+sdsi,tmp);
	::free(tmp);

	return val;
}

int SharedData::set_ident(int id, const char **annos)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED) {
		return DS_UNUSED;
	}

	return set_ident(sds+sdsi,annos);
}

int SharedData::create(int size)
{
	DataSet *sds=avp->sds;
	int n=avp->sdsize,i,j;
	for(i=0;i<n;i++) {
		if(sds[i].id==DS_UNUSED)
			break;
	}
	if(i==n){
		n=avp->sdsize+=DEFAULT_SDSIZE;
		sds=avp->sds=(DataSet*)xrealloc(sds,sizeof(DataSet)*n);
		for(j=i+1;j<n;j++) 
			sds[j].id=DS_UNUSED;
	}
	sds[i].id=avp->nid++;
	sds[i].x=(double*)xmalloc(sizeof(double)*size);
	sds[i].y=(double*)xmalloc(sizeof(double)*size);
	sds[i].nu=size;
	sds[i].nused=0;
	sds[i].linestyle=SOLIDLINE;
	sds[i].linecolor=color_ind("Black");
	sds[i].linewidth=0;
	sds[i].symbolstyle=NOSYMBOL;
	sds[i].symbolcolor=color_ind("Black");
	sds[i].symbolsize=5;
	sds[i].plev=0;
	sds[i].ident=NULL;
	return sds[i].id;
}

int SharedData::newid(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED) {
		return DS_UNUSED;
	}
	return sds[sdsi].id=avp->nid++;
}	
       
int SharedData::resize(int id,int size) 
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED) {
		return DS_UNUSED;
	}

	sds[sdsi].x=(double *)xrealloc(sds[sdsi].x,sizeof(double)*size);
	sds[sdsi].y=(double *)xrealloc(sds[sdsi].y,sizeof(double)*size);
	sds[sdsi].nu=size;
	return 0;
}

int SharedData::remove(int id)
{
	int sdsi;
	DataSet *sds=avp->sds;
	
	if((sdsi=idtosdsi(id))==DS_UNUSED) {
		return DS_UNUSED;
	}
	
	delete_ident(sds+sdsi);
	if(sds[sdsi].x) {
		free(sds[sdsi].x);
		sds[sdsi].x=NULL;
	}
	if(sds[sdsi].y) {
		free(sds[sdsi].y);
		sds[sdsi].y=NULL;
	}
	
	sds[sdsi].id=DS_UNUSED;
	return 0;
}


int SharedData::log(const char* message)
{
	char *fi;
	char *tmp;
	FILE *tfp;
	
	if (!message) return 1;

	if(!avp->logstate)
		return 1;

	if(!(fi=getenv(SD_FILE)))
		fatal("No enviromet variable %s\n",SD_FILE);
	asprintf(&tmp,"%s.log",fi);
	tfp=fopen(tmp,"a");
	if(!tfp) {
		fprintf(stderr,"%s could not be opend to append \"%s\"\n",
			tmp,message);
		return 1;
	}
	fprintf(tfp,"%s\n",message);
	::free(tmp);
	fclose(tfp);
	return 0;
}

int SharedData::logstate(int on_off)
{
	int ols=avp->logstate;
	switch(on_off) {
	case LOGOFF:
		avp->logstate=0;
		return ols;
	case LOGON:
		avp->logstate=1;
		return ols;
	case TOGGLELOG:
		avp->logstate=!ols;
		return ols;
	case TELLLOG: 
	default:
		return ols;
	}
}

int *SharedData::record_state(int *n)
{
	int sdsi,j=avp->sdsize;
	DataSet* sds=avp->sds;
	int *val;

	for(sdsi=0,*n=0;sdsi<j;sdsi++)
		if(sds[sdsi].id!=DS_UNUSED)
			(*n)++;

	if(*n==0)
		return NULL;

	val=(int*)::malloc(sizeof(int)**n);

	for(sdsi=0,*n=0;sdsi<j;sdsi++)
		if(sds[sdsi].id!=DS_UNUSED)
			val[(*n)++]=sds[sdsi].id;
	
	return val;
}

int isin(int n,int *ar,int nar)
{
	static int i=0;
	int j;

	j=i;
	for(;i<nar;i++)
		if(n==ar[i])
			return 1;
	for(i=0;i<j;i++)
		if(n==ar[i])
			return 1;
	return 0;
}

int *SharedData::what_s_new(int *old,int oldn, int *newn)
{
	int sdsi,j=avp->sdsize;
	DataSet* sds=avp->sds;
	int *val;

	for(sdsi=0,*newn=0;sdsi<j;sdsi++)
		if(sds[sdsi].id!=DS_UNUSED&&!isin(sds[sdsi].id,old,oldn))
			(*newn)++;

	if(*newn==0)
		return NULL;

	val=(int*)::malloc(sizeof(int)**newn);

	for(sdsi=0,*newn=0;sdsi<j;sdsi++)
		if(sds[sdsi].id!=DS_UNUSED&&!isin(sds[sdsi].id,old,oldn))
			val[(*newn)++]=sds[sdsi].id;
	
	return val;
}

int SharedData::changed(unsigned int *state)
{
	if(state==NULL)
		return ++avp->state;

	if(*state<avp->state) {
		*state=avp->state;
		return 1;
	}

	return 0;
}
