#include "htmlexport.h"
#include "imagelistitem.h"
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qimage.h>
#include <klocale.h>
#include <knuminput.h>
#include <kcolorbtn.h>
#include <kfiledialog.h>
#include <kbuttonbox.h>
#include <kmessagebox.h>
#include <kprogress.h>
#include <kapp.h>

#include <math.h>

KIFHtmlExport::KIFHtmlExport(QListBox *urlListBox, QWidget *parent,
                             const char *name)
    : QDialog(parent, name, true)
{
    if(urlListBox->count() == 0){
        // FIXME
        KMessageBox::error(this, i18n("No files selected for HTML!\n\
You must first select some images."), i18n("No files selected."));
        accept();
    }

    urlList = urlListBox;
    //QVBoxLayout *layout = new QVBoxLayout(this, 4);
    QGridLayout *layout = new QGridLayout(this, 1, 1, 4);

    QGroupBox *genGrp = new QGroupBox(i18n("General Options"), this);
    QVBoxLayout *genLayout = new QVBoxLayout(genGrp, 4);
    genLayout->addSpacing(14);

    QLabel *lbl = new QLabel(i18n("Output Directory:"), genGrp);
    genLayout->addWidget(lbl);

    QHBoxLayout *hLayout = new QHBoxLayout;
    genLayout->addLayout(hLayout);
    outputPathEdit = new QLineEdit(QDir::currentDirPath(), genGrp);
    hLayout->addWidget(outputPathEdit);
    QPushButton *browseBtn = new QPushButton(i18n("Browse"), genGrp);
    connect(browseBtn, SIGNAL(clicked()), this, SLOT(slotBrowseOutputDir()));
    hLayout->addWidget(browseBtn);

    lbl = new QLabel(i18n("HTML base name:"), genGrp);
    genLayout->addWidget(lbl);
    baseHTMLEdit = new QLineEdit(i18n("thumbnail"), genGrp);
    genLayout->addWidget(baseHTMLEdit);

    rowInput = new KIntNumInput(5, genGrp);
    rowInput->setRange(1, 40);
    rowInput->setLabel(i18n("Rows Per Page:"));
    genLayout->addWidget(rowInput);

    columnInput = new KIntNumInput(5, genGrp);
    columnInput->setRange(1, 20);
    columnInput->setLabel(i18n("Columns:"));
    genLayout->addWidget(columnInput);

    sizeInput = new KIntNumInput(90, genGrp);
    sizeInput->setRange(16, 150);
    sizeInput->setLabel(i18n("Thumb size:"));
    genLayout->addWidget(sizeInput);


    hLayout = new QHBoxLayout;
    genLayout->addLayout(hLayout);
    lbl = new QLabel(i18n("Background:"), genGrp);
    hLayout->addWidget(lbl);
    bgColorBtn = new KColorButton(Qt::white, genGrp);
    hLayout->addWidget(bgColorBtn);

    hLayout = new QHBoxLayout;
    genLayout->addLayout(hLayout);
    lbl = new QLabel(i18n("Foreground:"), genGrp);
    hLayout->addWidget(lbl);
    fgColorBtn = new KColorButton(Qt::black, genGrp);
    hLayout->addWidget(fgColorBtn);

    indexCB = new QCheckBox(i18n("Show page index."), genGrp);
    indexCB->setChecked(true);
    genLayout->addWidget(indexCB);
    fileNameCB = new QCheckBox(i18n("Show image names"), genGrp);
    genLayout->addWidget(fileNameCB);

    genLayout->addStretch(1);

    layout->addMultiCellWidget(genGrp, 0, 3, 0, 0);

    QGroupBox *nextGrp = new QGroupBox(i18n("Next button"), this);
    QGridLayout *gLayout = new QGridLayout(nextGrp, 1, 1, 4);
    gLayout->addRowSpacing(0, 14);
    lbl = new QLabel(i18n("Text:"), nextGrp);
    gLayout->addWidget(lbl, 1, 0);
    nextStrEdit = new QLineEdit(i18n("Next"), nextGrp);
    gLayout->addWidget(nextStrEdit, 1, 1);
    nextImgCB = new QCheckBox(i18n("Use image for button."), nextGrp);
    connect(nextImgCB, SIGNAL(toggled(bool)), this,
            SLOT(slotNextImgToggled(bool)));
    gLayout->addMultiCellWidget(nextImgCB, 2, 2, 0, 1);
    nextFileEdit = new QLineEdit(nextGrp);
    gLayout->addWidget(nextFileEdit, 3, 0);
    nextBrowseBtn = new QPushButton(i18n("Browse"), nextGrp);
    connect(nextBrowseBtn, SIGNAL(clicked()), this, SLOT(slotBrowseNextFile()));
    gLayout->addWidget(nextBrowseBtn, 3, 1);

    layout->addWidget(nextGrp, 0, 1);

    QGroupBox *prevGrp = new QGroupBox(i18n("Previous button"), this);
    gLayout = new QGridLayout(prevGrp, 1, 1, 4);
    gLayout->addRowSpacing(0, 14);
    lbl = new QLabel(i18n("Text:"), prevGrp);
    gLayout->addWidget(lbl, 1, 0);
    prevStrEdit = new QLineEdit(i18n("Back"), prevGrp);
    gLayout->addWidget(prevStrEdit, 1, 1);
    prevImgCB = new QCheckBox(i18n("Use image for button."), prevGrp);
    connect(prevImgCB, SIGNAL(toggled(bool)), this,
            SLOT(slotPrevImgToggled(bool)));
    gLayout->addMultiCellWidget(prevImgCB, 2, 2, 0, 1);
    prevFileEdit = new QLineEdit(prevGrp);
    gLayout->addWidget(prevFileEdit, 3, 0);
    prevBrowseBtn = new QPushButton(i18n("Browse"), prevGrp);
    connect(prevBrowseBtn, SIGNAL(clicked()), this, SLOT(slotBrowseNextFile()));
    gLayout->addWidget(prevBrowseBtn, 3, 1);

    layout->addWidget(prevGrp, 1, 1);

    QGroupBox *homeGrp = new QGroupBox(i18n("Home button"), this);
    gLayout = new QGridLayout(homeGrp, 1, 1, 4);
    gLayout->addRowSpacing(0, 14);
    lbl = new QLabel(i18n("Text:"), homeGrp);
    gLayout->addWidget(lbl, 1, 0);
    homeStrEdit = new QLineEdit(i18n("Home"), homeGrp);
    gLayout->addWidget(homeStrEdit, 1, 1);
    homeImgCB = new QCheckBox(i18n("Use image for button."), homeGrp);
    connect(homeImgCB, SIGNAL(toggled(bool)), this,
            SLOT(slotHomeImgToggled(bool)));
    gLayout->addMultiCellWidget(homeImgCB, 2, 2, 0, 1);
    homeFileEdit = new QLineEdit(homeGrp);
    gLayout->addWidget(homeFileEdit, 3, 0);
    homeBrowseBtn = new QPushButton(i18n("Browse"), homeGrp);
    connect(homeBrowseBtn, SIGNAL(clicked()), this, SLOT(slotBrowseHomeFile()));
    gLayout->addWidget(homeBrowseBtn, 3, 1);

    layout->addWidget(homeGrp, 2, 1);

    QGroupBox *logoGrp = new QGroupBox(i18n("Logo button"), this);
    gLayout = new QGridLayout(logoGrp, 1, 1, 4);
    gLayout->addRowSpacing(0, 14);
    lbl = new QLabel(i18n("Text:"), logoGrp);
    gLayout->addWidget(lbl, 1, 0);
    logoStrEdit = new QLineEdit(i18n("Web Thumbnails"), logoGrp);
    gLayout->addWidget(logoStrEdit, 1, 1);
    logoImgCB = new QCheckBox(i18n("Use image for button."), logoGrp);
    connect(logoImgCB, SIGNAL(toggled(bool)), this,
            SLOT(slotLogoImgToggled(bool)));
    gLayout->addMultiCellWidget(logoImgCB, 2, 2, 0, 1);
    logoFileEdit = new QLineEdit(logoGrp);
    gLayout->addWidget(logoFileEdit, 3, 0);
    logoBrowseBtn = new QPushButton(i18n("Browse"), logoGrp);
    connect(logoBrowseBtn, SIGNAL(clicked()), this, SLOT(slotBrowseNextFile()));
    gLayout->addWidget(logoBrowseBtn, 3, 1);

    layout->addWidget(logoGrp, 3, 1);

    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    connect(bbox->addButton(i18n("Cancel")), SIGNAL(clicked()), this,
            SLOT(reject()));
    connect(bbox->addButton(i18n("Make HTML")), SIGNAL(clicked()), this,
            SLOT(slotGenerateHTML()));
    layout->setRowStretch(3, 1);
    layout->addMultiCellWidget(bbox, 4, 4, 0, 1);

    slotNextImgToggled(false);
    slotPrevImgToggled(false);
    slotLogoImgToggled(false);
    slotHomeImgToggled(false);
}

