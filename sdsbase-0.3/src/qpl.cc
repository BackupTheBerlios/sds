/* 
 * $Author: vstein $ 
 * $Revision: 1.1 $
 * $Id: qpl.cc,v 1.1 2003/01/23 14:50:49 vstein Exp $
*/

#include <qapplication.h>
#include <qmenubar.h>
#include <qaccel.h>
#include <qfont.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <stdlib.h>

#include <getopt.h>

#ifdef SETITEMPARAM
#include <qpl.h>
#else
#include <qpl1.h>
#endif
#include <math.h>
#include <float.h>

extern char *program_invocation_short_name;

const char verstring[]="0.03";

QtPl::QtPl( QWidget *parent, const char *name, 
	    unsigned updtime, int minlevel, int maxlevel )
        : QWidget( parent, name )
{
    setMinimumSize( 500, 355 );
    QColor col;

    QPopupMenu *file = new QPopupMenu();
    CHECK_PTR( file );
    file->insertItem("&Quit", qApp, SLOT(quit()), ALT+Key_Q );
    file->insertItem("&Print", this, SLOT(print()), ALT+Key_P );

    view = new QPopupMenu();
    CHECK_PTR( view );
    view->insertItem("zoom &in", this, SLOT(zin()), Key_Prior );
    view->insertItem("zoom &in fast", this, SLOT(zin_fast()), SHIFT|Key_Prior );
    view->insertItem("zoom &out", this, SLOT(zout()), Key_Next );
    view->insertItem("zoom &out fast", this, SLOT(zout_fast()), SHIFT|Key_Next );
    view->insertItem("zoom &total", this, SLOT(ztotal()), Key_T );
    view->insertItem("shift &left", this, SLOT(sleft()), Key_Left );
    view->insertItem("shift fast &left", this, SLOT(sleft_fast()), SHIFT|Key_Left );
    view->insertItem("shift &right", this, SLOT(sright()), Key_Right );
    view->insertItem("shift fast &right", this, SLOT(sright_fast()), SHIFT|Key_Right );
    view->insertItem("shift &up", this, SLOT(sup()), Key_Up );
    view->insertItem("shift fast &up", this, SLOT(sup_fast()), SHIFT|Key_Up );
    view->insertItem("shift &down", this, SLOT(sdown()), Key_Down );
    view->insertItem("shift fast &down", this, SLOT(sdown_fast()), SHIFT|Key_Down );
    zxid=view->insertItem("fix &x", this, SLOT(zoomx()), Key_X );
    zyid=view->insertItem("fix &y", this, SLOT(zoomy()), Key_Y );
    view->insertItem("&log-axes", this, SLOT(logax()), Key_L );
    view->insertItem("&view port", this, SLOT(viewport()), Key_V );

    submin = new QPopupMenu();
    CHECK_PTR( submin );
#ifdef SETITEMPARAM
    submin->insertItem("min=0", this, SLOT(minlvl(int)), Key_AsciiCircum, 1);
    submin->setItemParameter(1,0);
    submin->insertItem("min=1", this, SLOT(minlvl(int)), Key_1, 2);
    submin->setItemParameter(2,1);
    submin->insertItem("min=2", this, SLOT(minlvl(int)), Key_2, 3);
    submin->setItemParameter(3,2);
    submin->insertItem("min=3", this, SLOT(minlvl(int)), Key_3, 4);
    submin->setItemParameter(4,3);
    submin->insertItem("min=4", this, SLOT(minlvl(int)), Key_4, 5);
    submin->setItemParameter(5,4);
    submin->insertItem("min=5", this, SLOT(minlvl(int)), Key_5, 6);
    submin->setItemParameter(6,5);
    submin->insertItem("min=6", this, SLOT(minlvl(int)), Key_6, 7);
    submin->setItemParameter(7,6);
    submin->insertItem("min=7", this, SLOT(minlvl(int)), Key_7, 8);
    submin->setItemParameter(8,7);
    submin->insertItem("min=8", this, SLOT(minlvl(int)), Key_8, 9);
    submin->setItemParameter(9,8);
    submin->insertItem("min=9", this, SLOT(minlvl(int)), Key_9,10);
    submin->setItemParameter(10,9);
    submin->insertItem("min=10", this, SLOT(minlvl(int)), Key_0,11);
    submin->setItemParameter(11,10);
    submin->insertItem("min=-1", this, SLOT(minlvl(int)), 
		       CTRL+ALT+Key_AsciiCircum,12);
    submin->setItemParameter(12,-1);
    submin->insertItem("min+=1", this, SLOT(mininc(int)), CTRL+Key_Up,13);
    submin->setItemParameter(13,1);
    submin->insertItem("min-=1", this, SLOT(mininc(int)), CTRL+Key_Down,14);
    submin->setItemParameter(14,-1);
#else
    submin->insertItem("min=0", this, SLOT(minlvl0()), Key_AsciiCircum);
    submin->insertItem("min=1", this, SLOT(minlvl1()), Key_1);
    submin->insertItem("min=2", this, SLOT(minlvl2()), Key_2);
    submin->insertItem("min=3", this, SLOT(minlvl3()), Key_3);
    submin->insertItem("min=4", this, SLOT(minlvl4()), Key_4);
    submin->insertItem("min=5", this, SLOT(minlvl5()), Key_5);
    submin->insertItem("min=6", this, SLOT(minlvl6()), Key_6);
    submin->insertItem("min=7", this, SLOT(minlvl7()), Key_7);
    submin->insertItem("min=8", this, SLOT(minlvl8()), Key_8);
    submin->insertItem("min=9", this, SLOT(minlvl9()), Key_9);
    submin->insertItem("min=10", this, SLOT(minlvlx()), Key_0);
    submin->insertItem("min=-1", this, SLOT(minlvlm()), 
		       CTRL+ALT+Key_AsciiCircum);
    submin->insertItem("min+=1", this, SLOT(mininc()), CTRL+Key_Up);
    submin->insertItem("min-=1", this, SLOT(mindec()), CTRL+Key_Down);
#endif

    submax = new QPopupMenu();
    CHECK_PTR( submax );
#ifdef SETITEMPARAM
    submax->insertItem("max=0", this, SLOT(maxlvl(int)), 
		      CTRL+Key_AsciiCircum,15);
    submax->setItemParameter(15,0);
    submax->insertItem("max=1", this, SLOT(maxlvl(int)), CTRL+Key_1,16);
    submax->setItemParameter(16,1);
    submax->insertItem("max=2", this, SLOT(maxlvl(int)), CTRL+Key_2,17);
    submax->setItemParameter(17,2);
    submax->insertItem("max=3", this, SLOT(maxlvl(int)), CTRL+Key_3,18);
    submax->setItemParameter(18,3);
    submax->insertItem("max=4", this, SLOT(maxlvl(int)), CTRL+Key_4,19);
    submax->setItemParameter(19,4);
    submax->insertItem("max=5", this, SLOT(maxlvl(int)), CTRL+Key_5,20);
    submax->setItemParameter(20,5);
    submax->insertItem("max=6", this, SLOT(maxlvl(int)), CTRL+Key_6,21);
    submax->setItemParameter(21,6);
    submax->insertItem("max=7", this, SLOT(maxlvl(int)), CTRL+Key_7,22);
    submax->setItemParameter(22,7);
    submax->insertItem("max=8", this, SLOT(maxlvl(int)), CTRL+Key_8,23);
    submax->setItemParameter(23,8);
    submax->insertItem("max=9", this, SLOT(maxlvl(int)), CTRL+Key_9,24);
    submax->setItemParameter(24,9);
    submax->insertItem("max=10", this, SLOT(maxlvl(int)), CTRL+Key_0,25);
    submax->setItemParameter(25,10);
    submax->insertItem("max=100", this, SLOT(maxlvl(int)), 
		       CTRL+ALT+Key_0,26);
    submax->setItemParameter(26,100);
    submax->insertItem("max+=1", this, SLOT(maxinc(int)), 
		       SHIFT+CTRL+Key_Up,27);
    submax->setItemParameter(27,1);
    submax->insertItem("max-=1", this, SLOT(maxinc(int)), 
		       SHIFT+CTRL+Key_Down,28);
    submax->setItemParameter(28,-1);
#else
    submax->insertItem("max=0", this, SLOT(maxlvl0()), 
		      CTRL+Key_AsciiCircum);
    submax->insertItem("max=1", this, SLOT(maxlvl1()), CTRL+Key_1);
    submax->insertItem("max=2", this, SLOT(maxlvl2()), CTRL+Key_2);
    submax->insertItem("max=3", this, SLOT(maxlvl3()), CTRL+Key_3);
    submax->insertItem("max=4", this, SLOT(maxlvl4()), CTRL+Key_4);
    submax->insertItem("max=5", this, SLOT(maxlvl5()), CTRL+Key_5);
    submax->insertItem("max=6", this, SLOT(maxlvl6()), CTRL+Key_6);
    submax->insertItem("max=7", this, SLOT(maxlvl7()), CTRL+Key_7);
    submax->insertItem("max=8", this, SLOT(maxlvl8()), CTRL+Key_8);
    submax->insertItem("max=9", this, SLOT(maxlvl9()), CTRL+Key_9);
    submax->insertItem("max=10", this, SLOT(maxlvlx()), CTRL+Key_0);
    submax->insertItem("max=100", this, SLOT(maxlvlc()), 
		       CTRL+ALT+Key_0);
    submax->insertItem("max+=1", this, SLOT(maxinc()), SHIFT+CTRL+Key_Up);
    submax->insertItem("max-=1", this, SLOT(maxdec()), SHIFT+CTRL+Key_Down);
#endif

    subboth = new QPopupMenu();
    CHECK_PTR( subboth );
#ifdef SETITEMPARAM
    subboth->insertItem("both=0", this, SLOT(bothlvl(int)), 
			ALT+Key_AsciiCircum,29);
    subboth->setItemParameter(29,0);
    subboth->insertItem("both=1", this, SLOT(bothlvl(int)), ALT+Key_1,30);
    subboth->setItemParameter(30,1);
    subboth->insertItem("both=2", this, SLOT(bothlvl(int)), ALT+Key_2,31);
    subboth->setItemParameter(31,2);
    subboth->insertItem("both=3", this, SLOT(bothlvl(int)), ALT+Key_3,32);
    subboth->setItemParameter(32,3);
    subboth->insertItem("both=4", this, SLOT(bothlvl(int)), ALT+Key_4,33);
    subboth->setItemParameter(33,4);
    subboth->insertItem("both=5", this, SLOT(bothlvl(int)), ALT+Key_5,34);
    subboth->setItemParameter(34,5);
    subboth->insertItem("both=6", this, SLOT(bothlvl(int)), ALT+Key_6,35);
    subboth->setItemParameter(35,6);
    subboth->insertItem("both=7", this, SLOT(bothlvl(int)), ALT+Key_7,36);
    subboth->setItemParameter(36,7);
    subboth->insertItem("both=8", this, SLOT(bothlvl(int)), ALT+Key_8,37);
    subboth->setItemParameter(37,8);
    subboth->insertItem("both=9", this, SLOT(bothlvl(int)), ALT+Key_9,38);
    subboth->setItemParameter(38,9);
    subboth->insertItem("both=10", this, SLOT(bothlvl(int)), ALT+Key_0,39);
    subboth->setItemParameter(39,10);
    subboth->insertItem("both+=1", this, SLOT(bothinc(int)), ALT+Key_Up,40);
    subboth->setItemParameter(40,1);
    subboth->insertItem("both-=1", this, SLOT(bothinc(int)), ALT+Key_Down,41);
    subboth->setItemParameter(41,-1);
#else
    subboth->insertItem("both=0", this, SLOT(bothlvl0()), 
			ALT+Key_AsciiCircum);
    subboth->insertItem("both=1", this, SLOT(bothlvl1()), ALT+Key_1);
    subboth->insertItem("both=2", this, SLOT(bothlvl2()), ALT+Key_2);
    subboth->insertItem("both=3", this, SLOT(bothlvl3()), ALT+Key_3);
    subboth->insertItem("both=4", this, SLOT(bothlvl4()), ALT+Key_4);
    subboth->insertItem("both=5", this, SLOT(bothlvl5()), ALT+Key_5);
    subboth->insertItem("both=6", this, SLOT(bothlvl6()), ALT+Key_6);
    subboth->insertItem("both=7", this, SLOT(bothlvl7()), ALT+Key_7);
    subboth->insertItem("both=8", this, SLOT(bothlvl8()), ALT+Key_8);
    subboth->insertItem("both=9", this, SLOT(bothlvl9()), ALT+Key_9);
    subboth->insertItem("both=10", this, SLOT(bothlvlx()), ALT+Key_0);
    subboth->insertItem("both+=1", this, SLOT(bothinc()), ALT+Key_Up);
    subboth->insertItem("both-=1", this, SLOT(bothdec()), ALT+Key_Down);
#endif

    level = new QPopupMenu();
    CHECK_PTR( level );
    level->insertItem("pl&ot level", this, SLOT(plotlevel()), Key_O );
    level->insertItem("min level", submin);
    level->insertItem("max level", submax);
    level->insertItem("both levels", subboth);

    zx=ZOOMX;
    zy=ZOOMY;
    zfi=zfo=zi=zo=zt=0;
    sfl=sfr=sfd=sfu=0;
    sl=sr=sd=su=0;
    lg=0;

    menu = new QMenuBar(this);
    CHECK_PTR(menu);

    menu->insertItem("&File", file);
    menu->insertItem("&View", view);
    menu->insertItem("&Level", level);

    parea = new PArea( this, "paerea", updtime, minlevel, maxlevel);
    parea->move(0,menu->height());
    col.setNamedColor("ivory1");
    parea->setBackgroundColor( col );
    parea->setCursor(crossCursor);
    connect(this, SIGNAL(zoom(int)),
	    parea, SLOT(zoom(int)));
    connect(this, SIGNAL(shift(int)),
	    parea, SLOT(shift(int)));
    connect(this, SIGNAL(loga()),
	    parea, SLOT(loga()));
    connect(this, SIGNAL(pspl(char *, char *, char *, char *,
			      char *, char *, char *)),
	    parea, SLOT(pspl(char *, char *, char *, char *,
			     char *, char *, char *)));
    connect(this, SIGNAL(zoomport(double, double, double, double)),
	    parea, SLOT(zoomport(double, double, double, double)));
    connect(this, SIGNAL(plevel(int, int)),
	    parea, SLOT(plevel(int, int)));
    prdi= new PrintDialog(this,"printDialog");
    prdi->hide();
    vpdi= new ViewPortDialog(this,"viewportDialog");
    vpdi->setFixedSize(100,200);
    vpdi->hide();
    pldi= new PlotLevelDialog(this,"plotlevelDialog");
    pldi->hide();
    startTimer(30);
}

