/* 
 * $Author: vstein $ 
 * $Revision: 1.2 $
 * $Id: parea.cc,v 1.2 2004/02/08 21:09:08 vstein Exp $
*/

#include <parea.h>
#include <math.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <color.h>
#include <linestyle.h>
#include <symbolstyle.h>

void grid_param(double &start, double &end, int &lines, double &delta);
void log_grid_param(double min, double max,
		    int *minpow, int *maxpow,
		    int *minfac, int *maxfac,
		    int *series, int fac[]);

PArea::PArea(QWidget * parent, const char *name, 
	     unsigned updtime, int minlevel, int maxlevel)
	:QWidget(parent, name)
{
	zx=ZOOMX;
	zy=ZOOMY;
	nxlog=nylog=1;
	minl=minlevel;
	maxl=maxlevel;
	xmin=xmax=ymin=ymax=0;
	total();
	setMouseTracking(TRUE);
	startTimer(updtime);
	//cout << xmin << " : " << xmax << " , " << ymin << " : " << ymax << "\n";
}

void PArea::total()
{
	int *sdind,indsize;
	int i,n;
	int j=0,pl;
	const double *x,*y;

	sd=new SharedData();
	sdind=sd->record_state(&indsize);

	while(j<indsize && (
		(pl=sd->plev(sdind[j]))<minl ||
		pl>maxl
		)) j++;
	if(j==indsize) {
		delete sd;
		free(sdind);
		if( fabs(xmax-xmin) < 1e2*DBL_MIN) {
			xmin-=DBL_EPSILON;
			xmax+=DBL_EPSILON;
		}
		if( fabs(ymax-ymin) < 1e2*DBL_MIN) {
			ymin-=DBL_EPSILON;
			ymax+=DBL_EPSILON;
		}
		if(xmin<=0||xmax<=0)
			nxlog=2;
		else {
			if(nxlog==2)
				nxlog=1;
			lxmin=xmin;
			lxmax=xmax;
		}
		if(ymin<=0||ymax<=0)
			nylog=2;
		else {
			if(nylog==2)
				nylog=1;
			lymin=ymin;
			lymax=ymax;
		}
		return;
	}
	if(zx&&zy) {
		x=sd->x(sdind[j]);
		y=sd->y(sdind[j]);
		xmin=xmax=x[0];
		ymin=ymax=y[0];
		for(;j<indsize;j++) {
			if((pl=sd->plev(sdind[j]))<minl || pl>maxl) continue;
			x=sd->x(sdind[j]);
			y=sd->y(sdind[j]);
			n=sd->n(sdind[j]);
			for (i = 0; i < n; i++) {
				xmin=( xmin < x[i] ?
				       xmin : x[i]);
				xmax=( xmax > x[i] ? 
				       xmax : x[i]);
				ymin=( ymin < y[i] ? 
				       ymin : y[i]);
				ymax=( ymax > y[i] ? 
				       ymax : y[i]);
			}
		}
	} else if (zx) {
		int fst=1;
		for(;j<indsize;j++) {
			if((pl=sd->plev(sdind[j]))<minl || pl>maxl) continue;
			x=sd->x(sdind[j]);
			y=sd->y(sdind[j]);
			n=sd->n(sdind[j]);
			for (i = 0; i < n; i++) {
				if(y[i]<=ymax&&y[i]>=ymin){
					if(fst) {
						fst=0;
						xmin=xmax=x[i];
					} else {
						xmin=( xmin < x[i] ?
						       xmin : x[i]);
						xmax=( xmax > x[i] ? 
						       xmax : x[i]);
					}
				}
			}
		}
	} else if (zy) {
		int fst=1;
		for(;j<indsize;j++) {
			if((pl=sd->plev(sdind[j]))<minl || pl>maxl) continue;
			x=sd->x(sdind[j]);
			y=sd->y(sdind[j]);
			n=sd->n(sdind[j]);
			for (i = 0; i < n; i++) {
				if(x[i]<=xmax&&x[i]>=xmin){
					if(fst) {
						fst=0;
						ymin=ymax=y[i];
					} else {
						ymin=( ymin < y[i] ?
						       ymin : y[i]);
						ymax=( ymax > y[i] ? 
						       ymax : y[i]);
					}
				}
			}
		}
	}
		
	if( fabs(xmax-xmin) < 1e2*DBL_MIN) {
		xmin-=DBL_EPSILON;
		xmax+=DBL_EPSILON;
	}
	if( fabs(ymax-ymin) < 1e2*DBL_MIN) {
		ymin-=DBL_EPSILON;
		ymax+=DBL_EPSILON;
	}
	if(xmin<=0||xmax<=0)
		nxlog=2;
	else {
		if(nxlog==2)
			nxlog=1;
		lxmin=xmin;
		lxmax=xmax;
	}
	if(ymin<=0||ymax<=0)
		nylog=2;
	else {
		if(nylog==2)
			nylog=1;
		lymin=ymin;
		lymax=ymax;
	}
		
	delete sd;
	free(sdind);
}

