/*
    KDE Icon Editor - a small graphics drawing program for the KDE
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

#include <stdlib.h>

#include <qlayout.h>

#include <klocale.h>
#include <kcolordlg.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <qcheckbox.h>

#include "kiconconfig.h"

#ifndef PICS_INCLUDED
#define PICS_INCLUDED
#include "pics/logo.xpm"
#endif

KTemplateEditDlg::KTemplateEditDlg(QWidget *parent) : QDialog(parent, 0, true)
{
  ok = cancel = browse = 0L;

  QGroupBox *grp = new QGroupBox(i18n("Template"), this);

  QVBoxLayout *ml = new QVBoxLayout(this, 10);
  QVBoxLayout *l = new QVBoxLayout(grp, 25);

  ln_name = new QLineEdit( this );
  connect( ln_name, SIGNAL( textChanged( const QString & ) ), SLOT( slotTextChanged( const QString & ) ) );

  QLabel* lb_name = new QLabel( ln_name, i18n( "Description:" ), this );

  ln_path = new QLineEdit(this);
  connect( ln_path, SIGNAL( textChanged( const QString & ) ), SLOT( slotTextChanged( const QString & ) ) );

  QLabel* lb_path = new QLabel( ln_path, i18n( "Path:" ), this );

  l->addWidget(lb_name);
  l->addWidget(ln_name);
  l->addWidget(lb_path);
  l->addWidget(ln_path);

  KButtonBox *bbox = new KButtonBox( this );

  ok = bbox->addButton( i18n( "&OK" ) );
  connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
  ok->setEnabled(false);

  cancel = bbox->addButton( i18n( "&Cancel" ) );
  connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );

  bbox->addStretch( 1 );

  browse = bbox->addButton( i18n( "&Browse..." ) );
  connect( browse, SIGNAL( clicked() ), SLOT( slotBrowse() ) );

  bbox->layout();

  ml->addWidget( grp, 1);
  ml->addWidget( bbox );
}

void KTemplateEditDlg::slotTextChanged(const QString &)
{
  QString name = ln_name->text(), path = ln_path->text();
  if(name.length() && path.length())
  {
    if(ok) ok->setEnabled(true);
  }
  else
  {
    if(ok) ok->setEnabled(false);
  }
}

void KTemplateEditDlg::setName(const QString & name)
{
  ln_name->setText(name);
}

QString KTemplateEditDlg::getName()
{
  return ln_name->text();
}

void KTemplateEditDlg::setPath(const QString & path)
{
  ln_path->setText(path);
}

QString KTemplateEditDlg::getPath()
{
  return ln_path->text();
}

void KTemplateEditDlg::slotBrowse()
{
  //  KURL url = KFileDialog::getOpenURL( getenv( "HOME" ), "*.xpm" );
  KURL url = KFileDialog::getImageOpenURL( getenv( "HOME" ) );

  if( url.isEmpty() )
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry( 0L, i18n( "Only local files are supported yet." ) );
    return;
  }

  setPath( url.path() );
}

KTemplateConfig::KTemplateConfig(QWidget *parent) : QWidget(parent)
{
  kdDebug() << "KTemplateConfig constructor" << endl;

  btadd = btremove = btedit = 0L;

  QGroupBox* grp = new QGroupBox( i18n( "Templates:" ), this );
  templates = new KIconListBox( grp );
  connect( templates, SIGNAL( highlighted( int ) ), SLOT( checkSelection( int ) ) );
  connect( templates, SIGNAL(doubleClicked( QListBoxItem * ) ), SLOT( edit() ) );
  QVBoxLayout* ml = new QVBoxLayout( this, 5 );
  ml->addWidget( grp );

  QVBoxLayout* l = new  QVBoxLayout( grp, 15 );
  l->addSpacing( 10 );
  l->addWidget( templates );

  tlist = KIconTemplateContainer::getTemplates();

  for( int i = 0; i < (int) tlist->count(); i++ )
    templates->insertItem( new KIconListBoxItem( tlist->at( i ) ) );

  KButtonBox *bbox = new KButtonBox( grp );

  btadd = bbox->addButton( i18n( "&Add" ) );
  connect( btadd, SIGNAL( clicked() ), SLOT( add() ) );

  btedit = bbox->addButton( i18n( "&Edit" ) );
  connect( btedit, SIGNAL( clicked() ), SLOT( edit() ) );
  btedit->setEnabled( false );

  btremove = bbox->addButton( i18n( "&Remove" ) );
  connect( btremove, SIGNAL( clicked() ), SLOT( remove() ) );
  btremove->setEnabled( false );
		
  bbox->addStretch( 1 );
		
  bbox->layout();
  l->addWidget( bbox );
}

void KTemplateConfig::saveSettings()
{
  kdDebug() << "KTemplateConfig::saveSettings" << endl;

  for(int i = 0; i < (int)templates->count(); i++)
  {
    if(!tlist->containsRef(templates->iconTemplate(i)))
    {
      KIconTemplate *it = new KIconTemplate;
      it->path = templates->path(i);
      it->title = templates->text(i);
      tlist->append(it);
      kdDebug() << "Name: " << templates->text(i) << "\nPath: " << templates->path(i) << endl;
      kdDebug() << "Name: " << it->path << "\nPath: " << it->path << endl;
    }
  }
  kdDebug() << "KTemplateConfig::saveSettings - done" << endl;

}

void KTemplateConfig::checkSelection(int)
{
  kdDebug() << "KTemplateConfig::checkSelection" << endl;
  if(templates->currentItem() != -1)
  {
    if(btremove) btremove->setEnabled(true);
    if(btedit) btedit->setEnabled(true);
  }
  else
  {
    if(btremove) btremove->setEnabled(false);
    if(btedit) btedit->setEnabled(false);
  }
  kdDebug() << "KTemplateConfig::checkSelection - done" << endl;
}

void KTemplateConfig::remove()
{
  templates->removeItem(templates->currentItem());
}

void KTemplateConfig::add()
{
  KTemplateEditDlg dlg(this);
  if(dlg.exec())
  {
    KIconTemplate *it = new KIconTemplate;
    it->path = dlg.getPath();
    it->title = dlg.getName();
    //tlist->append(it);
    templates->insertItem(new KIconListBoxItem(it));
  }
}

void KTemplateConfig::edit()
{
  KTemplateEditDlg dlg(this);
  dlg.setPath(templates->path(templates->currentItem()));
  dlg.setName(templates->text(templates->currentItem()));
  templates->item(templates->currentItem());
  if(dlg.exec())
  {
    KIconTemplate *it=templates->iconTemplate(templates->currentItem());
    int pos=templates->currentItem();
    it->path = dlg.getPath();
    it->title = dlg.getName();

    //refresh list
    templates->clear();
    for( int i = 0; i < (int) tlist->count(); i++ )
        templates->insertItem( new KIconListBoxItem( tlist->at( i ) ) );
    templates->setCurrentItem(pos);
  }
}

KBackgroundConfig::KBackgroundConfig( QWidget* parent )
  : QWidget( parent )
{
  kdDebug() << "KBackgroundConfig - constructor" << endl;

  lb_ex = 0L;

  pprops = KIconEditProperties::getProperties(parent);
  color = pprops->backgroundcolor;

  pixpath = pprops->backgroundpixmap;
  pix.load(pixpath);
  if(pix.isNull())
  {
    kdDebug() << "BGPIX: " << pixpath << " not valid!" << endl;
    QPixmap pmlogo((const char**)logo);
    pix = pmlogo;
  }

  QVBoxLayout *mainLayout = new QVBoxLayout( this, 5 );

  QGroupBox *grp1 = new QGroupBox( i18n( "Color or pixmap" ), this );
  mainLayout->addWidget( grp1 );

  QGridLayout *grp1Layout = new QGridLayout( grp1, 3, 2, 15 );

  QButtonGroup* btngrp = new QButtonGroup( grp1 );
  btngrp->setExclusive( true );
  btngrp->setFrameStyle( QFrame::NoFrame );
  connect( btngrp, SIGNAL( clicked( int ) ), SLOT( slotBackgroundMode( int ) ) );
  grp1Layout->addWidget( btngrp, 0, 0 );

  QVBoxLayout *bgl = new QVBoxLayout( btngrp, 5 );

  QRadioButton *rbc = new QRadioButton( i18n( "Colored background" ), btngrp );
  btngrp->insert( rbc, 0 );
  bgl->addWidget( rbc );

  QRadioButton *rbp = new QRadioButton( i18n( "Pixmap background" ), btngrp );
  btngrp->insert( rbp, 1 );
  bgl->addWidget( rbp );

  bgl->addStretch( 1 );

  KButtonBox *bbox = new KButtonBox( grp1, Qt::Vertical );
  grp1Layout->addWidget( bbox, 0, 1 );

  btcolor = bbox->addButton( i18n( "&Color..." ) );
  connect( btcolor, SIGNAL( clicked() ), SLOT( selectColor() ) );

  btpix = bbox->addButton( i18n( "&Pixmap..." ) );
  connect( btpix, SIGNAL( clicked() ), SLOT( selectPixmap() ) );

  bbox->layout();

  QRadioButton *rbb = new QRadioButton( i18n( "&Built-in" ), grp1 );
  grp1Layout->addMultiCellWidget( rbb, 1, 1, 0, 1, AlignHCenter );

  grp1Layout->setRowStretch( 2, 1 );

  QGroupBox *grp2 = new QGroupBox( i18n( "Example" ), this );
  mainLayout->addWidget( grp2, 1 );

  QBoxLayout *l2 = new QVBoxLayout( grp2, 15 );

  l2->addSpacing( 10 );

  lb_ex = new QLabel( grp2 );
  lb_ex->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  l2->addWidget( lb_ex );

/*
  l1->addWidget( btngrp, 0, AlignLeft );
  l1->addLayout( l1r );
*/

  bgmode = pprops->backgroundmode;
  if( bgmode == QWidget::FixedPixmap )
  {
    btngrp->setButton( 1 );
    btcolor->setEnabled( false );
    lb_ex->setBackgroundPixmap( pix );
  }
  else
  {
    btngrp->setButton( 0 );
    btpix->setEnabled( false );
    lb_ex->setBackgroundColor( color );
  }
}