void QtPl::resizeEvent( QResizeEvent * )
{
	parea->resize( width(), height() - menu->height() );
}

void version(void)
{
	fprintf(stderr, "%s version %s\n", program_invocation_short_name,
		verstring);

	exit(0);
}

void usage(void)
{
	fprintf(stderr,
	      "usage: %s [-u time] [-i minlevel] [-a maxlevel] [-l level]\n"
		"\n"
		"-u,    --updatetime in milli seconds\n"
		"-l,    --level      level to display\n"
		"-i,    --minlevel   minimum level\n"
		"-a,    --maxlevel   maximum level\n"
		"-h,-?, --help       this message\n"
		"-V,    --version    display version information and exit.\n"
		"-W,    --warranty   display licensing terms.\n"
		"-C,    --copyright  \n"
		"\n %s shows data loaded to a shared data segment\n", 
		program_invocation_short_name,program_invocation_short_name);
	exit(0);
}

void copyright(void)
{
	printf("%s version %s, Copyright (C) 1999 Roland Schaefer"
	       "qpl comes with ABSOLUTELY NO WARRANTY;\n"
	       "This is free software, and you are welcome to"
	       " redistribute it\n"
	       "under certain conditions; type `qpl -w' for details.\n",
	       program_invocation_short_name,verstring);
	exit(0);
}

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

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    unsigned updtime=333;
    int c,minl=0,maxl=100;

    static struct option const long_options[]= {
	    {"help",       no_argument,       0, 'h'},
	    {"help",       no_argument,       0, '?'},
	    {"version",    no_argument,       0, 'V'},
	    {"warranty",   no_argument,       0, 'W'},
	    {"copyright",  no_argument,       0, 'C'},
	    {"updatetime", required_argument, 0, 'u'},
	    {"level",      required_argument, 0, 'l'},
	    {"minlevel",   required_argument, 0, 'i'},
	    {"maxlevel",   required_argument, 0, 'a'},
	    {0,0,0,0}
    };
    
    while ((c = getopt_long(argc, argv, "u:i:a:l:h?VWC",
			    long_options, (int *) 0)) != EOF) {
		switch (c) {
		case 0:
			break;
		case 'u':
			if(sscanf(optarg,"%u",&updtime)!=1) {
				fprintf(stderr,"error in option --updatetime:"
					" \"%s\" should be an unsigned int\n",
					optarg);
				usage();
			}
			break;
		case 'l':
			if(sscanf(optarg,"%i",&minl)!=1) {
				fprintf(stderr,"error in option --minlevel:"
					" \"%s\" should be an integer\n",
					optarg);
				usage();
			}
			maxl=minl;
			break;
		case 'i':
			if(sscanf(optarg,"%i",&minl)!=1) {
				fprintf(stderr,"error in option --minlevel:"
					" \"%s\" should be an integer\n",
					optarg);
				usage();
			}
			break;
		case 'a':
			if(sscanf(optarg,"%i",&maxl)!=1) {
				fprintf(stderr,"error in option --maxlevel:"
					" \"%s\" should be an integer\n",
					optarg);
				usage();
			}
			break;
		case 'W':
			warranty();
			break;
		case 'V':
			version();
			break;
		case 'C':
			copyright();
			break;
		case 'h':
		case '?':
		default:
			usage();
			break;
		}
	}

    if(argc==2) {
	    updtime=(unsigned)strtol(argv[1],NULL,0);
	    if(!updtime)
		    updtime=10000;
    }

    QtPl w(NULL,NULL,updtime,minl,maxl);
    w.setGeometry( 100, 100, 500, 355 );
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}