void PArea::paintEvent(QPaintEvent *)
{
	QPainter p;
	QPen pen(blue, 0);
	QColor col;
	QBrush brush(SolidPattern);
	int i,j;
	int *sdind,indsize;
	char *splev;
	double yexp,xexp;

	QRect da(0,0,width()-LEFTCOLUMNWIDTH-BORDERWIDTH,
		 height()-BOTTOMLINEHEIGHT-BORDERWIDTH);
	QRect bl(0,height()-BOTTOMLINEHEIGHT,
		 width()-LEFTCOLUMNWIDTH,BOTTOMLINEHEIGHT);
	QRect lc(-LEFTCOLUMNWIDTH,0,LEFTCOLUMNWIDTH,height()-BOTTOMLINEHEIGHT);
	QRect corner(-LEFTCOLUMNWIDTH,height()-BOTTOMLINEHEIGHT,
		     LEFTCOLUMNWIDTH,BOTTOMLINEHEIGHT);
	QPixmap dapix(da.size());
	QPixmap blpix(bl.size());
	QPixmap lcpix(lc.size());
	QPixmap cornerpix(corner.size());
	QString label;

	col.setNamedColor("ivory2");
	dapix.fill(col);
	blpix.fill(this,bl.topLeft());
	lcpix.fill(this,lc.topLeft());
	cornerpix.fill(this,corner.topLeft());

	col.setNamedColor("ivory2");
	dapix.fill(col);
	blpix.fill(this,bl.topLeft());
	lcpix.fill(this,lc.topLeft());
	cornerpix.fill(this,corner.topLeft());
	p.begin(&dapix);
	col.setNamedColor("ivory3");
	pen.setColor(col);
	p.setPen(pen);

	if(nxlog) {
		xfac=(da.width())/(xmax-xmin);
		xoff=-xmin*xfac;
		gxmin=xmin;
		gxmax=xmax;
		xlines=10;
		grid_param(gxmin,gxmax,xlines,dx);
		for(i=0;i<xlines;i++)
			p.drawLine(QPoint((int)((gxmin+i*dx)*xfac+xoff),0),
				   QPoint((int)((gxmin+i*dx)*xfac+xoff),
					  da.height()));
	} else {
		lxfac=da.width()/(log10(lxmax)-log10(lxmin));
		lxoff=-log10(lxmin)*lxfac;
		log_grid_param(lxmin, lxmax,
			      &lxminpow, &lxmaxpow,
			      &lxminfac, &lxmaxfac,
			      &lxseries, lxf);
		for(j=0; lxf[j]!=lxminfac; j++);
		i=lxminpow;
		while(i<lxmaxpow||(i==lxmaxpow&& lxf[j]<=lxmaxfac)) {
			/*
			 * Draw line at i+log10(fac[j]);
			 */
			p.drawLine(QPoint((int)((i+log10(lxf[j]))
						*lxfac+lxoff),
					  0),
				   QPoint((int)((i+log10(lxf[j]))
						*lxfac+lxoff),
					  da.height()));
			if(++j==lxseries) {
				i++;
				j=0;
			}
		}
	}

	if(nylog) {
		yfac=(da.height())/(ymin-ymax);
		yoff=-ymax*yfac;
		gymin=ymin;
		gymax=ymax;
		ylines=10;
		grid_param(gymin,gymax,ylines,dy);
		for(i=0;i<ylines;i++)
			p.drawLine(QPoint(0,(int)((gymin+i*dy)*yfac+yoff)),
				   QPoint(da.width(),
					  (int)((gymin+i*dy)*yfac+yoff)));

	} else {
		lyfac=da.height()/(log10(lymin)-log10(lymax));
		lyoff=-log10(lymax)*lyfac;
		log_grid_param(lymin,lymax,
			      &lyminpow,&lymaxpow,
			      &lyminfac,&lymaxfac,
			      &lyseries,lyf);
		for(j=0; lyf[j]!=lyminfac; j++);
		i=lyminpow;
		while(i<lymaxpow||(i==lymaxpow&&lyf[j]<=lymaxfac)) {
			/*
			 * Draw line at i+log10(fac[j]);
			 */
			p.drawLine(QPoint(0,
					  (int)((i+log10(lyf[j]))
						*lyfac+lyoff)),
				   QPoint(da.width(),
					  (int)((i+log10(lyf[j]))
						*lyfac+lyoff)));
			if(++j==lyseries) {
				i++;
				j=0;
			}
		}
	}
	p.end();

	p.begin(&blpix);
	if(nxlog) {
		xexp=rint(log10(dx));
		for(i=0;i<xlines-1;i++){
			if(fabs(gxmin+i*dx)<dx/2)
				label.sprintf("%g",0.0);
			else {
				if(gxmin/dx<1e5)
					label.sprintf("%g",(gxmin+i*dx)/
								exp10(xexp));
				else
					label.sprintf("%g",i*dx/exp10(xexp));
			}
			p.drawText((int)((gxmin+i*dx)*xfac+xoff)-50,0,
				   100,bl.height(), AlignCenter, label);
		}
		if(gxmin/dx<1e5)
			label.sprintf("1e%d",int(xexp));
		else
			label.sprintf("1e%d+%g",int(xexp),gxmin);
			p.drawText((int)((gxmin+(xlines-1.5)*dx)*xfac+xoff)-50,
				   0, 100,bl.height(), AlignCenter, label);
	} else {
		for(j=0; lxf[j]!=lxminfac; j++);
		i=lxminpow;
		while(i<lxmaxpow||(i==lxmaxpow&lxf[j]<=lxmaxfac)) {
			/* Draw text AT i+log10(fac[j]);
			 *
			 * if(fac[j]==1)
			 *   asprintf(&pl_grid.xstring[pl_grid.xcount],
			 *            "$10^{%d}$",i);
			 * else
			 *   asprintf(&pl_grid.xstring[pl_grid.xcount],
			 *            "$%d$",fac[j]);
			 */
			if(lxf[j]==1)
				label.sprintf("1e%d",i);
			else
				label.sprintf("%d",lxf[j]);
			p.drawText((int)((i+log10(lxf[j]))*lxfac+lxoff)-50,0,
				   100,bl.height(), AlignCenter, label);
			if(++j==lxseries) {
				i++;
				j=0;
			}
		}
	}
	p.end();

	p.begin(&lcpix);
	p.translate(LEFTCOLUMNWIDTH,0);
	if(nylog) {
		yexp=rint(log10(dy));
		for(i=0;i<ylines-1;i++) {
			if(fabs(gymin+i*dy)<dy/2)
				label.sprintf("%g",0.0);
			else {
				if(gymin/dy<1e5)
					label.sprintf("%g",(gymin+i*dy)/
								exp10(yexp));
				else
					label.sprintf("%g",i*dy/exp10(yexp));
			}
			p.drawText(-LEFTCOLUMNWIDTH+5,
				   (int)((gymin+i*dy)*yfac+yoff)-10,
				   LEFTCOLUMNWIDTH-10, BOTTOMLINEHEIGHT, 
				   AlignVCenter|AlignRight, label);
		}
		if(gymin/dy<1e5)
			label.sprintf("1e%d",int(yexp));
		else
			label.sprintf("1e%d\n+%g",int(yexp),gymin);
		p.drawText(-LEFTCOLUMNWIDTH+5,
			   (int)((gymin+(ylines-1.5)*dy)*yfac+yoff)-20,
			   LEFTCOLUMNWIDTH-10, BOTTOMLINEHEIGHT*2, 
			   AlignVCenter|AlignRight, label);
	} else {
		for(j=0; lyf[j]!=lyminfac; j++);
		i=lyminpow;
		while(i<lymaxpow||(i==lymaxpow&&lyf[j]<=lymaxfac)) {
			/* Draw text AT i+log10(fac[j]);
			 *
			 * if(fac[j]==1)
			 *   asprintf(&pl_grid.xstring[pl_grid.xcount],
			 *            "$10^{%d}$",i);
			 * else
			 *   asprintf(&pl_grid.xstring[pl_grid.xcount],
			 *            "$%d$",fac[j]);
			 */
			if(lyf[j]==1)
				label.sprintf("1e%d",i);
			else
				label.sprintf("%d",lyf[j]);
			p.drawText(-LEFTCOLUMNWIDTH+5,
				   (int)((i+log10(lyf[j]))*lyfac+lyoff)-10,
				   LEFTCOLUMNWIDTH-10, BOTTOMLINEHEIGHT, 
				   AlignVCenter|AlignRight, label);
			if(++j==lyseries) {
				i++;
				j=0;
			}
		}
	}
	p.end();

	p.begin(&cornerpix);
	p.translate(LEFTCOLUMNWIDTH,0);
	if(minl==maxl)
		asprintf(&splev,"%d",minl);
	else if(minl<maxl)
		asprintf(&splev,"%d-%d",minl,maxl);
        else
		asprintf(&splev," nol");
	if(zx&&zy)
		label.sprintf("xy%s",splev);
	else if (zx)
		label.sprintf("x %s",splev);
	else if (zy)
		label.sprintf(" y%s",splev);
	else 
		label.sprintf("  %s",splev);
	free(splev);
	p.drawText(-LEFTCOLUMNWIDTH+BORDERWIDTH,0,
		   corner.width(),corner.height(),
		   AlignLeft|AlignVCenter,label);
	if(nxlog) {
		xexp=rint(log10(dx));
		if(fabs(gxmin+i*dx)<dx/2)
			label.sprintf("%g",0.0);
		else {
			if(gxmin/dx<1e5)
				label.sprintf("%g",gxmin/exp10(xexp));
			else
				label.sprintf("%g",0.0);
		}
		p.drawText((int)(gxmin*xfac+xoff)-50,0,100,bl.height(),
		   AlignCenter, label);
	} else {
		for(j=0; lxf[j]!=lxminfac; j++);
		i=lxminpow;
		if(lxf[j]==1)
			label.sprintf("1e%d",i);
		else
			label.sprintf("%d",lxf[j]);

	p.drawText((int)((i+log10(lxf[j]))*lxfac+lxoff)-50,0,100,bl.height(),
		   AlignCenter, label);
	}
	if(nylog) {
		yexp=rint(log10(dy));
		if(fabs(gymin)<dy/2)
			label.sprintf("%g",0.0);
		else {
			if(gymin/dy<1e5)
				label.sprintf("%g",gymin/exp10(yexp));
			else
				label.sprintf("%g",0.0);
		}
		p.drawText(-LEFTCOLUMNWIDTH+5,
			   (int)(gymin*yfac+yoff)-10-da.height()-BORDERWIDTH,
			   LEFTCOLUMNWIDTH-10,
			   BOTTOMLINEHEIGHT, AlignVCenter|AlignRight, label);
	} else {
		for(j=0; lyf[j]!=lyminfac; j++);
		i=lyminpow;
		if(lyf[j]==1)
			label.sprintf("1e%d",i);
		else
			label.sprintf("%d",lyf[j]);
		p.drawText(-LEFTCOLUMNWIDTH+5,
			   (int)((i+log10(lyf[j]))*lyfac+lyoff)
			   -10-da.height()-BORDERWIDTH,
			   LEFTCOLUMNWIDTH-10,
			   BOTTOMLINEHEIGHT, AlignVCenter|AlignRight, label);
	}	
	p.end();

	sd=new SharedData();
	sdind=sd->record_state(&indsize);
	sort_pl_id(sd,&sdind,indsize);
	for(j=0;j<indsize;j++)
		draw(sdind[j],dapix);
	delete sd;
	free(sdind);
	
	p.begin(this);
	p.translate(LEFTCOLUMNWIDTH,BORDERWIDTH);
	p.drawPixmap(da.topLeft(),dapix);
	p.drawPixmap(bl.topLeft(),blpix);
	p.drawPixmap(lc.topLeft(),lcpix);
	p.drawPixmap(corner.topLeft(),cornerpix);
	p.end();
}