KBackgroundConfig::~KBackgroundConfig()
{
}

void KBackgroundConfig::slotBackgroundMode(int id)
{
  if(id == 0)
  {
    bgmode = QWidget::FixedColor;
    btpix->setEnabled(false);
    btcolor->setEnabled(true);
    if(lb_ex)
      lb_ex->setBackgroundColor(color);
  }
  else
  {
    bgmode = QWidget::FixedPixmap;
    btpix->setEnabled(true);
    btcolor->setEnabled(false);
    if(lb_ex)
      lb_ex->setBackgroundPixmap(pix);
  }
}

void KBackgroundConfig::saveSettings()
{
  kdDebug() << "KBackgroundConfig::saveSettings" << endl;
  Properties *pprops = props(this);
  pprops->backgroundmode = bgmode;
  pprops->backgroundpixmap = pixpath;
  pprops->backgroundcolor = color;
  kdDebug() << "KBackgroundConfig::saveSettings - done" << endl;
}

void KBackgroundConfig::selectColor()
{
  QColor c;
  if(KColorDialog::getColor(c))
  {
    lb_ex->setBackgroundColor(c);
    color = c;
  }
}

void KBackgroundConfig::selectPixmap()
{
  //  KURL url = KFileDialog::getOpenURL("/", "*.xpm");
  KURL url = KFileDialog::getImageOpenURL("/");

  if( url.isEmpty() )
    return;

  if( !url.isLocalFile() )
  {
    KMessageBox::sorry( 0L, i18n( "Only local files are supported yet." ) );
    return;
  }

  QPixmap p(url.path());

  if( !p.isNull() )
  {
    lb_ex->setBackgroundPixmap( p );
    pixpath = url.path();
  }
}

