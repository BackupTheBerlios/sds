/* 
 * $Author: vstein $ 
 * $Revision: 1.1 $
 * $Id: parea.h,v 1.1 2003/01/23 14:50:49 vstein Exp $
*/

#ifndef PAREA_H
#define PAREA_H

#include <qwidget.h>
#include <SharedData.h>

#define ZIN		1<<0
#define ZIN_FAST	1<<1
#define ZOUT		1<<2
#define ZOUT_FAST	1<<3
#define ZTOTAL		1<<4
#define ZOOMX		1<<5
#define ZOOMY		1<<6

#define SLEFT  1<<0
#define SRIGHT 1<<2
#define SUP    1<<3
#define SDOWN  1<<4
#define SLEFT_FAST  1<<5
#define SRIGHT_FAST 1<<6
#define SUP_FAST    1<<7
#define SDOWN_FAST  1<<8

#define BORDERWIDTH 2
#define BOTTOMLINEHEIGHT 20
#define LEFTCOLUMNWIDTH 80

#define LOGZOOMFAC 1.55
#define LOGZOOMFAC_FAST 9

class PArea : public QWidget
{
	Q_OBJECT
 public:
	PArea( QWidget *parent=0, const char *name=0, 
	       unsigned updtime=100, int minlevel=0, int maxlevel=100);
	double minx() {return xmin;}
	double maxx() {return xmax;}
	double miny() {return ymin;}
	double maxy() {return ymax;}
	double lminx() {return lxmin;}
	double lmaxx() {return lxmax;}
	double lminy() {return lymin;}
	double lmaxy() {return lymax;}
	int nx_log() {return nxlog;}
	int ny_log() {return nylog;}
	int plmin() {return minl;}
	int plmax() {return maxl;}
 public slots:
        void zoom(int);
        void shift(int);
	void loga();
	void pspl(char *, char *,
		  char *, char *,
		  char *, char *, 
		  char *);
	void zoomport(double, double, double, double);
        void plevel(int, int);
 protected:
	void paintEvent( QPaintEvent * );
	void mouseMoveEvent( QMouseEvent *e );
	void timerEvent(QTimerEvent *);
 private:
	double xmin,xmax,ymin,ymax;
	double xfac,xoff,yfac,yoff;
	double gxmin,gxmax,gymin,gymax;
	int xlines,ylines;
	double dx,dy;
	int nxlog,nylog;
	double lxmin,lxmax,lymin,lymax;
	double lxfac,lxoff,lyfac,lyoff;
	int lxminpow,lxmaxpow,lxminfac,lxmaxfac;
	int lyminpow,lymaxpow,lyminfac,lymaxfac;
	int lxseries,lyseries;
	int lxf[9],lyf[9];
	int minl,maxl;
	SharedData *sd;

	void total();
	int zx,zy;
	void draw(int id,QPixmap &);
};

#endif // PAREA_H