void QtPl::zoomx()
{
	if(zx) {
		view->changeItem("Zoom &x",zxid);
		zx=0;
	} else {
		view->changeItem("fix  &x",zxid);
		zx=ZOOMX;
	}
	emit zoom(ZOOMX);
}

void QtPl::zoomy()
{
	if(zy) {
		view->changeItem("Zoom &y",zyid);
		zy=0;
	} else {
		view->changeItem("fix  &y",zyid);
		zy=ZOOMY;
	}
	emit zoom(ZOOMY);
}

void QtPl::zin()
{
	zi++;
}

void QtPl::zout()
{
	zo++;
}

void QtPl::zin_fast()
{
	zfi++;
}

void QtPl::zout_fast()
{
	zfo++;
}

void QtPl::ztotal()
{
	zt++;
}

void QtPl::sleft()
{
	sl++;
}

void QtPl::sright()
{
	sr++;
}

void QtPl::sup()
{
	su++;
}

void QtPl::sdown()
{
	sd++;
}

void QtPl::sleft_fast()
{
	sfl++;
}

void QtPl::sright_fast()
{
	sfr++;
}

void QtPl::sup_fast()
{
	sfu++;
}

void QtPl::sdown_fast()
{
	sfd++;
}

void QtPl::logax()
{
	lg++;
}

