#include <qlayout.h>
#include <qapplication.h>
#include <qaccel.h>
#include <qfont.h>
#include <qmenubar.h>
#include <qpainter.h>
#include <qpixmap.h>

#include <qcombobox.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qlineedit.h>

#include <sds/shareddata.h>
#include <sds/color.h>
#include <sds/linestyle.h>
#include <sds/symbolstyle.h>

class WidgetView : public QWidget
{
	Q_OBJECT
public:
	WidgetView( QWidget *parent=0, const char *name=0 );

protected:
	void timerEvent(QTimerEvent *);

private slots:
        void	sdsBoxItemSelected( int );
// 	void    hideBoxClicked();
	void    plevChanged(int);
	void	linestyleBoxItemActivated( int );
	void	linewidthBoxItemActivated( int );
	void	linecolorBoxItemSelected( int );
	void	symbolstyleBoxItemActivated( int );
	void	symbolsizeBoxItemActivated( int );
	void	symbolcolorBoxItemSelected( int );
	void    update();

private:
	QSpinBox  *plev;
	QListBox  *sdsl;
	QComboBox *linestyleb;
	QComboBox *linewidthb;
	QListBox  *linecolorb;
	QComboBox *symbolstyleb;
	QComboBox *symbolsizeb;
	QListBox  *symbolcolorb;
	char **basenames;
	QPixmap **pixmap;
	SharedDataSet **sets;
	int k;
};


//
// Construct the WidgetView with buttons
//

