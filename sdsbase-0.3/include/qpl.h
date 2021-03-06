/* 
 * $Author: vstein $ 
 * $Revision: 1.1 $
 * $Id: qpl.h,v 1.1 2003/01/23 14:50:49 vstein Exp $
*/

#ifndef QPL_H
#define QPL_H

#include <parea.h> 
#include <qmenubar.h>
#include <qlineedit.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvalidator.h>

/* struct PrintParam { */
/* 	double xsize,ysize; */
/* 	char *xlabel,*xunit; */
/* 	char *ylabel,*yunit; */
/* 	char *filename; */
/* } */

class PrintDialog : public QDialog 
{
	Q_OBJECT 
 public:
	PrintDialog( QWidget *parent, const char *name ); 
	QLineEdit *xsize,*ysize;
	QLineEdit *xlabel,*ylabel;
	QLineEdit *xunit,*yunit;
	QLineEdit *filename;
	QVBoxLayout *topLayout;
	QGridLayout *grid;
	QPushButton *ok;
	QPushButton *cancel;
	QLabel *xs;
	QLabel *ys;
	QDoubleValidator *dv;
	QLabel *xl;
	QLabel *yl;
	QLabel *xu;
	QLabel *yu;
	QLabel *fn;
}; 

class ViewPortDialog : public QDialog
{
	Q_OBJECT
 public:
	ViewPortDialog( QWidget *parent, const char *name);
	QLineEdit *xmin,*xmax;
	QLineEdit *ymin,*ymax;
	QVBoxLayout *topLayout;
	QGridLayout *grid;
	QPushButton *ok;
	QPushButton *cancel;
	QLabel *mix;
	QLabel *max;
	QLabel *miy;
	QLabel *may;
};

class PlotLevelDialog : public QDialog
{
	Q_OBJECT
 public:
	PlotLevelDialog( QWidget *parent, const char *name);
	QLineEdit *minl,*maxl;
	QVBoxLayout *topLayout;
	QGridLayout *grid;
	QPushButton *ok;
	QPushButton *cancel;
	QLabel *mil;
	QLabel *mal;
};

class QtPl : public QWidget
{
	Q_OBJECT
		public:
	QtPl( QWidget *parent=0, const char *name=0,
	      unsigned updtime=100, int minlevel=0, int maxlevel=100);

 public slots:
	void print();
	void viewport();
	void plotlevel();
	void zin();
	void zin_fast();
	void zout();
	void zout_fast();
	void ztotal();
	void zoomx();
	void zoomy();
	void sleft();
	void sright();
	void sup();
	void sdown();
	void sleft_fast();
	void sright_fast();
	void sup_fast();
	void sdown_fast();
	void logax();
	void minlvl(int);
	void maxlvl(int);
	void bothlvl(int);
	void mininc(int);
	void maxinc(int);
	void bothinc(int);
 protected:
        void resizeEvent( QResizeEvent * );
	void timerEvent(QTimerEvent *);

 signals:
	void zoom(int);
	void shift(int);
	void loga();
	void pspl(char *, char *,
		  char *, char *,
		  char *, char *,
		  char *);
	void zoomport(double, double, double, double);
	void plevel(int, int);
 private:
	QMenuBar *menu;
	QPopupMenu *view;
	QPopupMenu *level, *submin, *submax, *subboth;

	int zx,zy;
	int zxid,zyid;

	int zi,zo,zfi,zfo,zt;
	int sl,sr,su,sd;
	int sfl,sfr,sfu,sfd;
	int lg;

	PArea *parea;
	PrintDialog *prdi;
	ViewPortDialog *vpdi;
	PlotLevelDialog *pldi;
};

#endif // QPL_H