#ifdef SETITEMPARAM
void QtPl::minlvl(int min)
{
	int max=parea->plmax();
	emit plevel(min,max);
}

void QtPl::mininc(int inc)
{
	int max=parea->plmax();
	int min=parea->plmin();
	emit plevel(min+inc,max);
}

void QtPl::maxlvl(int max)
{
	int min=parea->plmin();
	emit plevel(min,max);
}

void QtPl::maxinc(int inc)
{
	int max=parea->plmax();
	int min=parea->plmin();
	emit plevel(min,max+inc);
}

void QtPl::bothlvl(int lvl)
{
	emit plevel(lvl,lvl);
}

void QtPl::bothinc(int inc)
{
	int max=parea->plmax();
	int min=parea->plmin();
	emit plevel(min+inc,max+inc);
}

#else
void QtPl::minlvl0()
{
	int max=parea->plmax();
	emit plevel(0,max);
}
void QtPl::minlvl1()
{
	int max=parea->plmax();
	emit plevel(1,max);
}
void QtPl::minlvl2()
{
	int max=parea->plmax();
	emit plevel(2,max);
}
void QtPl::minlvl3()
{
	int max=parea->plmax();
	emit plevel(3,max);
}
void QtPl::minlvl4()
{
	int max=parea->plmax();
	emit plevel(4,max);
}
void QtPl::minlvl5()
{
	int max=parea->plmax();
	emit plevel(5,max);
}
void QtPl::minlvl6()
{
	int max=parea->plmax();
	emit plevel(6,max);
}
void QtPl::minlvl7()
{
	int max=parea->plmax();
	emit plevel(7,max);
}
void QtPl::minlvl8()
{
	int max=parea->plmax();
	emit plevel(8,max);
}
void QtPl::minlvl9()
{
	int max=parea->plmax();
	emit plevel(9,max);
}
void QtPl::minlvlx()
{
	int max=parea->plmax();
	emit plevel(10,max);
}
void QtPl::minlvlm()
{
	int max=parea->plmax();
	emit plevel(-1,max);
}
void QtPl::mininc()
{
	int max=parea->plmax();
	int min=parea->plmin();
	emit plevel(++min,max);
}
void QtPl::mindec()
{
	int max=parea->plmax();
	int min=parea->plmin();
	emit plevel(--min,max);
}