WidgetView::WidgetView( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
	int i,max;
	SharedDataSeg *seg;
	int j;
	QPixmap *pm;
	QColor col;
	QPainter p;
	char tmp[10];

	sets=NULL;
	k=0;

 	col.setNamedColor("ivory2");
	QColorGroup cg(colorGroup().foreground(),
		       colorGroup().background(),
		       colorGroup().light(),
		       colorGroup().dark(),
		       colorGroup().mid(),
		       colorGroup().text(),
		       col
		);
	QPalette cp(cg,cg,cg);

	// Set the window caption/title
	
	setCaption( "SDS-QPL Manager" );

	// Install an application-global event filter

//    qApp->installEventFilter( this );

	// Create a layout to position the widgets

	QVBoxLayout *topLayout = new QVBoxLayout( this, 10 );

	// Create a grid layout to hold the widgets

	QGridLayout *grid = new QGridLayout( 4, 3 );

	// This layout will get all of the stretch
	topLayout->addLayout( grid, 10 );

	// Create a menubar
	QMenuBar *menubar = new QMenuBar( this );
	
	QPopupMenu* popup = new QPopupMenu;
	popup->insertItem( "&Quit", qApp, SLOT(quit()), ALT+Key_Q );

	QPopupMenu* view = new QPopupMenu();
	CHECK_PTR( view );
	view->insertItem("&update", this, SLOT(update()), Key_U );

	menubar->insertItem( "&File", popup );
	menubar->insertItem("&View", view);


	// Must tell the layout about a menubar in a widget
	topLayout->setMenuBar( menubar );

	// Create a list box
	sdsl = new QListBox( this, "listBox" );
	sdsl->setPalette(cp);
	seg=new SharedDataSeg();
	seg->list(&sets,&k,".");
	basenames=(char **)malloc(sizeof(char *)*k);
	pixmap=(QPixmap **)malloc(sizeof(QPixmap *)*k);
	for(j=0;sets[j];j++) {	// fill list box
		pixmap[j]=new QPixmap(300,20);
		col.setNamedColor("ivory2");
		pixmap[j]->fill(col);
		p.begin(pixmap[j]);
		if(sets[j]->linestyle()!=NOPEN) {
			col.setRgb(sets[j]->r(),
				   sets[j]->g(),
				   sets[j]->b());
			p.fillRect(0,0,20,20,col);
		}
		if(sets[j]->symbolstyle()!=NOSYMBOL) {
			col.setRgb(sets[j]->sr(),
				   sets[j]->sg(),
				   sets[j]->sb());
			p.fillRect(5,5,10,10,col);
		}
		sprintf(tmp,"%3d",sets[j]->plev());
		p.drawText(25,0,20,20,AlignLeft|AlignVCenter,tmp);
		p.drawText(50,0,250,20,AlignLeft|AlignVCenter,
			   sets[j]->sdsname());
		basenames[j]=strdup(sets[j]->sdsname());
		p.end();
		sdsl->insertItem(*pixmap[j]);
	}
	delete seg;

	grid->addMultiCellWidget( sdsl, 0, 3, 0, 0 );
	connect( sdsl, SIGNAL(selected(int)), SLOT(sdsBoxItemSelected(int)) );
	
	plev = new QSpinBox(-1,999,1,this,"plev");
	plev->setPrefix("Plot Level: ");
	plev->setPalette(cp);
// 	col.setNamedColor("black");
// 	plev->setBackgroundColor(col);
// 	plev->setBackgroundMode(FixedColor);
	plev->setFixedHeight(plev->sizeHint().height());
	connect(plev,SIGNAL(valueChanged(int)),SLOT(plevChanged(int)));
	grid->addWidget(plev,0,1);

	// Create a check box
// 	hideb = new QCheckBox( this );
// 	hideb->setText( "Hide" );
// 	hideb->setMinimumSize( hideb->sizeHint() );
// 	hideb->setFixedHeight(hideb->sizeHint().height());
// 	connect( hideb, SIGNAL(clicked()), SLOT(hideBoxClicked()) );
// 	grid->addWidget(hideb,0,1);

	// Create a combo box
	linestyleb = new QComboBox( FALSE, this, "LineStyleBox" );
	for(i=0,max=maxlinestyle();i<max;i++)
	{
		linestyleb->insertItem(linestyle[i]);
	}
	linestyleb->setFixedHeight(linestyleb->sizeHint().height());
	connect( linestyleb, SIGNAL(activated(int)), 
		 SLOT(linestyleBoxItemActivated(int)) );
	grid->addWidget(linestyleb,1,1);

	// Create a combo box
	linewidthb = new QComboBox( FALSE, this, "LineWidthBox" );
	for(i=0;i<10;i++)
	{
		QString str;
		str.sprintf( "%d Pixel", i );
		linewidthb->insertItem(str);
	}
	linewidthb->setFixedHeight(linewidthb->sizeHint().height());
	connect( linewidthb, SIGNAL(activated(int)), 
		 SLOT(linewidthBoxItemActivated(int)) );
	grid->addWidget(linewidthb,2,1);

	// Create a list box
	linecolorb = new QListBox( this, "LineColorBox" );
	linecolorb->setPalette(cp);
	
	for ( i=0,max=maxcolor(); i<max; i++ ) {		// fill list box
		pm=new QPixmap(114,20);
		col.setNamedColor("ivory2");
		pm->fill(col);
		col.setRgb(colors[i].r,colors[i].g,colors[i].b);
		p.begin(pm);
		p.fillRect(0,0,20,20,col);
		p.drawText(25,0,89,20,AlignLeft|AlignVCenter,colors[i].name);
		p.end();
		linecolorb->insertItem( *pm );
	}
	linecolorb->setFixedWidth(140);
	grid->addWidget( linecolorb, 3, 1 );
	connect( linecolorb, SIGNAL(selected(int)), 
		 SLOT(linecolorBoxItemSelected(int)) );

	// Create a combo box
	symbolstyleb = new QComboBox( FALSE, this, "SymbolStyleBox" );
	for(i=0,max=maxsymbolstyle();i<max;i++)
	{
		symbolstyleb->insertItem(symbolstyle[i]);
	}
	symbolstyleb->setFixedHeight(symbolstyleb->sizeHint().height());
	connect( symbolstyleb, SIGNAL(activated(int)), 
		 SLOT(symbolstyleBoxItemActivated(int)) );
	grid->addWidget(symbolstyleb,1,2);

	// Create a combo box
	symbolsizeb = new QComboBox( FALSE, this, "SymbolSizeBox" );
	for(i=1;i<20;i++)
	{
		QString str;
		str.sprintf( "%d Pixel", i );
		symbolsizeb->insertItem(str);
	}
	symbolsizeb->setFixedHeight(symbolsizeb->sizeHint().height());
	connect( symbolsizeb, SIGNAL(activated(int)), 
		 SLOT(symbolsizeBoxItemActivated(int)) );
	grid->addWidget(symbolsizeb,2,2);

	// Create a list box
	symbolcolorb = new QListBox( this, "SymbolColorBox" );
	symbolcolorb->setPalette(cp);
	
	for ( i=0,max=maxcolor(); i<max; i++ ) {		// fill list box
		pm=new QPixmap(114,20);
		col.setNamedColor("ivory2");
		pm->fill(col);
		col.setRgb(colors[i].r,colors[i].g,colors[i].b);
		p.begin(pm);
		p.fillRect(0,0,20,20,col);
		p.drawText(25,0,89,20,AlignLeft|AlignVCenter,colors[i].name);
		p.end();
		symbolcolorb->insertItem( *pm );
	}
	symbolcolorb->setFixedWidth(140);
	grid->addWidget( symbolcolorb, 3, 2 );
	connect( symbolcolorb, SIGNAL(selected(int)), 
		 SLOT(symbolcolorBoxItemSelected(int)) );

	startTimer(333);
	topLayout->activate();
}