void KIFHtmlExport::slotBrowseNextFile()
{
     KURL u = KFileDialog::getOpenURL(QString::null, QString::null,
                                               this, i18n("Open Image"));
     QString str = u.path();
     if(!str.isEmpty())
        nextFileEdit->setText(str);
}

void KIFHtmlExport::slotBrowsePrevFile()
{
    KURL u = KFileDialog::getOpenURL(QString::null, QString::null,
                                               this, i18n("Open Image"));
    QString str = u.path();
    if(!str.isEmpty())
        prevFileEdit->setText(str);

}

void KIFHtmlExport::slotBrowseHomeFile()
{
    KURL u = KFileDialog::getOpenURL(QString::null, QString::null,
                                               this, i18n("Open Image"));
    QString str = u.path();
    if(!str.isEmpty())
        homeFileEdit->setText(str);

}

void KIFHtmlExport::slotBrowseLogoFile()
{
    KURL u = KFileDialog::getOpenURL(QString::null, QString::null,
                                               this, i18n("Open Image"));
    QString str = u.path();
    if(!str.isEmpty())
        logoFileEdit->setText(str);
}

void KIFHtmlExport::slotBrowseOutputDir()
{
    KURL u = KFileDialog::getExistingDirectory(QString::null, this,
                                                    i18n("Output Directory"));
    QString str = u.path();
    if(!str.isEmpty())
        outputPathEdit->setText(str);
}

