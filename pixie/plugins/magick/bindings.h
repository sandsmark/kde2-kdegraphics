#ifndef __BINDINGS_H
#define __BINDINGS_H

#include <mini-magick/magick.h>

extern "C"{
    extern Export void runMagickProgressDlg(const char *, const off_t, const off_t);
    extern Export void runMagickErrorDlg(const ErrorType err, const char *,
                                         const char *);
    extern Export void runMagickWarningDlg(const WarningType err, const char *,
                                           const char *);
};

#endif