KToolBarConfig::KToolBarConfig(QWidget *parent) : QWidget(parent)
{
  kdDebug() << "KToolBarConfig - constructor" << endl;

  pprops = props(parent);

  QButtonGroup* btngrptool = new QButtonGroup(i18n("Main Toolbar"), this);
  btngrptool->setExclusive(true);

  QButtonGroup* btngrpdraw = new QButtonGroup(i18n("Drawing tools"), this);
  btngrpdraw->setExclusive(true);

  QLabel *lb = new QLabel(
      i18n("Note: Other settings of the toolbars can be reached\n"
           "from the toolbar \"handle\" or from the \"Options\" menu"), this);

  QBoxLayout *ml = new  QVBoxLayout(this, 5);
  QBoxLayout *lh = new QHBoxLayout();

  ml->addLayout(lh);
  ml->addWidget(lb);

  QBoxLayout *l1 = new  QVBoxLayout(btngrptool, 10);
  QBoxLayout *l2 = new  QVBoxLayout(btngrpdraw, 10);

  lh->addWidget(btngrptool);
  lh->addWidget(btngrpdraw);

  l1->addSpacing(btngrptool->fontMetrics().height());
  l2->addSpacing(btngrptool->fontMetrics().height());

  rb = new QRadioButton(i18n("Show Icon"), btngrptool);
  l1->addWidget(rb, 0, AlignLeft);
  //connect(rbp, SIGNAL(toggled(bool)), SLOT(pasteMode(bool)));

  rb = new QRadioButton(i18n("Show Icon and Text"), btngrptool);
  l1->addWidget(rb, 0, AlignLeft);

  rb = new QRadioButton(i18n("Show Text"), btngrptool);
  l1->addWidget(rb, 0, AlignLeft);

  rb = new QRadioButton(i18n("Show Icon"), btngrpdraw);
  l2->addWidget(rb, 0, AlignLeft);
  //connect(rbp, SIGNAL(toggled(bool)), SLOT(pasteMode(bool)));

  rb = new QRadioButton(i18n("Show Icon and Text"), btngrpdraw);
  l2->addWidget(rb, 0, AlignLeft);

  rb = new QRadioButton(i18n("Show Text"), btngrpdraw);
  l2->addWidget(rb, 0, AlignLeft);

  ml->addStretch(1);

  mstat = pprops->maintoolbartext;
  dstat = pprops->drawtoolbartext;
  btngrptool->setButton(mstat);
  btngrpdraw->setButton(dstat);

  connect(btngrptool, SIGNAL(clicked(int)), SLOT(mainToolBar(int)));
  connect(btngrpdraw, SIGNAL(clicked(int)), SLOT(drawToolBar(int)));

  kdDebug() << "KToolBarConfig - constructor done" << endl;
}

