#ifndef _KDSC_H_
#define _KDSC_H_

#include <qcstring.h>
#include <qfile.h>
#include <qmap.h>
#include <qsize.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvector.h>

extern "C" {
#include "ps.h"
}

#include "kgv.h"

namespace KDSC {

class Document;
class Page;

/**
 * Factory method which returns a pointer to the document structure. Returns
 * 0 if the file doesn't exist, or if the file doesn't conform to the DSC
 * conventions.
 */
Document* scan( const QString& fileName );

class Document
{
public:
    friend Document* scan( const QString& fileName );
    ~Document();
      
    bool		isEPSF;
    QString		title;
    QString		creationDate;
    KGV::PageOrderM	pageOrder;
    KGV::BoundingBoxM	boundingBox;
    KGV::OrientationM	orientation;
    KGV::BoundingBoxM	defaultPageBoundingBox;
    KGV::OrientationM	defaultPageOrientation;
    QString		defaultPageMedia;
    
    KGV::FileOffsetM	headerOffset;
    KGV::FileOffsetM	previewOffset;
    KGV::FileOffsetM	defaultsOffset;
    KGV::FileOffsetM	prologOffset;
    KGV::FileOffsetM	setupOffset;
    KGV::FileOffsetM	trailerOffset;
 
    unsigned int	numberOfPages;    
    QVector< Page >	pages;

    QStringList		   documentMedia;
    QMap< QString, QSize > paperSizes;
    
    /**
     * Returns true if this document has a useful page structure. I.e.
     * one that can be used to randomly access the page.
     */
    bool isStructured() const {
	return !isEPSF && numberOfPages > 0 || isEPSF && numberOfPages > 1;
    }
   
    KGV::OrientationM	pageOrientation( unsigned int pageNo );
    QString		pageMedia      ( unsigned int pageNo );
    
    QString asString();
    
    struct document* _doc;

protected:
    Document( const QString& fileName );
    bool scanDocument();

    void addStandardMedia();
    void addBoundingBoxMedia();
    
private:
    QString	    _fileName;
};

class Page
{
public:
    friend class Document;
    
    QString		label;
    unsigned int	number;
    KGV::BoundingBoxM	boundingBox;
    KGV::OrientationM	orientation;
    QString		media;
    
    KGV::FileOffset	offset;

    QString asString();

private:
};

}

#endif