void QtPl::maxlvl0()
{
	int min=parea->plmin();
	emit plevel(min,0);
}
void QtPl::maxlvl1()
{
	int min=parea->plmin();
	emit plevel(min,1);
}
void QtPl::maxlvl2()
{
	int min=parea->plmin();
	emit plevel(min,2);
}
void QtPl::maxlvl3()
{
	int min=parea->plmin();
	emit plevel(min,3);
}
void QtPl::maxlvl4()
{
	int min=parea->plmin();
	emit plevel(min,4);
}
void QtPl::maxlvl5()
{
	int min=parea->plmin();
	emit plevel(min,5);
}
void QtPl::maxlvl6()
{
	int min=parea->plmin();
	emit plevel(min,6);
}
void QtPl::maxlvl7()
{
	int min=parea->plmin();
	emit plevel(min,7);
}
void QtPl::maxlvl8()
{
	int min=parea->plmin();
	emit plevel(min,8);
}
void QtPl::maxlvl9()
{
	int min=parea->plmin();
	emit plevel(min,9);
}
void QtPl::maxlvlx()
{
	int min=parea->plmin();
	emit plevel(min,10);
}
void QtPl::maxlvlc()
{
	int min=parea->plmin();
	emit plevel(min,100);
}
void QtPl::maxinc()
{
	int max=parea->plmax();
	int min=parea->plmin();
	emit plevel(min,++max);
}
void QtPl::maxdec()
{
	int max=parea->plmax();
	int min=parea->plmin();
	emit plevel(min,--max);
}

void QtPl::bothlvl0()
{
	emit plevel(0,0);
}
void QtPl::bothlvl1()
{
	emit plevel(1,1);
}
void QtPl::bothlvl2()
{
	emit plevel(2,2);
}
void QtPl::bothlvl3()
{
	emit plevel(3,3);
}
void QtPl::bothlvl4()
{
	emit plevel(4,4);
}
void QtPl::bothlvl5()
{
	emit plevel(5,5);
}
void QtPl::bothlvl6()
{
	emit plevel(6,6);
}
void QtPl::bothlvl7()
{
	emit plevel(7,7);
}
void QtPl::bothlvl8()
{
	emit plevel(8,8);
}
void QtPl::bothlvl9()
{
	emit plevel(9,9);
}
void QtPl::bothlvlx()
{
	emit plevel(10,10);
}
void QtPl::bothinc()
{
	int max=parea->plmax();
	int min=parea->plmin();
	emit plevel(++min,++max);
}
void QtPl::bothdec()
{
	int max=parea->plmax();
	int min=parea->plmin();
	emit plevel(--min,--max);
}
#endif

