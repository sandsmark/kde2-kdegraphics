/**
 * This is essentially the KGhostview widget stripped down and without all the
 * KTMainWindow dependencies (there were quite a bit with menus, toolbars,
 * etc.. accessed all over the place). It would probably make sense to make
 * that use this in the future (mosfet@kde.org).
 */

#ifndef __KGV_MINIWIDGET_H
#define __KGV_MINIWIDGET_H

#include <qsize.h>
#include <qstring.h>

#include <kurl.h>

#include "marklist.h"
#include "kdsc.h"
#include "kpswidget.h"

#include "kgv_view.h"

class QWidget;

class KPrinter;
class KTempFile;

class GotoDialog;
class InfoDialog;
class KGVCopiesPage;
class ScrollBox;

class KGVMiniWidget : public QObject
{
    Q_OBJECT

public:
    KGVMiniWidget( KGVPart* parent = 0, const char* name = 0 );
    ~KGVMiniWidget();

    /**
     * Is a file currently open?
     **/
    bool isFileOpen() {return psfile!=0;}

    /**
     * Open the @em local file @p filename.
     **/
    bool openFile(QString filename);

    /**
     * Tell what was the original URL requested by the user.
     **/
    void setOriginalURL (const KURL &url);

    void setPSWidget( KPSWidget* psWidget );

    /**
     * Enable/disable fancy, document-supplied page labels.
     **/
    void enableFancyPageLabels( bool e = true );
    bool areFancyPageLabelsEnabled () { return _useFancyPageLabels; }
    /**
     * Write settings to application config file.
     **/
    void writeSettings();

    /**
     * A list of page media (sizes).
     **/
    QStringList sizeList() const { return _doc ? _doc->documentMedia : QStringList(); }

    void restoreOverrideOrientation();
    void setOverrideOrientation( KGV::Orientation orientation );
    KGV::Orientation orientation() const;
    KGV::Orientation orientation( int pageNo ) const;

    void restoreOverridePageMedia();
    void setOverridePageMedia( const QString& pageMedia );
    void setOverridePageMedia( int pageMediaIndex );
    QString pageMedia() const;
    QString pageMedia( int pageNo ) const;

    /**
     * Return true if the current page is the first page, false otherwise.
     */
    bool atFirstPage() const;
    /**
     * Return true if the current page is the last page, false otherwise.
     */
    bool atLastPage()  const;

    int currentPage() const { return _currentPage; }
    
    QString pageListToRange( const QValueList<int>& ) const;

    unsigned int magnification() const { return _magstep; }
    void setMagstep( unsigned int magstep );
    
public slots:
    void saveAs();
    bool prevPage();
    bool nextPage();
    void firstPage();
    void lastPage();
    void goToPage();
    void goToPage( int page );
    void print();
    void zoomIn();
    void zoomOut();
    //    void zoom( float zoom_factor );
    /**
     * Redisplay the page if the file has changed on disk.
     **/
    void redisplay();
    void info();
    void fileChanged( const QString& );
    void configureGhostscript();

signals:
    /**
     * Page changed.
     **/
    void newPageShown( int page );

    void setStatusBarText( const QString& );

    /**
     * Emitted if a fileChanged() call fails.
     **/
    void fileChangeFailed();

protected:
    bool setup();
    void buildTOC();
    void showPage( int pagenumber );

    KGV::BoundingBox computeBoundingBox( const QString& pageMedia );
    QSize computePageSize( const QString& pageMedia );

    void magnify( float& dpi, unsigned int magstep );

    void readSettings();

    KPSWidget* page;

    bool savePages( const QString& saveFileName,
		    const QValueList<int>& pageList );

    bool psCopyDoc( const QString& inputFile, const QString& outputFile,
		    const QValueList<int>& pageList );

    bool pdf2dsc( const QString& pdfFile, const QString& dscFile );
    bool convertFromPDF();

    void clearTemporaryFiles();

    FILE		*psfile;
    int			_currentPage;
    KDSC::Document*	_doc;
    struct 		document *doc;

protected slots:
    void pdf2dscExited();

private:
    unsigned int shrink_magsteps, expand_magsteps;
    unsigned int _magstep;
    QString page_total_label;
    QString page_string;
    QString page_label_text;
    QString position_label;
    bool _useFancyPageLabels;

    float _defaultXdpi, _defaultYdpi;
    int hintwidth;

    KURL origurl;
    QString _fileName;
    QString _pdfFileName;

    MarkList* _marklist;

    InfoDialog *mInfoDialog;
    GotoDialog *mGotoDialog;

    KPrinter* _printer;
    QWidget* _widget;

    enum DocFormat {PS, PDF};
    DocFormat format;

    KTempFile* _tmpStdin;
    KTempFile* _tmpUnzipped;
    KTempFile* _tmpFromPDF;
    KTempFile* _tmpDSC;

    KGV::OrientationM	_overrideOrientation;
    QString		_overridePageMedia;
    QString		_defaultPageMedia;

    QString _interpreterPath;


    int	toc_length;
    int	toc_entry_length;
};

#endif
