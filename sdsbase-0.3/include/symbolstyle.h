#ifndef _SYMBOSTYLE_H
#define _SYMBOSTYLE_H

#define NOSYMBOL      (char)0
#define CLOSEDCIRCLE  (char)1
#define OPENCIRCLE    (char)2
#define CLOSEDSQUARE  (char)3
#define OPENSQUARE    (char)4
#define CLOSEDDIAMOND (char)5
#define OPENDIAMOND   (char)6

extern char *symbolstyle[];
int maxsymbolstyle(void);

#endif /* _SYMBOSTYLE_H */