void cp(int x, int y, int size,QPainter &p)
{
	p.drawEllipse(x-size,y-size,size*2,size*2);
}

void sp(int x, int y, int size,QPainter &p)
{
	int k2=(int)((double)size*sqrt(M_PI)/2.0);
	p.drawRect(x-k2,y-k2,k2*2,k2*2);
}

void dp(int x, int y, int size,QPainter &p)
{
	QPointArray a;
	int k2=(int)((double)size*sqrt(2*M_PI/sqrt(3.0))/2.0);
	int h=(int)((double)size*sqrt(M_PI*sqrt(3.0)/2.0));
	a.setPoints(4, x-k2,y, x,y+h, x+k2,y, x,y-h); 
	p.drawPolygon(a);
}

void PArea::draw(int id,QPixmap &pixmap)
{
	QPainter p;
	QPen pen(blue, 0);
	QColor col;
	QBrush brush(SolidPattern);
	int j,found;
	double prevx,prevy;
	double nowx,nowy;
	double zx[2],zy[2];
	int prev,now,prev_now;
	double fx,fy,sx,sy;
	double tx,ty;
	const double *x=sd->x(id);
	const double *y=sd->y(id);
	int n=sd->n(id);
	int pl,ci;
	
	if((pl=sd->plev(id))<minl || pl>maxl)
		return;
	p.begin(&pixmap);
	ci=sd->linecolor(id);
	col.setRgb(colors[ci].r,colors[ci].g,colors[ci].b);
	pen.setColor(col);
	switch(sd->linestyle(id)) {
	case NOPEN:
		pen.setStyle(NoPen);
		break;
	case SOLIDLINE:
		pen.setStyle(SolidLine);
		break;
	case DASHLINE:
		pen.setStyle(DashLine);
		break;
	case DOTLINE:
		pen.setStyle(DotLine);
		break;
	case DASHDOTLINE:
		pen.setStyle(DashDotLine);
		break;
	case DASHDOTDOTLINE:
		pen.setStyle(DashDotDotLine);
		break;
	}
	pen.setWidth(sd->linewidth(id));
	p.setPen(pen);
	p.setBrush(NoBrush);

	/*
	 * This is how the positon of points to be drawn relativ to the 
	 * graphics window is coded
	 *                     0x4
	 *            ________________________
         *           |                        |
         *           |                        |
         *           |                        |
         *     0x8   | graphics window == 0x0 |  0x2
         *           |                        |
         *           |                        |
         *           |________________________|
         *                       
         *                       0x1
	 */

 
	j=0;
	if(nxlog)
		prevx=x[j]*xfac+xoff;
	else
		prevx=log10(x[j])*lxfac+lxoff;
	if(nylog)
		prevy=y[j]*yfac+yoff;
	else
		prevy=log10(y[j])*lyfac+lyoff;
	
	if(prevx<0)
		prev=0x8;
	else if(prevx>pixmap.width())
		prev=0x2;
	else
		prev=0x0;
	if(prevy<0)
		prev|=0x4;
	else if(prevy>pixmap.height())
		prev|=0x1;

	if(!prev)  /* if its in, moveto */
		p.moveTo((int)prevx,(int)prevy);
		  
	for(j++;j<n;j++,prev=now,prevx=nowx,prevy=nowy) {
		if(nxlog)
			nowx=x[j]*xfac+xoff;
		else
			nowx=log10(x[j])*lxfac+lxoff;
		if(nylog)
			nowy=y[j]*yfac+yoff;
		else
			nowy=log10(y[j])*lyfac+lyoff;
		  
		  
		if(nowx<0)
			now=0x8;
		else if(nowx>pixmap.width())
			now=0x2;
		else
			now=0x0;
		if(nowy<0)
			now|=0x4;
		else if(nowy>pixmap.height())
			now|=0x1;
		
		if(!(now&prev)) { /* nothing to do if(now&prev) */
			found=0;
			prev_now=prev|now;
			
			if(!now && !prev) { /* both in */
				p.lineTo((int)nowx,(int)nowy);
				continue;
			}

			if(!now) /* This one is in!*/ {
				zx[0]=nowx;
				zy[0]=nowy;
				found++;
			}
			if(!prev) /* previous is in */ {
				zx[0]=prevx;
				zy[0]=prevy;
				found++;
			}
			/* Parameters for straight line y(x)*/
			if(fabs(nowx-prevx)>DBL_MIN*1e20) {
				if(prev_now & 0xa) {
					fy=(nowy-prevy)/(nowx-prevx);
					sy=prevy-prevx*fy;
				}
			}
			else /* nothing to do in 0x1|0x4 */
				prev_now&=0x5;
			/* Parameters for straight line x(y)*/
			if(fabs(nowy-prevy)>DBL_MIN*1e20) {
				if(prev_now & 0x5) {
					fx=(nowx-prevx)/(nowy-prevy);
					sx=prevx-prevy*fx;
				}
			}
			else /* nothing to do in 0x2|0x8 */
				prev_now&=0xa;
			
			if(prev_now & 0x4) {
				if(sx<=pixmap.width() && sx>=0) {
					zx[found]=sx;
					zy[found++]=0;
				}
			}
			if((prev_now & 0x1) && (found<2)) {
				tx=fx*pixmap.height()+sx;
				if(tx<=pixmap.width() && tx>=0) {
					zx[found]=tx;
					zy[found++]=pixmap.height();
				}
			}
			if((prev_now & 0x8) && (found<2)) {
				if(sy<=pixmap.height() && sy>=0) {
					zx[found]=0; 
					zy[found++]=sy;
				}
			}
			if((prev_now & 0x2) && (found<2)) {
				ty=fy*pixmap.width()+sy;
				if(ty<=pixmap.height() && ty>=0) {
					zx[found]=pixmap.width(); 
					zy[found++]=ty;
				}
			}
			if(found==2) {
				p.moveTo((int)zx[1],(int)zy[1]);
				p.lineTo((int)zx[0],(int)zy[0]);
			}
			/* if now is in, the current point is now now.
			 * This is important!
			 */
		}
	}
	if(sd->symbolstyle(id)!=NOSYMBOL) {
		pen.setWidth(0);
		pen.setStyle(SolidLine);
		ci=sd->symbolcolor(id);
		col.setRgb(colors[ci].r,colors[ci].g,colors[ci].b);
		pen.setColor(col);
		p.setPen(pen);
		switch(sd->symbolstyle(id))
		{
		case CLOSEDCIRCLE:
		case CLOSEDSQUARE:
		case CLOSEDDIAMOND:
			brush.setColor(col);
			break;
		case OPENCIRCLE:
		case OPENSQUARE:
		case OPENDIAMOND:
			brush.setColor("ivory2");
			break;
		}
		p.setBrush(brush);

		for(j=0;j<n;j++) {
			if(nxlog)
				nowx=x[j]*xfac+xoff;
			else
				nowx=log10(x[j])*lxfac+lxoff;
			if(nylog)
				nowy=y[j]*yfac+yoff;
			else
				nowy=log10(y[j])*lyfac+lyoff;

			if(nowx>=0 && nowx<=pixmap.width()&&
			   nowy>=0 && nowy<=pixmap.height()) {
				switch(sd->symbolstyle(id)) {
				case CLOSEDCIRCLE:
				case OPENCIRCLE:
					cp((int)nowx, (int)nowy,
					   sd->symbolsize(id), p);
					break;
				case CLOSEDSQUARE:
				case OPENSQUARE:
					sp((int)nowx, (int)nowy,
					   sd->symbolsize(id), p);
					break;
				case CLOSEDDIAMOND:
				case OPENDIAMOND:
					dp((int)nowx, (int)nowy,
					   sd->symbolsize(id), p);
					break;
				}
			}
		}
	}
	p.end();
}