void KIFHtmlExport::generateHTMLHeader(QTextStream &stream, int page)
{
    stream << "<HTML>\n<HEAD><TITLE=\"Thumbnails\"></TITLE></HEAD>\n";
    stream << QString("<BODY BGCOLOR=\"") + bgColorBtn->color().name() +
        "\" FGCOLOR=\"" + fgColorBtn->color().name() + "\">\n";
    if(logoStrEdit->isEnabled()){ // logo text
        stream << "<H1>" << logoStrEdit->text() << "</H1><P>"
            << i18n("Page: ") << page << "</P>\n";
    }
    else{ // logo image
        ;
    }
}

// this is a mess ;-) I need to split this stuff up :P
void KIFHtmlExport::slotGenerateHTML()
{
    int curPage, thumbCount=0;
    int totalThumbs = urlList->count();
    int pages = (int)ceil(totalThumbs/
                          (rowInput->value()*columnInput->value()));
    QFileInfo fi;

    qWarning("Pages: %d", pages);
    // FIXME - pages is always ending up a even number of row*cols
    for(curPage=0; curPage < pages-1 ; ++curPage){
        QString filePath = outputPathEdit->text() + "/" + baseHTMLEdit->text() +
            QString::number(curPage) + ".html";
        QFile outputFile(filePath);
        if(!outputFile.open(IO_WriteOnly)){
            KMessageBox::error(this, i18n("Unable to open output file!"),
                               i18n("File error."));
            return;
        }
        QTextStream stream(&outputFile);
        generateHTMLHeader(stream, curPage+1);
        // do the index
        if(indexCB->isChecked()){
            // TODO: add home button
            if(curPage != 0){
                stream << "<A HREF=\"" << outputPathEdit->text() << "/"
                    << baseHTMLEdit->text() + QString::number(curPage-1) <<
                    ".html\">" << i18n("Previous |") << "</A>\n";
            }
            int i;
            for(i=0; i < pages-1; ++i){
                if(i != curPage){
                    stream << "<A HREF=\"" << outputPathEdit->text() << "/"
                        << baseHTMLEdit->text() + QString::number(i) <<
                        ".html\">" << QString::number(i+1) << "</A>\n";
                }
                else{
                    stream << QString::number(i+1) << "\n";
                }
            }
            if(curPage != pages-2){
                stream << "<A HREF=\"" << outputPathEdit->text() << "/"
                    << baseHTMLEdit->text() + QString::number(curPage+1) <<
                    ".html\">" << i18n("| Next") << "</A>\n";
            }

        }

        // make the actual thumbnails
        int row, column;
        stream << "<TABLE BORDER=1>\n";
        for(row=0; row < rowInput->value() && thumbCount < totalThumbs ; ++row){
            stream << "<TR>\n";
            for(column=0; column < columnInput->value() && thumbCount < totalThumbs;
                ++column){
                fi.setFile(urlList->text(thumbCount));
                // check if there is an existing thumbnail
                QString thumbStr = fi.dirPath();
                // grr.. all the fileinfo dir stuff returns "file:/...", which
                // screws us up ;-)
                if(thumbStr.left(5) == "file:")
                    thumbStr.remove(0, 5);
                thumbStr += "/.pics/large/" + fi.fileName();
                qWarning("%s", thumbStr.latin1());
                if(sizeInput->value() == 90 && QFile::exists(thumbStr)){
                    qWarning("Found existsing thumbnail");
                    QFile copyInput(thumbStr);
                    QFile copyOutput(outputPathEdit->text() + "/" +
                                     fi.baseName() + "-thumb.png");
                    copyInput.open(IO_ReadOnly); // TODO: error dialog
                    copyOutput.open(IO_WriteOnly);
                    while(!copyInput.atEnd()){
                        copyOutput.putch(copyInput.getch());
                    }
                    copyInput.close();
                    copyOutput.close();
                    stream << "<TD><A HREF=\"" << fi.fileName() <<
                        "\">" << "<IMG SRC=\"" << fi.baseName() <<
                        "-thumb.png\"</IMG>";
                    if(fileNameCB->isChecked())
                        stream << "<BR>" << fi.fileName();
                    stream << "</A></TD>\n";
                }
                else{
                    // generate thumbnail here
                    stream << "<TD><IMG SRC=\"" << fi.fileName() << "\"></TD>\n";
                }
                // copy image to HTML dir if needed
                if(!QFile::exists(outputPathEdit->text() + "/" + fi.fileName())){
                    qWarning("Copying image to HTML dir");
                    QString srcStr = urlList->text(thumbCount);
                    if(srcStr.left(5) == "file:")
                        srcStr.remove(0, 5);
                    QFile copyInput(srcStr);
                    QFile copyOutput(outputPathEdit->text() + "/" + fi.fileName());
                    copyInput.open(IO_ReadOnly); // TODO: error dialog
                    copyOutput.open(IO_WriteOnly);
                    while(!copyInput.atEnd()){
                        copyOutput.putch(copyInput.getch());
                    }
                    copyInput.close();
                    copyOutput.close();
                }
                ++thumbCount;
            }
            stream << "</TR>\n";
        }
        // the end of a page
        stream << "</TABLE>\n";
        stream << "\n<P><EM><A HREF=\"http://www.mosfet.org\">"
            << i18n("&copy;Pixie - free software for KDE.")
            << "</A></EM>";
        outputFile.close();
    }
    accept();
}

