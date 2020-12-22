#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qstring.h>

extern "C" {
#include "ps.h"
}

#include "kdsc.h"

using namespace KDSC;

Document* KDSC::scan( const QString& fileName )
{
    Document* doc = new Document( fileName );
    if( doc->scanDocument() )
	return doc;
    else {
	delete doc;
	return 0;
    }
}

Document::Document( const QString& fileName )
    : _fileName( fileName )
{
    _doc = 0;
}

Document::~Document()
{
    if( _doc )
	psfree( _doc );
    _doc = 0;
}

KGV::OrientationM Document::pageOrientation( unsigned int pageNo )
{
    ASSERT( pageNo < numberOfPages );
    
    // If the orientation is specified in the document we should 
    // return that.
    if( !( pages[ pageNo ]->orientation.isNull() ) ) 
	return pages[ pageNo ]->orientation;
    else if( !defaultPageOrientation.isNull() ) 
	return defaultPageOrientation;
    else if( !orientation.isNull() )
	return orientation;
    else
	return KGV::OrientationM();
}
 
QString Document::pageMedia( unsigned int pageNo )
{
    ASSERT( pageNo < numberOfPages );
    if( !( pages[ pageNo ]->media.isNull() ) ) 
	return pages[ pageNo ]->media;
    else if( !defaultPageMedia.isNull() )
	return defaultPageMedia;
    else
	return QString::null;
}
 
bool Document::scanDocument()
{
    FILE* file = fopen( QFile::encodeName( _fileName), "r" );
    
    if( !file ) 
	return false;
    
    struct document* doc = psscan( file );
    fclose( file );

    if( !doc )
	return false;
  
    pages.setAutoDelete( true );
    
    isEPSF = static_cast< bool >( doc->epsf );

    title = QString( doc->title );
    creationDate = QString( doc->date );
    
    if( doc->pageorder == ASCEND )
	pageOrder = KGV::ASCEND;
    else if( doc->pageorder == DESCEND )
	pageOrder = KGV::DESCEND;
    else if( doc->pageorder == SPECIAL )
	pageOrder = KGV::SPECIAL;

    KGV::BoundingBox bbox = KGV::BoundingBox( doc->boundingbox );
    if( bbox != KGV::BoundingBox() )
	boundingBox = bbox;

    if( doc->orientation == PORTRAIT )
	orientation = KGV::PORTRAIT;
    else if( doc->orientation == LANDSCAPE )
	orientation = KGV::LANDSCAPE;
    else if( doc->orientation == UPSIDEDOWN )
	orientation = KGV::UPSIDEDOWN;
    else if( doc->orientation == SEASCAPE )
	orientation = KGV::SEASCAPE;

    bbox = KGV::BoundingBox( doc->default_page_boundingbox );
    if( bbox != KGV::BoundingBox() )
	defaultPageBoundingBox = bbox;
  
    if( doc->default_page_orientation == PORTRAIT )
	defaultPageOrientation = KGV::PORTRAIT;
    else if( doc->default_page_orientation == LANDSCAPE )
	defaultPageOrientation = KGV::LANDSCAPE;
    else if( doc->default_page_orientation == UPSIDEDOWN )
	defaultPageOrientation = KGV::UPSIDEDOWN;
    else if( doc->default_page_orientation == SEASCAPE )
	defaultPageOrientation = KGV::SEASCAPE;

    if( doc->default_page_media )
	defaultPageMedia = QString( doc->default_page_media->name );

    // Insert the mediatypes specified in the document
    for( unsigned i = 0; i < doc->nummedia; ++i ) {
	documentMedia.append( doc->media[i].name );
	paperSizes.insert( 
		QString( doc->media[i].name ).lower(),
		QSize( doc->media[i].width, doc->media[i].height ) );
    }
    
    numberOfPages = doc->numpages;

    if( doc->lenheader > 0 )
	headerOffset	= KGV::FileOffset( 
				doc->beginheader, doc->endheader, 
				doc->lenheader ); 		
    if( doc->lenpreview > 0 )
	previewOffset	= KGV::FileOffset( 
				doc->beginpreview, doc->endpreview, 
				doc->lenpreview ); 
    if( doc->lendefaults > 0 )
	defaultsOffset	= KGV::FileOffset( 
				doc->begindefaults, doc->enddefaults, 
				doc->lendefaults ); 
    if( doc->lenprolog > 0 )
	prologOffset	= KGV::FileOffset( 
				doc->beginprolog, doc->endprolog, 
				doc->lenprolog ); 
    if( doc->lensetup > 0 )
	setupOffset	= KGV::FileOffset( 
				doc->beginsetup, doc->endsetup, 
				doc->lensetup ); 
    if( doc->lentrailer > 0 )
	trailerOffset	= KGV::FileOffset(
				doc->begintrailer, doc->endtrailer, 
				doc->lentrailer ); 
    
    pages.resize( doc->numpages );

    for( unsigned int i = 0; i < doc->numpages; ++i ) {
	Page* page = new Page;
	page->number = i + 1;
	page->label = QString( doc->pages[i].label );
	if( doc->pages[i].orientation == PORTRAIT )
	    page->orientation = KGV::PORTRAIT;
	else if( doc->pages[i].orientation == LANDSCAPE )
	    page->orientation = KGV::LANDSCAPE;
	else if( doc->pages[i].orientation == UPSIDEDOWN )
	    page->orientation = KGV::UPSIDEDOWN;
	else if( doc->pages[i].orientation == SEASCAPE )
	    page->orientation = KGV::SEASCAPE;
	bbox = KGV::BoundingBox( doc->pages[i].boundingbox );
	if( bbox != KGV::BoundingBox() )
	    page->boundingBox = bbox;
	page->offset = KGV::FileOffset( 
				doc->pages[i].begin, doc->pages[i].end, 
				doc->pages[i].len );
	pages.insert( i, page );
    }

    _doc = doc;

    addStandardMedia();
    addBoundingBoxMedia();

    return true;
}