void PArea::zoom(int kind)
{
	switch(kind) {
	case ZIN:
		if(zx) {
			if(nxlog) {
				xmin=gxmin+dx;
				xmax=gxmax-dx;
			} else {
				lxmin*=LOGZOOMFAC;
				lxmax/=LOGZOOMFAC;
			}
		}
		if(zy) {
			if(nylog) {
				ymin=gymin+dy;
				ymax=gymax-dy;
			} else {
				lymin*=LOGZOOMFAC;
				lymax/=LOGZOOMFAC;
			}
		}
		break;
	case ZIN_FAST:
		if(zx) {
			if(nxlog) {
				xmin=gxmin+xlines*dx/4;
				xmax=gxmax-xlines*dx/4;
			} else {
				lxmin*=LOGZOOMFAC_FAST;
				lxmax/=LOGZOOMFAC_FAST;
			}
		}
		if(zy) {
			if(nylog) {
				ymin=gymin+ylines*dy/4;
				ymax=gymax-ylines*dy/4;
			} else {
				lymin*=LOGZOOMFAC_FAST;
				lymax/=LOGZOOMFAC_FAST;
			}
		}
		break;
	case ZOUT:
		if(zx) {
			if(nxlog) {
				xmin=gxmin-dx;
				xmax=gxmax+dx;
			} else {
				lxmin/=LOGZOOMFAC;
				lxmax*=LOGZOOMFAC;
			}
		}
		if(zy) {
			if(nylog) {
				ymin=gymin-dy;
				ymax=gymax+dy;
			} else {
				lymin/=LOGZOOMFAC;
				lymax*=LOGZOOMFAC;
			}
		}
		break;
	case ZOUT_FAST:
		if(zx) {
			if(nxlog) {
				xmin=gxmin-xlines*dx/4;
				xmax=gxmax+xlines*dx/4;
			} else {
				lxmin/=LOGZOOMFAC_FAST;
				lxmax*=LOGZOOMFAC_FAST;
			}
		}
		if(zy) {
			if(nylog) {
				ymin=gymin-ylines*dy/4;
				ymax=gymax+ylines*dy/4;
			} else {
				lymin/=LOGZOOMFAC_FAST;
				lymax*=LOGZOOMFAC_FAST;
			}
		}
		break;
	case ZTOTAL:
		total();
		break;
	case ZOOMX:
		if(zx)
			zx=0;
		else
			zx=ZOOMX;
		break;
	case ZOOMY:
		if(zy)
			zy=0;
		else
			zy=ZOOMY;
		break;
	}
	repaint(FALSE);
}