void QtPl::print()
{
	static char *xs=NULL;
	static char *ys=NULL;
	static char *xl=NULL;
	static char *yl=NULL;
	static char *xu=NULL;
	static char *yu=NULL;
	static char *fn=NULL;
	if(prdi->exec()){
		if(xs) free(xs);
		if(ys) free(ys);
		if(xl) free(xl);
		if(yl) free(yl);
		if(xu) free(xu);
		if(yu) free(yu);
		if(fn) free(fn);
		xs=strdup(prdi->xsize->text());
		ys=strdup(prdi->ysize->text());
		xl=strdup(prdi->xlabel->text());
		yl=strdup(prdi->ylabel->text());
		xu=strdup(prdi->xunit->text());
		yu=strdup(prdi->yunit->text());
		fn=strdup(prdi->filename->text());
		emit pspl(xs,ys,xl,yl,xu,yu,fn);
	}
}

#define S_XMIN 0x01
#define S_XMAX 0x02
#define S_YMIN 0x04
#define S_YMAX 0x08


void QtPl::viewport()
{
	int cont;
	char *xmin,*xmax;
	char *ymin,*ymax;
	double xmi,xma;
	double ymi,yma;
	double tmp;
	if(parea->nx_log()) {
		asprintf(&xmin,"%g",parea->minx());
		asprintf(&xmax,"%g",parea->maxx());
	} else {
		asprintf(&xmin,"%g",parea->lminx());
		asprintf(&xmax,"%g",parea->lmaxx());
	}
	if(parea->ny_log()) {
		asprintf(&ymin,"%g",parea->miny());
		asprintf(&ymax,"%g",parea->maxy());
	} else {
		asprintf(&ymin,"%g",parea->lminy());
		asprintf(&ymax,"%g",parea->lmaxy());
	} 
	vpdi->xmin->validateAndSet(xmin,0,0,0);
	vpdi->xmax->validateAndSet(xmax,0,0,0);
	vpdi->ymin->validateAndSet(ymin,0,0,0);
	vpdi->ymax->validateAndSet(ymax,0,0,0);
	while(1){
		if(!vpdi->exec())
			break;
		cont=0;
		if(sscanf(vpdi->xmin->text(),"%lf",&xmi)!=1) {
			free(xmin);
			if(parea->nx_log())
				asprintf(&xmin,"%g",parea->minx());
			else
				asprintf(&xmin,"%g",parea->lminx());
			vpdi->xmin->validateAndSet(xmin,0,0,0);
			cont=1;
		}
		if(sscanf(vpdi->xmax->text(),"%lf",&xma)!=1) {
			free(xmax);
			if(parea->nx_log())
				asprintf(&xmax,"%g",parea->maxx());
			else
				asprintf(&xmax,"%g",parea->lmaxx());
			vpdi->xmax->validateAndSet(xmax,0,0,0);
			cont=1;
		}
		if(sscanf(vpdi->ymin->text(),"%lf",&ymi)!=1) {
			free(ymin);
			if(parea->ny_log())
				asprintf(&ymin,"%g",parea->miny());
			else
				asprintf(&ymin,"%g",parea->lminy());
			vpdi->ymin->validateAndSet(ymin,0,0,0);
			cont=1;
		}
		if(sscanf(vpdi->ymax->text(),"%lf",&yma)!=1) {
			free(ymax);
			if(parea->ny_log())
				asprintf(&ymax,"%g",parea->maxy());
			else
				asprintf(&ymax,"%g",parea->lmaxy());
			vpdi->ymax->validateAndSet(ymax,0,0,0);
			cont=1;
		}
		if(cont)
			continue;

		if(xmi<xma&&ymi<yma) {
			if((parea->nx_log()||(xmi>0&&xma>0))&&
			   (parea->ny_log()||(ymi>0&&yma>0))) {
				emit zoomport(xmi,xma,ymi,yma);
				break;
			}
		}
		if(xmi>xma) {
			tmp=xma;
			xma=xmi;
			xmi=tmp;
			free(xmin);
			asprintf(&xmin,"%g",xmi);
			vpdi->xmin->validateAndSet(xmin,0,0,0);
			free(xmax);
			asprintf(&xmax,"%g",xma);
			vpdi->xmax->validateAndSet(xmax,0,0,0);
		}
		if(ymi>yma) {
			tmp=yma;
			yma=ymi;
			ymi=tmp;
			free(ymin);
			asprintf(&ymin,"%g",ymi);
			vpdi->ymin->validateAndSet(ymin,0,0,0);
			free(ymax);
			asprintf(&ymax,"%g",yma);
			vpdi->ymax->validateAndSet(ymax,0,0,0);
		}
		if(!parea->nx_log()) {
			if(xmi<=0) {
				xmi=fabs(xma/10);
				free(xmin);
				asprintf(&xmin,"%g",xmi);
				vpdi->xmin->validateAndSet(xmin,0,0,0);
			}
			if(xma<=0) {
				xma=fabs(xmi*10);
				free(xmax);
				asprintf(&xmax,"%g",xma);
				vpdi->xmax->validateAndSet(xmax,0,0,0);
			}
		}
		if(!parea->ny_log()) {
			if(ymi<=0) {
				ymi=fabs(yma/10);
				free(ymin);
				asprintf(&ymin,"%g",ymi);
				vpdi->ymin->validateAndSet(ymin,0,0,0);
			}
			if(yma<=0) {
				yma=fabs(ymi*10);
				free(ymax);
				asprintf(&ymax,"%g",yma);
				vpdi->ymax->validateAndSet(ymax,0,0,0);
			}
		}
		if(fabs(xmi-xma)<DBL_EPSILON) {
			xma+=DBL_EPSILON+xma*1e-4;
			xmi-=DBL_EPSILON+xmi*1e-4;
			free(xmin);
			asprintf(&xmin,"%g",xmi);
			vpdi->xmin->validateAndSet(xmin,0,0,0);
			free(xmax);
			asprintf(&xmax,"%g",xma);
			vpdi->xmax->validateAndSet(xmax,0,0,0);
		}
		if(fabs(ymi-yma)<DBL_EPSILON) {
			yma+=DBL_EPSILON+yma*1e-4;
			ymi-=DBL_EPSILON+ymi*1e-4;
			free(ymin);
			asprintf(&ymin,"%g",ymi);
			vpdi->ymin->validateAndSet(ymin,0,0,0);
			free(ymax);
			asprintf(&ymax,"%g",yma);
			vpdi->ymax->validateAndSet(ymax,0,0,0);
		}
	}
	free(xmin);
	free(xmax);
	free(ymin);
	free(ymax);
}