void Document::addStandardMedia()
{
    documentMedia 
	<< "Letter"    << "Tabloid"   << "Ledger" << "Legal" 
	<< "Statement" << "Executive" << "A4"     << "A5"
	<< "B4"        << "B5"        << "Folio"  << "Quarto"
	<< "10x14";
    
    paperSizes[ "letter"    ] = QSize(  612,  792 );
    paperSizes[ "tabloid"   ] = QSize(  792, 1224 );
    paperSizes[ "ledger"    ] = QSize( 1224,  792 );
    paperSizes[ "legal"     ] = QSize(  612, 1008 );
    paperSizes[ "statement" ] = QSize(  396,  612 );
    paperSizes[ "executive" ] = QSize(  540,  720 );
    paperSizes[ "a3"        ] = QSize(  842, 1190 );
    paperSizes[ "a4"        ] = QSize(  595,  842 );
    paperSizes[ "a5"        ] = QSize(  420,  595 );
    paperSizes[ "b4"        ] = QSize(  729, 1032 );
    paperSizes[ "b5"        ] = QSize(  516,  729 );
    paperSizes[ "folio"     ] = QSize(  612,  936 );
    paperSizes[ "quarto"    ] = QSize(  610,  780 );
    paperSizes[ "10x14"     ] = QSize(  720, 1008 );
}

void Document::addBoundingBoxMedia()
{    
    // Add BoundingBox as a special mediatype and make it the default 
    // mediatype if this document hasn't one
    if( !boundingBox.isNull() ) {
	documentMedia.prepend( "BoundingBox" );
	paperSizes.insert( "boundingbox",
		QSize( boundingBox->width(), boundingBox->height() ) );
	if( defaultPageMedia.isNull() )
	    defaultPageMedia = "BoundingBox";
    }
}	