void PArea::shift(int kind)
{
	switch(kind) {
	case SLEFT:
		if(nxlog) {
			xmin+=dx;
			xmax+=dx;
		} else {
			lxmin*=LOGZOOMFAC;
			lxmax*=LOGZOOMFAC;
		}
		break;
	case SRIGHT:
		if(nxlog) {
			xmin-=dx;
			xmax-=dx;
		} else {
			lxmin/=LOGZOOMFAC;
			lxmax/=LOGZOOMFAC;
		}
		break;
	case SUP:
		if(nylog) {
			ymin-=dy;
			ymax-=dy;
		} else {
			lymin/=LOGZOOMFAC;
			lymax/=LOGZOOMFAC;
		}
		break;
	case SDOWN:
		if(nylog) {
		ymin+=dy;
		ymax+=dy;
		} else {
			lymin*=LOGZOOMFAC;
			lymax*=LOGZOOMFAC;
		}
		break;
	case SLEFT_FAST:
		if(nxlog) {
			xmin+=dx*(xlines-2);
			xmax+=dx*(xlines-2);
		} else {
			lxmin*=LOGZOOMFAC_FAST;
			lxmax*=LOGZOOMFAC_FAST;
		}
		break;
	case SRIGHT_FAST:
		if(nxlog) {
			xmin-=dx*(xlines-2);
			xmax-=dx*(xlines-2);
		} else {
			lxmin/=LOGZOOMFAC_FAST;
			lxmax/=LOGZOOMFAC_FAST;
		}
		break;
	case SUP_FAST:
		if(nylog) {
			ymin-=dy*(ylines-2);
			ymax-=dy*(ylines-2);
		} else {
			lymin/=LOGZOOMFAC_FAST;
			lymax/=LOGZOOMFAC_FAST;
		}
		break;
	case SDOWN_FAST:
		if(nylog) {
		ymin+=dy*(ylines-2);
		ymax+=dy*(ylines-2);
		} else {
			lymin*=LOGZOOMFAC_FAST;
			lymax*=LOGZOOMFAC_FAST;
		}
		break;
	}
	repaint(FALSE);
}