void QtPl::plotlevel()
{
	char *mins, *maxs;
	int pmin, pmax;
	int cont;

	asprintf(&mins,"%d",parea->plmin());
	asprintf(&maxs,"%d",parea->plmax());
	pldi->minl->validateAndSet(mins,0,0,0);
	pldi->maxl->validateAndSet(maxs,0,0,0);

	while(1){
		if(!pldi->exec())
			break;
		cont=0;
		if (sscanf(pldi->minl->text(),"%d",&pmin)!=1) {
			free(mins);
			asprintf(&mins,"%d",parea->plmin());
			pldi->minl->validateAndSet(mins,0,0,0);
			cont=1;
		}
		if (sscanf(pldi->maxl->text(),"%d",&pmax)!=1) {
			free(maxs);
			asprintf(&maxs,"%d",parea->plmax());
			pldi->maxl->validateAndSet(maxs,0,0,0);
			cont=1;
		}
		if (cont)
			continue;

		emit plevel(pmin,pmax);
		break;
	}
	free(mins);
	free(maxs);
}


void QtPl::timerEvent(QTimerEvent *)
{
	if(zt)
		emit zoom(ZTOTAL);
	else if(zfi-zfo>0)
		emit zoom(ZIN_FAST);
	else if(zfo-zfi>0)
		emit zoom(ZOUT_FAST);
	else if(zi-zo>0)
		emit zoom(ZIN);
	else if(zo-zi>0)
		emit zoom(ZOUT);

	if(sfl-sfr>0)
		emit shift(SLEFT_FAST);
	else if(sfr-sfl>0)
		emit shift(SRIGHT_FAST);
	else if(sl-sr>0)
		emit shift(SLEFT);
	else if(sr-sl>0)
		emit shift(SRIGHT);
	
	if(sfu-sfd>0)
		emit shift(SUP_FAST);
	else if(sfd-sfu>0)
		emit shift(SDOWN_FAST);
	else if(su-sd>0)
		emit shift(SUP);
	else if(sd-su>0)
		emit shift(SDOWN);
	
	if(lg)
		emit loga();
	
	zfi=zfo=zi=zo=zt=0;
	sfl=sfr=sfu=sfd=sl=sr=su=sd=0;
	lg=0;
}

ViewPortDialog::ViewPortDialog (QWidget *parent, const char *name ) : 
	QDialog( parent, name, TRUE ) 
{
	QVBoxLayout *topLayout= new QVBoxLayout( this, 10);

	QGridLayout *grid = new QGridLayout(6,2);
	topLayout->addLayout(grid, 10);
	
	ok = new QPushButton( "Ok", this ); 
	ok->setGeometry( 10,10, 100,30 ); 
	connect( ok, SIGNAL(clicked()), SLOT(accept()) ); 
	grid->addWidget(ok,5,0);

	cancel = new QPushButton( "Cancel", this ); 
	cancel->setGeometry( 10,60, 100,30); 
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) ); 
	grid->addWidget(cancel,5,1);

	may=new QLabel("y-max",this);
	grid->addWidget(may,0,1);
	miy=new QLabel("y-min",this);
	grid->addWidget(miy,1,1);

	ymax=new QLineEdit(this,"ymax");
	ymax->validateAndSet("+1.0000e10",0,0,0);
	ymax->setFixedSize(ymax->sizeHint());
	grid->addWidget(ymax,0,0);

	ymin=new QLineEdit(this,"ymin");
	ymin->validateAndSet("-1.0000e10",0,0,0);
	ymin->setFixedSize(ymin->sizeHint());
	grid->addWidget(ymin,1,0);

	xmin=new QLineEdit(this,"xmin");
	xmin->validateAndSet("-1.0000e10",0,0,0);
	xmin->setFixedSize(xmin->sizeHint());
	grid->addWidget(xmin,2,0);

	xmax=new QLineEdit(this,"xmax");
	xmax->validateAndSet("+1.0000e10",0,0,0);
	xmax->setFixedSize(xmax->sizeHint());
	grid->addWidget(xmax,2,1);

	mix=new QLabel("    x-min",this);
	grid->addWidget(mix,3,0);
	max=new QLabel("    x-max",this);
	grid->addWidget(max,3,1);
}