KToolBarConfig::~KToolBarConfig()
{

}

void KToolBarConfig::saveSettings()
{
  kdDebug() << "KToolBarConfig::saveSettings" << endl;

  pprops->maintoolbartext = mstat;
  pprops->drawtoolbartext = dstat;

  pprops->maintoolbar->setIconText((KToolBar::IconText)mstat);
  pprops->drawtoolbar->setIconText((KToolBar::IconText)dstat);

  kdDebug() << "KToolBarConfig::saveSettings - done" << endl;
}

void KToolBarConfig::drawToolBar(int i)
{
  dstat = i;
}

void KToolBarConfig::mainToolBar(int i)
{
  mstat = i;
}

KMiscConfig::KMiscConfig(QWidget *parent) : QWidget(parent)
{
  kdDebug() << "KMiscConfig - constructor" << endl;

  pprops = props(parent);

  QBoxLayout *ml = new QVBoxLayout( this, 5 );

  QGroupBox *grp1 = new QGroupBox( i18n( "Paste mode" ), this );
  ml->addWidget(grp1);

  QBoxLayout *l1 = new  QVBoxLayout(grp1, 20);

  QCheckBox *cbp = new QCheckBox( i18n( "Paste transparent pixels" ), grp1 );
  connect( cbp, SIGNAL( toggled( bool ) ), SLOT( pasteMode( bool ) ) );
  l1->addWidget(cbp);

  QCheckBox *cbr = new QCheckBox( i18n( "Show rulers" ), grp1 );
  connect( cbr, SIGNAL( toggled( bool ) ), SLOT( showRulers( bool ) ) );
  l1->addWidget(cbr);

  l1->addStretch(1);

  pastemode = pprops->pastetransparent;
  if(pastemode)
    cbp->setChecked(true);

  showrulers = pprops->showrulers;
  if(showrulers)
    cbr->setChecked(true);
}

KMiscConfig::~KMiscConfig()
{

}

void KMiscConfig::saveSettings()
{
  kdDebug() << "KMiscConfig::saveSettings" << endl;

  pprops->pastetransparent = pastemode;
  pprops->showrulers = showrulers;
}

void KMiscConfig::pasteMode(bool mode)
{
  pastemode = mode;
}

void KMiscConfig::showRulers(bool mode)
{
  showrulers = mode;
}

KIconConfig::KIconConfig(QWidget *parent)
  : QTabDialog(parent, 0, true)
{
  setCaption( "Configure" );
  setCancelButton();
  connect( this, SIGNAL( applyButtonPressed() ), SLOT( saveSettings() ) );

  pprops = KIconEditProperties::getProperties(parent);
  //keys = k; //new KAccel(parent);
  //dict = new QDict<KKeyEntry>( k->keyDict() );
  //kdDebug() << "Keys: " << k->keyDict().count() << endl;
  //kdDebug() << "Keys: " << dict->count() << endl;
  //CHECK_PTR(dict);
  pprops->keydict = pprops->keys->keyDict();
  kdDebug() << "Keys: " << pprops->keydict.count() << endl;

  keychooser = new KKeyChooser( &pprops->keydict, this );
  addTab( keychooser, i18n( "Keys" ) );

  temps = new KTemplateConfig(this);
  addTab( temps, i18n( "Icon templates" ) );

  backs = new KBackgroundConfig(this);
  addTab( backs, i18n( "Background" ) );

  toolbars = new KToolBarConfig( this );
  addTab( toolbars, i18n( "Toolbars" ) );

  misc = new KMiscConfig( this );
  addTab( misc, i18n( "Icon grid" ) );
}

KIconConfig::~KIconConfig()
{
  //delete dict;
}

void KIconConfig::saveSettings()
{
  kdDebug() << "KIconEditConfig::saveSettings" << endl;

  pprops->keys->setKeyDict( pprops->keydict );
  temps->saveSettings();
  backs->saveSettings();
  misc->saveSettings();
  toolbars->saveSettings();
  accept();
}
#include "kiconconfig.moc"
