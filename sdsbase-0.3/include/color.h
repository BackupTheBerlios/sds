/* 
 * $Author: vstein $ 
 * $Revision: 1.1 $
 * $Id: color.h,v 1.1 2003/01/23 14:50:49 vstein Exp $
*/

#ifndef _COLOR_H
#define _COLOR_H

#include <stdio.h>
#include <stdlib.h>

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
