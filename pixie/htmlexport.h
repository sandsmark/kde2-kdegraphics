#ifndef __KIF_HTMLEXPORT_H
#define __KIF_HTMLEXPORT_H

#include <qdialog.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qsemimodal.h>

class KIntNumInput;
class KColorButton;
class QPushButton;
class QListBox;
class KProgress;
class QLabel;
class QTextStream;

class KIFHtmlExportProgress : public QSemiModal
{
public:
    KIFHtmlExportProgress(int maxVal, QWidget *parent=0, const char *name=0);
    void setProgress(const QString &text, int value);
protected:
    KProgress *progress;
    QLabel *lbl;
};

class KIFHtmlExportURL : public QDialog
{
    Q_OBJECT
public:
    KIFHtmlExportURL(QWidget *parent=0, const char *name=0);
    bool useUrl(){return(useUrlCB->isChecked());}
    const QString url(){return(urlEdit->text());}
protected slots:
    void slotUseUrlToggled(bool on);
protected:
    QCheckBox *useUrlCB;
    QLineEdit *urlEdit;
};

class KIFHtmlExport : public QDialog
{
    Q_OBJECT
public:
    KIFHtmlExport(QListBox *urlListBox, QWidget *parent=0, const char *name=0);
    ~KIFHtmlExport(){qWarning("Done");}
protected slots:
    void slotBrowseNextFile();
    void slotBrowsePrevFile();
    void slotBrowseHomeFile();
    void slotBrowseLogoFile();
    void slotBrowseOutputDir();
    void slotGenerateHTML();
    void generateHTMLHeader(QTextStream &stream, int page);

    void slotNextImgToggled(bool on);
    void slotPrevImgToggled(bool on);
    void slotLogoImgToggled(bool on);
    void slotHomeImgToggled(bool on);
    
protected:
    QCheckBox *nextImgCB, *prevImgCB, *homeImgCB, *logoImgCB, *indexCB,
        *fileNameCB;
    QPushButton *nextBrowseBtn, *prevBrowseBtn, *logoBrowseBtn, *homeBrowseBtn;
    QLineEdit *nextStrEdit, *prevStrEdit, *logoStrEdit, *homeStrEdit;
    QLineEdit *nextFileEdit, *prevFileEdit, *logoFileEdit, *homeFileEdit;
    QLineEdit *outputPathEdit, *baseHTMLEdit;
    KIntNumInput *rowInput, *columnInput, *sizeInput;
    KColorButton *bgColorBtn, *fgColorBtn;
    QListBox *urlList;
};

#endif