PrintDialog::PrintDialog( QWidget *parent, const char *name ) : 
	QDialog( parent, name, TRUE ) 
{
	topLayout = new QVBoxLayout( this, 10 );

	grid = new QGridLayout( 9, 2 );
	topLayout->addLayout( grid, 10 );

	ok = new QPushButton( "Ok", this ); 
	ok->setGeometry( 10,10, 100,30 ); 
	connect( ok, SIGNAL(clicked()), SLOT(accept()) ); 
	grid->addWidget(ok,8,0);

	cancel = new QPushButton( "Cancel", this ); 
	cancel->setGeometry( 10,60, 100,30); 
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) ); 
	grid->addWidget(cancel,8,1);

	xs=new QLabel("x-size/mm",this);
	grid->addWidget(xs,0,0);
	ys=new QLabel("y-size/mm",this);
	grid->addWidget(ys,0,1);
	
	dv=new QDoubleValidator(5,1000,2,this);
	
	xsize=new QLineEdit(this,"xsize");
	xsize->setValidator(dv);
	xsize->validateAndSet("150",0,0,0);
	xsize->setFixedSize(xsize->sizeHint());
	grid->addWidget(xsize,1,0);

	ysize=new QLineEdit(this,"ysize");
	ysize->setValidator(dv);
	ysize->validateAndSet("92.71",0,0,0);
	ysize->setFixedSize(ysize->sizeHint());
	grid->addWidget(ysize,1,1);

	xl=new QLabel("x-label",this);
	grid->addWidget(xl,2,0);
	yl=new QLabel("y-label",this);
	grid->addWidget(yl,2,1);

	xlabel=new QLineEdit(this,"xlabel");
	xlabel->validateAndSet("x",0,0,0);
	xlabel->setFixedSize(xlabel->sizeHint());
	grid->addWidget(xlabel,3,0);

	ylabel=new QLineEdit(this,"ylabel");
	ylabel->validateAndSet("y",0,0,0);
	ylabel->setFixedSize(ylabel->sizeHint());
	grid->addWidget(ylabel,3,1);

	xu=new QLabel("x-unit",this);
	grid->addWidget(xu,4,0);
	yu=new QLabel("y-unit",this);
	grid->addWidget(yu,4,1);

	xunit=new QLineEdit(this,"xunit");
	xunit->validateAndSet("",0,0,0);
	xunit->setFixedSize(xunit->sizeHint());
	grid->addWidget(xunit,5,0);

	yunit=new QLineEdit(this,"yunit");
	yunit->validateAndSet("",0,0,0);
	yunit->setFixedSize(yunit->sizeHint());
	grid->addWidget(yunit,5,1);
	
	fn=new QLabel("file name",this);
	grid->addWidget(fn,6,0);

	filename=new QLineEdit(this,"filename");
	filename->validateAndSet("tmp.pspl",0,0,0);
	grid->addWidget(filename,7,0);
}

PlotLevelDialog::PlotLevelDialog (QWidget *parent, const char *name ) : 
	QDialog( parent, name, TRUE ) 
{
	QVBoxLayout *topLayout= new QVBoxLayout( this, 10);

	QGridLayout *grid = new QGridLayout(3,2);
	topLayout->addLayout(grid, 10);
	
	ok = new QPushButton( "Ok", this ); 
	ok->setGeometry( 10,10, 100,30 ); 
	connect( ok, SIGNAL(clicked()), SLOT(accept()) ); 
	grid->addWidget(ok,2,0);

	cancel = new QPushButton( "Cancel", this ); 
	cancel->setGeometry( 10,60, 100,30); 
	connect( cancel, SIGNAL(clicked()), SLOT(reject()) ); 
	grid->addWidget(cancel,2,1);

	mil=new QLabel("min",this);
	grid->addWidget(mil,1,0);
	mal=new QLabel("max",this);
	grid->addWidget(mal,1,1);

	minl=new QLineEdit(this,"minl");
	minl->validateAndSet("0",0,0,0);
	minl->setFixedSize(minl->sizeHint());
	grid->addWidget(minl,0,0);

	maxl=new QLineEdit(this,"maxl");
	maxl->validateAndSet("0",0,0,0);
	maxl->setFixedSize(maxl->sizeHint());
	grid->addWidget(maxl,0,1);
}