void PArea::loga(void)
{
	if(zx) {
		if(nxlog==1)
			nxlog=0;
		else if(nxlog==0)
			nxlog=1;
	}
	if(zy) {
		if(nylog==1)
			nylog=0;
		else if(nylog==0)
			nylog=1;
	}
	repaint(FALSE);
}

void scale(double min,double max,int *power,double *offset, double *factor)
{
  double diff=max-min,fac=1e-3;

  if(diff<DBL_MIN*1e2)
    {
      fprintf(stderr,"error in scale!\n"
	      "2nd arg(%g) - 1st arg(%g) has to be greater than %g\n"
	      ,max,min,DBL_MIN*1e2);
      *power=0;
      *offset=0.0;
      return;
    }

  *power=0;
  while(diff<1e+1)
    {
      diff*=1e+1;
      fac*=1e+1;
      *power+=1;
    }
  while(diff>=1e+1)
    {
      diff*=1e-1;
      fac*=1e-1;
      *power-=1;
    }

  if(min<0)
    modf(min*pow(10,*power-1),offset);
  else
    modf(max*pow(10,*power-1),offset);
    
  *offset/=pow(10,*power-1);
  *factor=pow(10,*power);
  return;
}

void PArea::zoomport(double xmi,double xma, double ymi, double yma)
{
	if(nxlog) {
		xmin=xmi;
		xmax=xma;
	} else {
		lxmin=xmi;
		lxmax=xma;
	}
	if(nylog) {
		ymin=ymi;
		ymax=yma;
	} else  {
		lymin=ymi;
		lymax=yma;
	}
	repaint(FALSE);
}

void PArea::pspl(char *xs, char *ys,
		 char *xl, char *yl,
		 char *xu, char *yu,
		 char *fn)
{
	double xfactor,yfactor,xoffset,yoffset;
	int power,pl;
	FILE *fp=fopen(fn,"w");

	if(!fp) {
		fprintf(stderr,"\"%s\" cannot be opened for writing\n",fn);
		return;
		
	}
	
	SharedData sd;
	int *sdind,indsize;
	char **identstr;
	int identn;

	int j;
	const double *x,*y;
	int i,max,sst,pst;

	fprintf(fp,"size {\n x: %s\n y: %s\n}\n\n",xs,ys);

	fprintf(fp,"logscale {\n");
	if(!nxlog) fprintf(fp," x\n");
	if(!nylog) fprintf(fp," y\n");
	fprintf(fp,"}\n\n");

	fprintf(fp,"viewport {\n");
	if(nxlog) 
		fprintf(fp," xmin: %g\n xmax: %g\n",xmin,xmax);
	else
		fprintf(fp," xmin: %e\n xmax: %e\n",lxmin,lxmax);
	if(nylog) 
		fprintf(fp," ymin: %g\n ymax: %g\n",ymin,ymax);
	else
		fprintf(fp," ymin: %e\n ymax: %e\n",lymin,lymax);
        fprintf(fp," color: Transparent\n}\n\n");

	fprintf(fp,"grid {\n x: auto\n y: auto\n dx: auto\n dy: auto\n");
	fprintf(fp," adjust viewport x: dx\n adjust viewport y: dy\n");
	fprintf(fp,"# xlines: 10\n# ylines: 10\n# linewidth: 0.125\n");
	fprintf(fp,"# lengthx: 10\n# lengthy: 10\n");
	fprintf(fp," xlabeloffset: 20\n ylabeloffset: 20\n}\n\n");

	fprintf(fp,"label {\n x 13: (\"%s\")\n y 17: (\"%s\")\n",xl,yl);
	fprintf(fp," xunit: (\"%s\")\n yunit: (\"%s\")\n}\n\n",xu,yu);

	if(nxlog)
		scale(xmin,xmax,&power,&xoffset,&xfactor);
	else {
		xoffset=0.0;
		xfactor=1.0;
	}

	if(nylog)
		scale(ymin,ymax,&power,&yoffset,&yfactor);
	else {
		yoffset=0.0;
		yfactor=1.0;
	}
	
	sdind=sd.record_state(&indsize);
	sort_pl_id(&sd,&sdind,indsize);

	fprintf(fp,"data {\n");
	for(j=0;j<indsize;j++) {
		if( (pl=sd.plev(sdind[j]))<minl || pl>maxl)
			continue;
		x=sd.x(sdind[j]);
		y=sd.y(sdind[j]);
		max=sd.n(sdind[j]);
		sst=sd.symbolstyle(sdind[j]);
		pst=sd.linestyle(sdind[j]);
		identstr=sd.ident(sdind[j],&identn);

		fprintf(fp," set {\n");
		for (i=0;i<identn;i++)
			fprintf(fp,"# ident[%d]=\"%s\"\n",i,identstr[i]);
		fprintf(fp,"  linewidth: %f\n",
			sd.linewidth(sdind[j]) ? 0.25 : 0.5 );
		fprintf(fp,"  errwidth: %f\n",
			sd.linewidth(sdind[j]) ? 0.125 : 0.25 );
		fprintf(fp,"  linetype: ");
			switch(pst) {
			case NOPEN: case SOLIDLINE:
				fprintf(fp,"solid\n");
				break;
			case DASHLINE: case DOTLINE:
				fprintf(fp,"dash\n");
				break;
			case DASHDOTLINE:
				fprintf(fp,"dashdot\n");
				break;
			case DASHDOTDOTLINE:
				fprintf(fp,"dashdotdot\n");
				break;
			}
		fprintf(fp,"  linecolor: %s\n",
			color_name(sd.linecolor(sdind[j])));
		fprintf(fp,"  symbolcolor: %s\n",
			color_name(sd.symbolcolor(sdind[j])));
		fprintf(fp,"  xoffset: %g\n",xoffset);
		fprintf(fp,"  xfactor: %g\n",1.0/xfactor);
		fprintf(fp,"  yoffset: %g\n",yoffset);
		fprintf(fp,"  yfactor: %g\n",1.0/yfactor);
		if(max) {
			fprintf(fp,"  %g %g b",
				(x[0]-xoffset)*xfactor,
				(y[0]-yoffset)*yfactor);
			switch(sst) {
			case NOSYMBOL:
				fprintf(fp,";\n");
				break;
			case CLOSEDCIRCLE:
				fprintf(fp," cc;\n");
				break;
			case OPENCIRCLE:  
				fprintf(fp," co;\n");
				break;
			case CLOSEDSQUARE:
				fprintf(fp," sc;\n");
				break;
			case OPENSQUARE:  
				fprintf(fp," so;\n");
				break;
			case CLOSEDDIAMOND:
				fprintf(fp," dc;\n");
				break;
			case OPENDIAMOND: 
				fprintf(fp," do;\n");
				break;
			}
		}
		for(i=1;i<max;i++) {
			fprintf(fp,"  %g %g ",
				(x[i]-xoffset)*xfactor,
				(y[i]-yoffset)*yfactor);
			if(pst==NOPEN)
				fprintf(fp," m");
			else
				fprintf(fp," l");
			switch(sst) {
			case NOSYMBOL:
				fprintf(fp,";\n");
				break;
			case CLOSEDCIRCLE:
				fprintf(fp," cc;\n");
				break;
			case OPENCIRCLE:  
				fprintf(fp," co;\n");
				break;
			case CLOSEDSQUARE:
				fprintf(fp," sc;\n");
				break;
			case OPENSQUARE:  
				fprintf(fp," so;\n");
				break;
			case CLOSEDDIAMOND:
				fprintf(fp," dc;\n");
				break;
			case OPENDIAMOND: 
				fprintf(fp," do;\n");
				break;
			}
		}
		fprintf(fp," }\n");
	}
	fprintf(fp,"}\n");
	fclose(fp);
	free(sdind);
}


