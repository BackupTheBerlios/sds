#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <malloc.h>

#define MAXCOL 50

char *progname;
char verstring[]="1.0";

static struct option const long_options[] =
{
  {"h",            no_argument,       0, 'h'},
  {"help",         no_argument,       0, '?'},
  {"output",       required_argument, 0, 'o'},
  {"input",        required_argument, 0, 'i'},
  {"precision",    required_argument, 0, 'p'},
  {"x",            required_argument, 0, 'x'},
  {"y",            required_argument, 0, 'y'},
  {"version",      no_argument,       0, 'v'},
  {"warranty",     no_argument,       0, 'w'},
  {0,              0,                 0, 0  }
};

void warranty(void)
{
	printf("This program is free software; you can redistribute it"
               " and/or modify it\n"
               "under the terms of the GNU General Public License as"
	       " published by the \n"
	       "Free Software Foundation; either version 2 of the License,"
	       " or (at your \n"
	       "option) any later version.\n"
	       "\n"
	       "This program is distributed in the hope that it will be"
	       " useful, but \n"
	       "WITHOUT ANY WARRANTY; without even the implied warranty of"
	       " MERCHANTABILITY\n"
	       "or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General"
	       " Public License \n"
	       "for more details.\n"
	       "\n"
	       "You should have received a copy of the GNU General Public"
	       " License along \n"
	       "with this program; if not, write to the Free Software"
	       " Foundation, Inc., \n"
	       "59 Temple Place Suite 330, Boston, MA 02111-1307 USA.\n");
	exit(0);
}

void usage(void)
{
  fprintf(stderr,"usage: %s [-o file][-x int][-y int]\n\n"

"  -i,    --input         read input from file\n"
"  -o,    --output        place output in file\n"
"  -x,    --x             x values from column #n\n"
"  -y,    --y             y values from column #n\n"
"  -p,    --precision     specify precision of output\n" 
"                         (0: use %%a in format string)\n"
"  -h,-?, --help          this message\n"
"  -v,    --version       display version information and exit.\n"
"  -w,    --warranty      display licensing terms.\n", progname);

  exit(0);
}

void version(void)
{
  fprintf(stderr,"%s version %s\n",progname,verstring);
  
  exit(0);
}

int main(int argc, char **argv)
{
  char *line, *formatstr;
  int max,outx=1,outy=2,bufsize=500,prec=6;
  FILE* out=stdout;
  FILE* in=stdin;
  double arr[MAXCOL];
  char c;

  int parse_line(char *line,double *arr,int outx,int outy);


  progname=strrchr(argv[0], '/');
  if (progname == NULL) progname=argv[0];
  else                  progname++;

  while ((c = getopt_long(argc, argv, "i:o:x:y:p:h?vw",
			  long_options, (int *)0)) != EOF)
  {
    switch (c)
    {
    case 0:   break;
    case 'o': out=fopen(optarg,"w"); 
      if(out==NULL) 
	{
	  printf("error while opening %s for output\n",optarg);
	  usage();
	}
      break;
    case 'i': in=fopen(optarg,"r"); 
      if(in==NULL) 
	{
	  printf("error while opening %s for input\n",optarg);
	  usage();
	}
      break;
    case 'x':
      if(sscanf(optarg,"%d",&outx)!=1)
	{
	  fprintf(stderr,"error at option --x!\n");
	  usage();
	}
      break;
    case 'y':
      if(sscanf(optarg,"%d",&outy)!=1)
	{
	  fprintf(stderr,"error at option --y!\n");
	  usage();
	}
      break;
    case 'p':
      if(sscanf(optarg,"%d",&prec)!=1)
	{
	  fprintf(stderr,"error at option --precision!\n");
	  usage();
	}
      break;
    case 'w': warranty(); break;
    case 'v': version(); break;
    case 'h':
    case '?':
    default: usage(); break;
    }
  }

  line=(char *)malloc(sizeof(char)*bufsize);

  outx-=1;
  outy-=1;
  max=(outx>outy);

  if(max>MAXCOL)
    {
      fprintf(stderr,"column number x or y greater than allowed value %d\n",
	      MAXCOL+1);
      fprintf(stderr,"recompile with increased value for MAXCOL\n");

      usage();
    }

  if(prec==0) {
  	asprintf(&formatstr, "%%a %%a\n");
  } else {
  	asprintf(&formatstr, "%%.%dg %%.%dg\n",prec,prec);
  }

  while(getdelim(&line,&bufsize,'\n',in)>-1)
    {
      if(parse_line(line,arr,outx,outy))
	printf(formatstr,arr[outx],arr[outy]);
      fflush(out);
    }
   return 0;
}

#define COMMENTSTART '/'
#define COMMENTLINE '/'
#define COMMENTBLOCK '*'
#define COMMENTEND "*/"

char *parsecomment(char *line)
{
  char *tmp,*cm;
  static int cf=0;

  if(cf) {
    tmp=strstr(line, COMMENTEND);
    if (!tmp) {
      *line=0;
      return line;
    } else {
      cf=0;
      return parsecomment(tmp+2);
    }
  }
  
  cm=strchr(line, COMMENTSTART);

  if(cm) {
    if(*(cm+1) == COMMENTLINE)
      *cm=0;
    else if (*(cm+1) == COMMENTBLOCK) {
      cf=1;
      tmp=parsecomment(cm+2);
      memmove(cm,tmp,strlen(tmp)+1);
      parsecomment(cm);
    }
  }
  return line;
}

int parse_line(char *line,double *arr,int outx,int outy)
{
  int n;
char *skip_not_space(char *buf);
char *skip_space(char *buf);

 line=parsecomment(line);

  for(n=0;n<MAXCOL;n++)
    {
      line=skip_space(line);
      if(line==NULL)
	{
	  if(n<outx||n<outy)
	    return 0;
	  return 1;
	}
      if(sscanf(line,"%lf",&arr[n])!=1)
	{
	  arr[n]=0;
	  if(n==outx||n==outy)
	    return 0;
	}
      line=skip_not_space(line);
      if(line==NULL)
	{
	  if(n<outx||n<outy)
	    return 0;
	  return 1;
	}
    }
  return 1;
}

char *skip_space(char *buf)
{
  while(*buf!=0 && isspace(*buf))
    buf++;
  if(buf==0)
    return NULL;
  return buf;
}

char *skip_not_space(char *buf)
{
  while(*buf!=0 && !isspace(*buf))
    buf++;
  if(buf==0)
    return NULL;
  return buf;
}
