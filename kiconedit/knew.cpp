/*
    KDE Draw - a small graphics drawing program for the KDE
    Copyright (C) 1998  Thomas Tanghus (tanghus@kde.org)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/  

#include <qlayout.h>

#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "knew.h"

QList<KIconTemplate> *KIconTemplateContainer::templatelist = 0;
int KIconTemplateContainer::instances = 0;

void createStandardTemplates(QList<KIconTemplate> *list)
{
    KIconLoader *kil = KGlobal::iconLoader();

    KIconTemplate *it = new KIconTemplate;
    it->path = kil->iconPath("standard", KIcon::User);
    it->title = i18n("Standard file");
    list->append(it);

    it = new KIconTemplate;
    it->path = kil->iconPath("source", KIcon::User);
    it->title = i18n("Source file");
    list->append(it);

    it = new KIconTemplate;
    it->path = kil->iconPath("compressed", KIcon::User);
    it->title = i18n("Compressed file");
    list->append(it);

    it = new KIconTemplate;
    it->path = kil->iconPath("folder", KIcon::User);
    it->title = i18n("Standard folder");
    list->append(it);

    it = new KIconTemplate;
    it->path = kil->iconPath("package", KIcon::User);
    it->title = i18n("Standard package");
    list->append(it);

    it = new KIconTemplate;
    it->path = kil->iconPath("mini-folder", KIcon::User);
    it->title = i18n("Mini folder");
    list->append(it);

    it = new KIconTemplate;
    it->path = kil->iconPath("mini-package", KIcon::User);
    it->title = i18n("Mini package");
    list->append(it);
}

KIconTemplateContainer::KIconTemplateContainer() : QObject()
{
  QString path;
  instances++;
  kdDebug() << "KIconTemplateContainer: instances " << instances << endl;
  if(templatelist)
    return;

  kdDebug() << "KIconTemplateContainer: Creating templates" << endl;

  templatelist = new QList<KIconTemplate>;
  templatelist->setAutoDelete(true);

  QStrList names;
  KConfig *k = kapp->config();
  k->setGroup("Templates");
  k->readListEntry("Names", names);
  for(int i = 0; i < (int)names.count(); i++)
  {
    KIconTemplate *it = new KIconTemplate;
    it->path = k->readEntry(names.at(i));
    it->title = names.at(i);
    //kdDebug() << "Template: " << names.at(i) << "\n" << path.data() << endl;
    templatelist->append(it);
  }

  if(templatelist->count() == 0)
  {
    createStandardTemplates(templatelist);
  }
}

KIconTemplateContainer::~KIconTemplateContainer()
{
  instances--;
  kdDebug() << "KIconTemplateContainer: instances " << instances << endl;
  if(instances == 0)
  {
    kdDebug() << "KIconTemplateContainer: Deleting templates" << endl;
    templatelist->clear();
    delete templatelist;
    kdDebug() << "KIconTemplateContainer: Deleted templates" << endl;
  }
}

QList<KIconTemplate> *KIconTemplateContainer::getTemplates()
{
  KIconTemplateContainer t;
  return t.templatelist;
}

KIconListBoxItem::KIconListBoxItem( KIconTemplate *t )
   : QListBoxItem()
{
  //kdDebug() << "KIconListBoxItem - " << t->path.data() << ", " << t->title.data() << endl;
  icontemplate = t;
  pm.load(t->path);
  setText( t->title );
}

void KIconListBoxItem::paint( QPainter *p )    
{
  p->drawPixmap( 3, 0, pm );      
  QFontMetrics fm = p->fontMetrics();      
  int yPos;                       // vertical text position      
  if ( pm.height() < fm.height() )          
    yPos = fm.ascent() + fm.leading()/2;      
  else            
    yPos = pm.height()/2 - fm.height()/2 + fm.ascent();      
  p->drawText( pm.width() + 5, yPos, text() );    
}

int KIconListBoxItem::height(const QListBox *lb ) const    
{
  return QMAX( pm.height(), lb->fontMetrics().lineSpacing() + 1 );    
}
    
int KIconListBoxItem::width(const QListBox *lb ) const
{
  return pm.width() + lb->fontMetrics().width( text() ) + 6;
}

NewSelect::NewSelect(QWidget *parent) : QWidget( parent )
{
  wiz = (QWizard*) parent;
  grp = new QButtonGroup( this );
  connect( grp, SIGNAL( clicked( int ) ), SLOT( buttonClicked( int ) ) );
  grp->setExclusive( true );
  
  QVBoxLayout* ml = new QVBoxLayout( this, 10 );
  ml->addWidget( grp, 1 );
  //ml->addWidget(grp, 10, AlignLeft);
  QVBoxLayout* l = new QVBoxLayout( grp, 10 );
  
  rbscratch = new QRadioButton( i18n( "Create from scratch" ), grp );
  l->addWidget( rbscratch, 1 );
  //l->addWidget(rbscratch, 5, AlignLeft);
  
  rbtempl = new QRadioButton( i18n( "Create from template" ), grp );
  l->addWidget( rbtempl, 1 );
  //l->addWidget(rbtempl, 5, AlignLeft);
  
  //grp->setMinimumSize(grp->childrenRect().size());
  
  grp->setButton( 0 );
}

NewSelect::~NewSelect()
{
}

void NewSelect::buttonClicked(int id)
{
  kdDebug() << "Button: " << id << endl;

  emit iconopenstyle(id);
}

NewFromTemplate::NewFromTemplate( QWidget* parent )
  : QWidget( parent )
{
  wiz = (QWizard*) parent;
  
  QVBoxLayout* ml = new QVBoxLayout(this, 10);
  
  grp = new QGroupBox( i18n( "Templates:" ), this );
  ml->addWidget( grp, 1 );
  //ml->addWidget(grp, 10, AlignLeft);
  
  QHBoxLayout* l = new QHBoxLayout( grp, 15 );
  
  templates = new KIconListBox( grp );
  connect( templates, SIGNAL( highlighted( int ) ), SLOT( checkSelection( int ) ) );
  l->addWidget( templates );
  
  QList<KIconTemplate> *tlist = KIconTemplateContainer::getTemplates();

  for( int i = 0; i < (int) tlist->count(); i++ )
    templates->insertItem( new KIconListBoxItem( tlist->at( i ) ) );
}

NewFromTemplate::~NewFromTemplate()
{
}

void NewFromTemplate::checkSelection( int )
{
  if( templates->currentItem() != -1 )
    wiz->finishButton()->setEnabled( true );
  else
    wiz->finishButton()->setEnabled( false );
}

KNewIcon::KNewIcon( QWidget* parent )
  : QWizard( parent, 0, true )
{
  setCaption( i18n( "Create new icon" ) );
  resize( 400, 250 );
  
  openstyle = 0;

  finishButton()->setEnabled( true );
  cancelButton()->setEnabled( true );
  nextButton()->setEnabled( false );

  select = new NewSelect( this );
  connect( select, SIGNAL( iconopenstyle( int ) ), SLOT( iconOpenStyle( int ) ) );

  scratch = new KResizeWidget( this, 0, QSize( 32, 32 ) );
  // this doesn't accept default valid size, besides spin buttons won't allow 
  // an invalid size to be set by the user - forces user to change valid default 
  // size to create the new icon object - 
  connect( scratch, SIGNAL( validSize( bool ) ), SLOT( checkPage( bool ) ) );
  
  templ = new NewFromTemplate(this);

  addPage(select, i18n("Select icontype"));
  addPage(scratch, i18n("Create from scratch"));
  //addPage(templ, i18n("Create from template"));
}

KNewIcon::~KNewIcon()
{
  delete select;
  delete scratch;
  delete templ;
}

void KNewIcon::okClicked()
{
  if(openstyle == Blank)
    emit newicon(scratch->getSize());
  else
    emit newicon(templ->path());
  hide();
  setResult(1);
  accept();
}

void KNewIcon::cancelClicked()
{
  hide();
  setResult(0);
  reject();
}

void KNewIcon::iconOpenStyle(int style)
{
  openstyle = style;
  if( act )
    removePage( act );
  if(style == 0)
  {
    act = scratch;
    setNextEnabled( select, true );
    addPage( scratch, i18n( "Create from scratch" ) );
    showPage( select );
  }
  else if( style == 1 )
  {
    act = templ;
    setNextEnabled( select, true );
    addPage( templ, i18n( "Create from template" ) );
    showPage( select );
  }
}

void KNewIcon::checkPage(int p)
{

if(openstyle == Blank)
    finishButton()->setEnabled(true);
else if(p == 0)
    finishButton()->setEnabled(false);
else
    templ->checkSelection(0);

/*
  if(p == 0)
    finishButton()->setEnabled(false);
  else if(openstyle == Blank)
    scratch->checkValue(0);
  else
    templ->checkSelection(0);
*/
}

void KNewIcon::checkPage(bool /* ok */)
{
  if(/* ok && */ openstyle == Blank)
    finishButton()->setEnabled(true);
  else
    finishButton()->setEnabled(false);
}
#include "knew.moc"