void PArea::plevel(int min, int max)
{
	minl=min;
	maxl=max;
	repaint(FALSE);
}


void PArea::mouseMoveEvent( QMouseEvent *e )
{
	if(!nxlog||!nylog)
		return;

	int i,draw=1;
	char *splev;
	QString label;
	QPoint pnt;
	QRect bl(0,height()-BOTTOMLINEHEIGHT,
		 width()-LEFTCOLUMNWIDTH,BOTTOMLINEHEIGHT);
	QRect lc(-LEFTCOLUMNWIDTH,0,LEFTCOLUMNWIDTH,height()-BOTTOMLINEHEIGHT);
	QRect corner(-LEFTCOLUMNWIDTH,height()-BOTTOMLINEHEIGHT,
		     LEFTCOLUMNWIDTH,BOTTOMLINEHEIGHT);
	QPixmap blpix(bl.size());
	QPixmap lcpix(lc.size());
	QPixmap cornerpix(corner.size());

	QPainter p;
	blpix.fill(this,bl.topLeft());
	lcpix.fill(this,lc.topLeft());
	cornerpix.fill(this,corner.topLeft());
	pnt = e->pos()-QPoint(LEFTCOLUMNWIDTH,BORDERWIDTH);
	if(pnt.x()<0||pnt.y()>height()-BOTTOMLINEHEIGHT)
		draw=0;

        p.begin(&blpix);
	for(i=0;i<xlines-1;i++){
		if(draw&&fabs((gxmin+i*dx)*xfac+xoff-pnt.x())<dx*xfac/2)
			continue;
		if(fabs(gxmin+i*dx)<dx/2)
			label.sprintf("%g",0.0);
		else
			label.sprintf("%g",gxmin+i*dx);
		p.drawText((int)((gxmin+i*dx)*xfac+xoff)-50,0,100,bl.height(),
			   AlignCenter, label);
	}
	if(draw&&fabs(xfac)>DBL_MIN*1e2){
		label.sprintf("%g",(pnt.x()-xoff)/xfac);
		p.drawText(pnt.x()-50,0,100,bl.height(),
			   AlignCenter, label);
	}
	p.end();

        p.begin(&lcpix);
	p.translate(LEFTCOLUMNWIDTH,0);
	for(i=0;i<ylines-1;i++) {
		if(draw&&fabs((gymin+i*dy)*yfac+yoff-pnt.y())<dy*yfac/-5)
			continue;
		if(fabs(gymin+i*dy)<dy/2)
			label.sprintf("%g",0.0);
		else
			label.sprintf("%g",gymin+i*dy);
		p.drawText(-LEFTCOLUMNWIDTH+5,
			   (int)((gymin+i*dy)*yfac+yoff)-10,
			   LEFTCOLUMNWIDTH-10,BOTTOMLINEHEIGHT,
			   AlignVCenter|AlignRight, label);
	}

	if(draw&&fabs(yfac)>DBL_MIN*1e2){
		label.sprintf("%g",(pnt.y()-yoff)/yfac);
		p.drawText(-LEFTCOLUMNWIDTH+5,pnt.y()-10,
			   LEFTCOLUMNWIDTH-10,BOTTOMLINEHEIGHT,
			   AlignVCenter|AlignRight, label);
	}
	p.end();

	p.begin(&cornerpix);
	p.translate(LEFTCOLUMNWIDTH,0);
	if(minl==maxl)
		asprintf(&splev,"%d",minl);
	else if(minl<maxl)
		asprintf(&splev,"%d-%d",minl,maxl);
	else
		asprintf(&splev," nol");
	if(zx&&zy)
		label.sprintf("xy%s",splev);
	else if (zx)
		label.sprintf("x %s",splev);
	else if (zy)
		label.sprintf(" y%s",splev);
	else 
		label.sprintf("  %s",splev);
	free(splev);
	p.drawText(-LEFTCOLUMNWIDTH+BORDERWIDTH,0,
		   corner.width(),corner.height(),
		   AlignLeft|AlignVCenter,label);

	if(!(draw&&fabs(gxmin*xfac+xoff-pnt.x())<dx*xfac/2)){
		
		if(fabs(gxmin)<dx/2)
			label.sprintf("%g",0.0);
		else
			label.sprintf("%g",gxmin);
		p.drawText((int)(gxmin*xfac+xoff)-50,0,100,bl.height(),
			   AlignCenter, label);
	}
	if(draw&&fabs(xfac)>DBL_MIN*1e2){
		label.sprintf("%g",(pnt.x()-xoff)/xfac);
		p.drawText(pnt.x()-50,0,100,bl.height(),
			   AlignCenter, label);
	}
	if(!(draw&&fabs(gymin*yfac+yoff-pnt.y())<dy*yfac/-5)) {
		if(fabs(gymin)<dy/2)
			label.sprintf("%g",0.0);
		else
			label.sprintf("%g",gymin);
		p.drawText(-LEFTCOLUMNWIDTH+5,
			   (int)(gymin*yfac+yoff)-10
			   -height()+BOTTOMLINEHEIGHT,
			   LEFTCOLUMNWIDTH-10,BOTTOMLINEHEIGHT,
			   AlignVCenter|AlignRight, label);
	}
	if(draw&&fabs(yfac)>DBL_MIN*1e2){
		label.sprintf("%g",(pnt.y()-yoff)/yfac);
		p.drawText(-LEFTCOLUMNWIDTH+5,pnt.y()-10
			   -height()+BOTTOMLINEHEIGHT,
			   LEFTCOLUMNWIDTH-10,BOTTOMLINEHEIGHT,
			   AlignVCenter|AlignRight, label);
	}
	p.end();

	p.begin(this);
	p.translate(LEFTCOLUMNWIDTH,BORDERWIDTH);
	p.drawPixmap(lc.topLeft(),lcpix);
	p.drawPixmap(bl.topLeft(),blpix);
	p.drawPixmap(corner.topLeft(),cornerpix);
	p.end();

}

