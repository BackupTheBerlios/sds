%{

#include "dsh_y_l.h"
#include <string.h>
#include <ctype.h>
#include "y.tab.h"

static struct Version *version;
static char *text_token;
int line_number;
%}

%x COMMENT

DOUBLE [+-]?([0-9]+\.?[0-9]*|\.[0-9]+)([eE][+-]?[0-9]{1,3})?

%option noyywrap
%option nounput

%%

"/*" {	BEGIN(COMMENT);
	return T_BEG_COM; }

<COMMENT>{
	[^*]* {
		text_token=strdup(yytext);
		dsh_yylval.text=text_token; 
		return T_COMMENT;
	}
	"*"+[^*/]* { 
		text_token=strdup(yytext);
		dsh_yylval.text=text_token; 
		return T_COMMENT; 
	}
	"*"+"/" {
		BEGIN(INITIAL);
		return T_END_COM;
	}
}
"//".*$ {
		text_token=strdup(yytext);
		dsh_yylval.text=text_token; 
		return T_COMMENT_DSLASH; 
		
}
^"#".*$ {
		text_token=strdup(yytext);
		dsh_yylval.text=text_token; 
		return T_COMMENT_HASH; 
}

["].*["] {
	*strrchr(yytext,'"')=0;
	text_token=strdup(yytext+1);
	dsh_yylval.text=text_token; return T_TEXT; }

^":"[[:blank:]]*[^[:space:]]*-[^[:space:]]*\.[0-9]+[[:blank:]]*$ {
	char *tmp;
	version=(struct Version*)malloc(sizeof(struct Version));
	sscanf(strrchr(yytext,'.')+1,"%d",&version->ver);
	*strrchr(yytext,'.')=0;
	version->verbase=strdup(strrchr(yytext,'-')+1);
	*strrchr(yytext,'-')=0;
	tmp=yytext+1;
	while(isblank(*tmp)) 
		tmp++;
	version->source=strdup(tmp);
	dsh_yylval.verp=version; return T_DEPENDENCE; }

^":"[[:blank:]]*[^[:space:]]*[[:blank:]]*$ {
	char *tmp;
	version=(struct Version*)malloc(sizeof(struct Version));
	tmp=yytext+strlen(yytext)-1;
	while(isblank(*tmp)) {
		tmp=0;
		tmp--;
	}
	tmp=yytext+1;
	while(isblank(*tmp)) 
		tmp++;
	version->source=strdup(tmp);
	version->verbase=0;
	version->ver=-1;
	dsh_yylval.verp=version; return T_DEPENDENCE; }

^[[:blank:]]*[^[:space:]]*\.[0-9]+[ \t]+  {
	char *tmp;
	version=(struct Version*)malloc(sizeof(struct Version));
	version->source=NULL;
	sscanf(strrchr(yytext,'.')+1,"%d",&version->ver);
	*strrchr(yytext,'.')=0;
	tmp=yytext;
	while(isblank(*tmp)) 
		tmp++;
	version->verbase=strdup(tmp);
	dsh_yylval.verp=version; return T_VERSION; }

[^[:space:];]+ {
	text_token=strdup(yytext);
	dsh_yylval.text=text_token; return T_TEXT; }


[ \t\r]

"\n" {line_number++;}

";" { return ';'; } 

%%