void WidgetView::update()
{
	int j;
	SharedDataSeg *seg;
	QColor col;
	QPainter p;
	char tmp[10];
	
	sdsl->clear();
	for(j=0;j<k-1;j++) {
		delete pixmap[j];
		free(basenames[j]);
	}

	seg=new SharedDataSeg();
	seg->list(&sets,&k,".");
	basenames=(char **)realloc(basenames,sizeof(char *)*k);
	pixmap=(QPixmap **)realloc(pixmap,sizeof(QPixmap *)*k);
	for(j=0;sets[j];j++) {	// fill list box
		pixmap[j]=new QPixmap(300,20);
		col.setNamedColor("ivory2");
		pixmap[j]->fill(col);
		p.begin(pixmap[j]);
		if(sets[j]->linestyle()!=NOPEN) {
			col.setRgb(sets[j]->r(),
				   sets[j]->g(),
				   sets[j]->b());
			p.fillRect(0,0,20,20,col);
		}
		if(sets[j]->symbolstyle()!=NOSYMBOL) {
			col.setRgb(sets[j]->sr(),
				   sets[j]->sg(),
				   sets[j]->sb());
			p.fillRect(5,5,10,10,col);
		}
		sprintf(tmp,"%3d",sets[j]->plev());
		p.drawText(25,0,20,20,AlignLeft|AlignVCenter,tmp);
		p.drawText(50,0,250,20,AlignLeft|AlignVCenter,
			   sets[j]->sdsname());
		basenames[j]=strdup(sets[j]->sdsname());
		p.end();
		sdsl->insertItem(*pixmap[j]);
		sdsl->setCurrentItem(j);
	}
	delete seg;
}

// void WidgetView::hideBoxClicked()
// {
// 	SharedDataSeg seg;
// 	SharedDataSet *set;
// 	QPainter p;
// 	QColor col;
// 	int ci=sdsl->currentItem();

// 	if(ci<0)
// 		return;

// 	set=seg.item(basenames[ci]);

// 	p.begin(pixmap[ci]);
// 	col.setNamedColor("ivory2");
// 	p.fillRect(0,0,20,20,col);
// 	if(!hideb->isChecked()) {
// 		if(set->linestyle()!=NOPEN) {
// 			col.setRgb(set->r(),set->g(),set->b());
// 			p.fillRect(0,0,20,20,col);
// 		}
// 		if(set->symbolstyle()!=NOSYMBOL) {
// 			col.setRgb(set->sr(),set->sg(),set->sb());
// 			p.fillRect(5,5,10,10,col);
// 		}
// 	}
// 	p.end();
// 	sdsl->changeItem(*pixmap[ci],ci);

// 	if(hideb->isChecked())
// 		set->set_plev(-1);
// 	else
// 		set->set_plev(0);
// 	seg.setChanged(1);
// }

void WidgetView::plevChanged(int nlev)
{
	if(!plev->hasFocus())
		return;
	SharedDataSeg seg;
	SharedDataSet *set;
	QPainter p;
	QColor col;
	char tmp[10];
	int ci=sdsl->currentItem();

	if(ci<0)
		return;

	set=seg.item(basenames[ci]);

	set->set_plev(nlev);
	p.begin(pixmap[ci]);
	col.setNamedColor("ivory2");
	p.fillRect(25,0,20,20,col);
	sprintf(tmp,"%3d",nlev);
	p.drawText(25,0,20,20,AlignLeft|AlignVCenter,tmp);
	p.end();
	sdsl->changeItem(*pixmap[ci],ci);

	seg.setChanged(1);
}

void WidgetView::sdsBoxItemSelected( int index )
{
	SharedDataSeg seg;
	SharedDataSet *set=seg.item(basenames[index]);
	char tmp[10];

	sprintf(tmp,"%3d",set->plev());
	plev->setValue(set->plev());
	linecolorb->setCurrentItem(set->color());
	linecolorb->centerCurrentItem();
	symbolcolorb->setCurrentItem(set->symbolcolor());
	symbolcolorb->centerCurrentItem();
	linestyleb->setCurrentItem(set->linestyle());
	linewidthb->setCurrentItem(set->linewidth());
	symbolstyleb->setCurrentItem(set->symbolstyle());
	symbolsizeb->setCurrentItem(set->symbolsize()-1);
}

void WidgetView::linecolorBoxItemSelected( int index )
{
	SharedDataSeg seg;
	SharedDataSet *set;
	QPainter p;
	QColor col;
	int ci=sdsl->currentItem();

	if(ci<0)
		return;

	set=seg.item(basenames[ci]);

	set->set_color(colors[index].name);
	p.begin(pixmap[ci]);
	if(set->linestyle()!=NOPEN)
		col.setRgb(set->r(),set->g(),set->b());
	else
		col.setNamedColor("ivory2");
	p.fillRect(0,0,20,20,col);
	if(set->symbolstyle()!=NOSYMBOL) {
		col.setRgb(set->sr(),set->sg(),set->sb());
		p.fillRect(5,5,10,10,col);
	}
	p.end();
	sdsl->changeItem(*pixmap[ci],ci);
	
	seg.setChanged(1);
}


