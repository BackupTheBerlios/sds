/* 
 * $Author: vstein $ 
 * $Revision: 1.2 $
 * $Id: color.h,v 1.2 2004/02/08 21:07:42 vstein Exp $
*/

#ifndef _COLOR_H
#define _COLOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct RGBColor {
	unsigned char r,g,b;
	char *name;
	RGBColor(double c, double m, double y, double k, const char *_name) {
		r=(unsigned char)((1-c)*(1-k)*255);
		g=(unsigned char)((1-m)*(1-k)*255);
		b=(unsigned char)((1-y)*(1-k)*255);
		asprintf(&name,"%s",_name);
	}
	~RGBColor() { if(name) free(name); };
};

extern RGBColor colors[];
int maxcolor(void);
int color_ind(const char *name);
const char *color_name(int ind);

#endif // _COLOR_H
