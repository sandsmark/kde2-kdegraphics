/*  This file is part of the KDE project
    Copyright (C) 2000 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "kfilteraction.h"

#include "filtlist.h"
#include "filter.h"

#include <qstring.h>
#include <qobject.h>
#include <qiconset.h>
#include <qstrlist.h>

#include <kaction.h>
#include <kdebug.h>

#include <assert.h>

KFilterAction::KFilterAction( KFilterList *filters, const QString& text, QObject* parent, const char* name )
        : KActionMenu( text, parent, name ),
        m_filters( filters ),
        m_actionCollection( parent ),
        m_menus( new QDict<KActionMenu> )
{
  createMenu();
}

KFilterAction::KFilterAction( KFilterList *filters, const QString& text, const QIconSet& icon, QObject* parent, const char* name )
        : KActionMenu( text, icon, parent, name ),
        m_filters( filters ),
        m_actionCollection( parent ),
        m_menus( new QDict<KActionMenu> )
{
  createMenu();
}

KFilterAction::KFilterAction( KFilterList *filters, const QString& text, const QString& icon, QObject* parent, const char* name )
        : KActionMenu( text, icon, parent, name ),
        m_filters( filters ),
        m_actionCollection( parent ),
        m_menus( new QDict<KActionMenu> )
{
  createMenu();
}

KFilterAction::~KFilterAction()
{
  delete m_menus; m_menus = 0L;
}

void KFilterAction::createMenu()
{
  assert( m_menus );
  //remove old menus
  m_menus->clear();

  //create new ones
  for( int i = 0; i < m_filters->count(); ++i )
  {
    KImageFilter *filter = m_filters->filter( i );

    QString name;
    KActionMenu *menu = getItemMenu( name, this, filter->name() );

    if( 0 == menu )
    {
      kdWarning() << "couldn't get menu for " << filter->name() << endl;
      continue;
    }

    KAction *entry = new KAction( name, 0, filter, SLOT( slotRaiseFilter() ), m_actionCollection, name.latin1() );
    menu->insert( entry );
  }
}

KActionMenu *KFilterAction::getItemMenu( QString &name, KActionMenu *parent, const QString &fullname )
{
  KActionMenu *menu = 0;

  QStringList tokens = QStringList::split( ':', fullname );
  int count = tokens.count();

  if( count <= 1 )
  {
    name = count ? fullname : QString::fromLatin1( "" );
    return parent;
  }

  QString accum = "";
  for( QStringList::Iterator it = tokens.begin();
       it != tokens.end() && it != tokens.at( count - 1 );
       ++it )
  {
    accum += *it;
    accum += ':';
    menu = m_menus->find( accum );

    if( 0 == menu )
    {
      menu = new KActionMenu( *it, m_actionCollection, (*it).latin1() );
      m_menus->insert( accum, menu );

      parent->insert( menu );

      parent = menu;
    }
  }

  name = tokens.last();

  return menu;
}

#include "kfilteraction.moc"