void WidgetView::linestyleBoxItemActivated( int index )
{
	SharedDataSeg seg;
	SharedDataSet *set;
	QPainter p;
	QColor col;
	int ci=sdsl->currentItem();

	if(ci<0)
		return;

	set=seg.item(basenames[ci]);
	if(set->linestyle()==NOPEN&&index!=NOPEN) {
		p.begin(pixmap[ci]);
		col.setRgb(set->r(),set->g(),set->b());
		p.fillRect(0,0,20,20,col);
		if(set->symbolstyle()!=NOSYMBOL) {
			col.setRgb(set->sr(),set->sg(),set->sb());
			p.fillRect(5,5,10,10,col);
		}
		p.end();
		sdsl->changeItem(*pixmap[ci],ci);
	} else if(index==NOPEN&&set->linestyle()!=NOPEN) {
		p.begin(pixmap[ci]);
		col.setNamedColor("ivory2");
		p.fillRect(0,0,20,20,col);
		if(set->symbolstyle()!=NOSYMBOL) {
			col.setRgb(set->sr(),set->sg(),set->sb());
			p.fillRect(5,5,10,10,col);
		}
		p.end();
		sdsl->changeItem(*pixmap[ci],ci);
	}

	set->set_linestyle(index);
	seg.setChanged(1);
}

void WidgetView::linewidthBoxItemActivated( int index )
{
	SharedDataSeg seg;
	SharedDataSet *set;

	if(sdsl->currentItem()<0)
		return;

	set=seg.item(basenames[sdsl->currentItem()]);

	set->set_linewidth(index);
	seg.setChanged(1);
}

void WidgetView::symbolcolorBoxItemSelected( int index )
{
	SharedDataSeg seg;
	SharedDataSet *set;
	QPainter p;
	QColor col;
	int ci=sdsl->currentItem();

	if(ci<0)
		return;

	set=seg.item(basenames[ci]);

	set->set_symbolcolor(colors[index].name);
	col.setRgb(set->sr(),set->sg(),set->sb());
	if(set->symbolstyle()!=NOSYMBOL) {
		p.begin(pixmap[ci]);
		p.fillRect(5,5,10,10,col);
		p.end();
	}
	sdsl->changeItem(*pixmap[ci],ci);
	
	seg.setChanged(1);
}


void WidgetView::symbolstyleBoxItemActivated( int index )
{
	SharedDataSeg seg;
	SharedDataSet *set;
	QPainter p;
	QColor col;
	int ci=sdsl->currentItem();

	if(ci<0)
		return;

	set=seg.item(basenames[ci]);
	if(set->symbolstyle()==NOSYMBOL&&index!=NOSYMBOL) {
		p.begin(pixmap[ci]);
		col.setRgb(set->sr(),set->sg(),set->sb());
		p.fillRect(5,5,10,10,col);
		p.end();
		sdsl->changeItem(*pixmap[ci],ci);
	} else if(index==NOSYMBOL&&set->symbolstyle()!=NOSYMBOL) {
		p.begin(pixmap[ci]);
		if(set->linestyle()==NOPEN)
			col.setNamedColor("ivory2");
		else 
			col.setRgb(set->r(),set->g(),set->b());
		p.fillRect(0,0,20,20,col);
		p.end();
		sdsl->changeItem(*pixmap[ci],ci);
	}

	set->set_symbolstyle(index); 
	seg.setChanged(1); 
}

void WidgetView::symbolsizeBoxItemActivated( int index )
{
	SharedDataSeg seg;
	SharedDataSet *set;

	if(sdsl->currentItem()<0)
		return;

	set=seg.item(basenames[sdsl->currentItem()]);

	set->set_symbolsize(index+1);
	seg.setChanged(1);
}

//
// Include the meta-object code for classes in this file
//

#include "qm.moc"


//
// Create and display our WidgetView.
//

void WidgetView::timerEvent(QTimerEvent *)
{
	SharedDataSeg *seg;
	int rep=0;
	static unsigned int version=0;
	
	seg= new SharedDataSeg();
	if(seg->ChangedVer()>version){
		rep=1;
		version=seg->ChangedVer();
	}
	delete seg;
	
	if(rep)
		emit update();
};

int main( int argc, char **argv )
{
    QApplication::setColorSpec( QApplication::CustomColor );
    QApplication a( argc, argv );
    WidgetView w;
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
