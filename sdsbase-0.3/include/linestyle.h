#ifndef _LINESTYLE_H
#define _LINESTYLE_H

#define NOPEN          (char)0
#define SOLIDLINE      (char)1
#define DASHLINE       (char)2
#define DOTLINE        (char)3
#define DASHDOTLINE    (char)4
#define DASHDOTDOTLINE (char)5

extern char *linestyle[];
int maxlinestyle(void);

#endif /* _LINESTYLE_H */