void log_grid_param(double min, double max,
		    int *minpow, int *maxpow,
		    int *minfac, int *maxfac,
		    int *series, int fac[])
{
	*minpow=(int)floor(log10(min));
	*maxpow=(int)ceil(log10(max));

	for(*minfac=1; *minfac<10; (*minfac)++) {
		if((*minfac)*pow(10,*minpow)>=min)
			break;
	}
	for(*maxfac=10; *maxfac>2; (*maxfac)--) {
		if((*maxfac)*pow(10,*maxpow-1)<=max)
			break;
	}
	if(*maxfac==10)
		*maxfac=1;
	else
		(*maxpow)--;

	*series=9;
	if((*maxpow-*minpow)*9>10)
		*series=3;
	if((*maxpow-*minpow)*3>10)
		*series=1;

      fac[0]=1;
      if(*series==3) {
	      fac[1]=2;
	      fac[2]=5;
      } else if(*series==9) {
	      fac[1]=2;
	      fac[2]=3;
	      fac[3]=4;
	      fac[4]=5;
	      fac[5]=6;
	      fac[6]=7;
	      fac[7]=8;
	      fac[8]=9;
      }

      if(*series==1&&*maxfac!=1) {
	      *maxfac=1;
	      //(*maxpow)++;
      }

      if(*series==3&&(*maxfac!=1&&*maxfac!=2&&*maxfac!=5)) {
		if(*maxfac>5) {
			*maxfac=5;
			//(*maxpow)++;
		}
		else if(*maxfac>2)
			*maxfac=2;
      }

      if(*series==1&&*minfac!=1){
	      *minfac=1;
	      (*minpow)++;
      }
      if(*series==3&&(*minfac!=1&&*minfac!=2&&*minfac!=5)) {
	      if(*minfac>5){
		      *minfac=1;
		      (*minpow)++;
	      }
	      else if(*minfac>2)
		      *minfac=5;
      }
}

void grid_param(double &start, double &end, int &lines, double &delta)
{
	double min=start,max=end, diff=(max-min),fac=1.0,power;

	if(diff<DBL_MIN*1e2) {
		fprintf(stderr,"error in grid_param!\n"
			"2nd arg(%g) - 1st arg(%g) has to be greater than %g\n"
			,max,min,DBL_MIN*1e2);
		start=0.0;
		end=1.0;
		delta=0.1;
		lines=11;
		return;
	}
	if(lines<=0)
		lines=10;
	diff/=lines;
	power=0;
	while(diff<1) {
		diff*=10;
		fac*=10;
		power+=1;
	}
	while(diff>=1) {
		diff*=0.1;
		fac*=0.1;
		power-=1;
	}
	delta=0.2/fac;
	if(diff>0.2)
	  delta=0.5/fac;
	if(diff>0.5)
	  delta=1/fac;

	start=(floor(min/delta)*delta);
	while(start+delta/1000.0<min)
		start+=delta;

	lines=1;
	end=start;
	while(end+delta<max) {
		(lines)++;
		end+=delta;
	}
	return;
}

void PArea::timerEvent(QTimerEvent *)
{
	SharedData *sd;
	int rep=0;
	static unsigned int version=0;
	
	sd=new SharedData();
	if(sd->changed(&version))
		rep=1;
	delete sd;
	
	if(rep)
		repaint(FALSE);
};
