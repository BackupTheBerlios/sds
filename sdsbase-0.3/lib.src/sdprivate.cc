#include "SharedData_pub.h"
#include <string.h>

void SharedData::format(const char *name)
{
	int size;
	char hallo[]=IDENTIFIER,h=0;

	fp=fopen(name,"w+x");
	if(fp==NULL)
		return;
	setvbuf(fp,NULL,_IONBF,0);
	fwrite(hallo,sizeof(char),strlen(hallo),fp);
	size=sizeof(malloc_state)+strlen(hallo);
	fwrite(&size,sizeof(int),1,fp);
	fseeko(fp,sizeof(malloc_state)-sizeof(int)-1,SEEK_END);
	fwrite(&h,sizeof(char),1,fp);
	fseeko(fp,0,SEEK_SET);
}

int SharedData::idtosdsi(int id)
{
	static int i=0;
	int j,n=avp->sdsize;
	DataSet *sds=avp->sds;
	for(j=i;i<n;i++) {
		if(id==sds[i].id)
			return i;
	}
	for(i=0;i<j;i++) {
		if(id==sds[i].id)
			return i;
	}
	return DS_UNUSED;
}

int SharedData::set_ident(DataSet *sd,const char **annos)
{
	int i,l;
	const char *cc;
	char *c;
	char **ident=sd->ident;

	if(ident) {
		for(c=ident[i=0];c;c=ident[++i])
			free(c);
		free(ident);
		sd->ident=NULL;
	}
	
	if(annos) {
		for(cc=annos[i=0];cc;cc=annos[++i]);
		ident=sd->ident=(char **)xmalloc(sizeof(char*)*(i+1));
		for(cc=annos[i=0];cc;cc=annos[++i]) {
			l=strlen(cc);
			ident[i]=(char*)xmalloc(sizeof(char)*(l+1));
			strcpy(ident[i],cc);
		}
		ident[i]=NULL;
	}
	return 0;
}

int SharedData::add_ident(DataSet *sd,const char *anno)
{
	int i,l;
	char *c;
	char **ident=sd->ident;

	if(!anno)
		return 0;

	if(ident) {
		for(c=ident[i=0];c;c=ident[++i]);
		ident=sd->ident=(char **)xrealloc(ident,sizeof(char *)*(i+2));
		ident[i+1]=NULL;
	} else {
		ident=sd->ident=(char **)xmalloc(sizeof(char *)*2);
		ident[1]=NULL;
		i=0;
	}
	
	l=strlen(anno);
	ident[i]=(char*)xmalloc(sizeof(char)*(l+1));
	strcpy(ident[i],anno);
	return 0;
}

int SharedData::delete_ident(DataSet *sd) {
	return set_ident(sd,NULL);
}