QString Document::asString()
{
    QString strRep;

    strRep += QString( "DSC representation of file: %1 \n" ).arg( _fileName );

    strRep += QString( "\n" );

    strRep += QString( "EPSF: %1\n" ).arg( isEPSF ? "yes" : "no" );

    if( !title.isNull() )
	strRep += QString( "Title: %1 \n" ).arg( title );

    if( !creationDate.isNull() )
	strRep += QString( "CreationDate: %1 \n" ).arg( creationDate );
    
    if( !pageOrder.isNull() ) {
	strRep += QString( "PageOrder: " );
	switch( pageOrder.data() ) {
	case KGV::ASCEND:
	    strRep += QString( "Ascend\n" );
	    break;
	case KGV::DESCEND:
	    strRep += QString( "Descend\n" );
	    break;
	case KGV::SPECIAL:
	    strRep += QString( "Special\n" );
	    break;
	}
    }

    if( !boundingBox.isNull() )
	strRep += QString( "BoundingBox: %1 %2 %3 %4\n" )
		    .arg( boundingBox->llx() )
		    .arg( boundingBox->lly() )
		    .arg( boundingBox->urx() )
		    .arg( boundingBox->ury() );
	       
    if( !orientation.isNull() )
	strRep += QString( "Orientation: %1 \n" ) 
		    .arg( orientation == KGV::PORTRAIT
			? "Portrait" 
			: "Landscape" );

    strRep += QString( "Pages: %1\n" ).arg( numberOfPages );

    strRep += QString( "\n" );

    strRep += QString( "Page defaults:\n" );
    
    if( !defaultPageBoundingBox.isNull() )
	strRep += QString( "PageBoundingBox: %1 %2 %3 %4\n" )
		    .arg( defaultPageBoundingBox->llx() )
		    .arg( defaultPageBoundingBox->lly() )
		    .arg( defaultPageBoundingBox->urx() )
		    .arg( defaultPageBoundingBox->ury() );

    if( !defaultPageOrientation.isNull() )
	strRep += QString( "PageOrientation: %1 \n" )
		.arg( defaultPageOrientation == KGV::PORTRAIT 
			? "Portrait"
			: "Landscape" );
    
    strRep += QString( "\n" );

    if( !headerOffset.isNull() )
	strRep += QString( "HeaderOffset: %1 - %2\n" )
		    .arg( headerOffset->begin() )
		    .arg( headerOffset->end() );
    if( !previewOffset.isNull() )
	strRep += QString( "PreviewOffset: %1 - %2\n" )
		    .arg( previewOffset->begin() )
		    .arg( previewOffset->end() );
    if( !defaultsOffset.isNull() )
	strRep += QString( "DefaultsOffset: %1 - %2\n" )
		    .arg( defaultsOffset->begin() )
		    .arg( defaultsOffset->end() );
    if( !prologOffset.isNull() )
	strRep += QString( "PrologOffset: %1 - %2\n" )
		    .arg( prologOffset->begin() )
		    .arg( prologOffset->end() );
    if( !setupOffset.isNull() )
	strRep += QString( "SetupOffset: %1 - %2\n" )
		    .arg( setupOffset->begin() )
		    .arg( setupOffset->end() );
    if( !trailerOffset.isNull() )
	strRep += QString( "TrailerOffset: %1 - %2\n" )
		    .arg( trailerOffset->begin() )
		    .arg( trailerOffset->end() );

    strRep += QString( "\n" );
    
    for( unsigned int i = 0; i < numberOfPages; ++i ) {
	strRep += pages[ i ]->asString();
	strRep += "\n";
    }
    
    return strRep;
}

QString Page::asString()
{
    QString strRep;
	
    strRep += QString( "Page %1 %2:\n" ).arg( label ).arg( number );

    if( !boundingBox.isNull() )
	strRep += QString( "BoundingBox: %1 %2 %3 %4\n" )
		    .arg( boundingBox->llx() )
		    .arg( boundingBox->lly() )
		    .arg( boundingBox->urx() )
		    .arg( boundingBox->ury() );

    if( !orientation.isNull() )
	strRep += QString( "Orientation: %1 \n" ) 
		    .arg( orientation == KGV::PORTRAIT
			? "Portrait" 
			: "Landscape" );

    strRep += QString( "Offset: %1 - %2\n" )
		.arg( offset.begin() ).arg( offset.end() );

    return strRep;
}