void KIFHtmlExport::slotNextImgToggled(bool on)
{
    nextFileEdit->setEnabled(on);
    nextBrowseBtn->setEnabled(on);
    nextStrEdit->setEnabled(!on);
}

void KIFHtmlExport::slotPrevImgToggled(bool on)
{
    prevFileEdit->setEnabled(on);
    prevBrowseBtn->setEnabled(on);
    prevStrEdit->setEnabled(!on);

}

void KIFHtmlExport::slotLogoImgToggled(bool on)
{
    logoFileEdit->setEnabled(on);
    logoBrowseBtn->setEnabled(on);
    logoStrEdit->setEnabled(!on);
}

void KIFHtmlExport::slotHomeImgToggled(bool on)
{
    homeFileEdit->setEnabled(on);
    homeBrowseBtn->setEnabled(on);
    homeStrEdit->setEnabled(!on);
}

KIFHtmlExportURL::KIFHtmlExportURL(QWidget *parent, const char *name)
    : QDialog(parent, name, true)
{
    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    QLabel *infoLbl = new QLabel(i18n("You must now select what URL location you would\n\
like the thumbnails to point to in the HTML thumbnail\n\
files. This can either be the location in the file list,\n\
or a internet URL like http://www.mosfet.org/screenshots/.\n\
Be aware that if you choose not to use an internet URL,\n\
the thumbnails will point to images on your local hard\n\
drive or filesystem - not on your web or ftp site."), this);

    layout->addWidget(infoLbl);
    layout->addSpacing(20);
    useUrlCB = new QCheckBox(i18n("Use internet URL."), this);
    useUrlCB->setChecked(false);
    connect(useUrlCB, SIGNAL(toggled(bool)), this,
            SLOT(slotUseUrlToggled(bool)));
    layout->addWidget(useUrlCB);
    urlEdit = new QLineEdit(i18n("http://www.mosfet.org/"), this);
    layout->addWidget(urlEdit);
    urlEdit->setEnabled(false);
    layout->addStretch(1);
    KButtonBox *bbox = new KButtonBox(this);
    bbox->addStretch(1);
    connect(bbox->addButton(i18n("OK")), SIGNAL(clicked()), this,
            SLOT(accept()));
    layout->addWidget(bbox);
}

void KIFHtmlExportURL::slotUseUrlToggled(bool on)
{
    urlEdit->setEnabled(on);
}

KIFHtmlExportProgress::KIFHtmlExportProgress(int maxVal, QWidget *parent,
                                             const char *name)
    : QSemiModal(parent, name, true)
{

    QVBoxLayout *layout = new QVBoxLayout(this, 4);
    lbl = new QLabel(this);
    lbl->setMinimumSize(250, lbl->sizeHint().height()*2+16);
    layout->addWidget(lbl);
    progress = new KProgress(0, maxVal, 0, KProgress::Horizontal, this);
    layout->addWidget(progress);
    resize(sizeHint());
    show();
};

void KIFHtmlExportProgress::setProgress(const QString &text, int value)
{
    progress->setValue(value);
    lbl->setText(text);
}


#include "htmlexport.moc"

