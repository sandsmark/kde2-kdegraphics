/***************************************************************************
                          devselector.h  -  description
                             -------------------
    begin                : Sun Jun 11 2000
    copyright            : (C) 2000 by Klaas Freitag
    email                : freitag@suse.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef DEVSELECTOR_H
#define DEVSELECTOR_H


#include <kdialogbase.h>

class QButtonGroup;
class QStrList;
class QStringList;
class QCheckBox;

/**
  *@author Klaas Freitag
  */

/* Configuration-file definitions */
#define GROUP_STARTUP    "Scan Settings"
#define STARTUP_SCANDEV  "ScanDevice"
#define STARTUP_SKIP_ASK "SkipStartupAsk"
#define STARTUP_ONLY_LOCAL "QueryLocalOnly"


/**
 *  A utitlity class that lets the user select a scan device.
 *
 *  This class provides functions to get the scan device to use for an 
 *  application with scan support.
 *
 *  Its main purpose is to display a selection dialog, but it also reads
 *  and writes config entries to store the users selection and handles
 *  a field 'do not ask me again'.
 *
 */

class DeviceSelector: public KDialogBase
{
   Q_OBJECT
public:
   /**
    *  constructs the dialog class
    *  @param QWidget *parent - the parent
    *  @param QStrList backends - a list of device names retrieved from the scan device
    *  @param QStrList scannerNames - a list of corresponding human readable sanner names.
    */
   DeviceSelector( QWidget *parent, const QStrList&, const QStringList& );
   ~DeviceSelector();

   /**
    *  returns the device the user selected.
    *  @return a CString containing the technical name of the selected device (taken from
    *          the backends-list from the constructor)
    */
   QCString getSelectedDevice( void ) const;

   /**
    *  returns the users selection if the dialog should be skipped in future.
    *  @return true for should be skipped.
    */
   bool     getShouldSkip( void ) const;

   /**
    *  retrieval to get the device from the config file. The function reads the applications
    *  config file, cares for the 'do not ask me again'-settings and checks if the scandevice
    *  specified in the config file is present at the current configuration.
    *  If this function returns null, the DeviceSelector should be opened for a user selection.
    *  @return a string containing the device to open or null if no device is specified or the
    *  one specified is not valid.
    */
   QCString getDeviceFromConfig( void ) const;

public slots:
   void setScanSources( const QStrList&, const QStringList& );

private:
   QButtonGroup *selectBox;
   mutable QStrList devices;
   QCheckBox   *cbSkipDialog;

   class DeviceSelectorPrivate;
   DeviceSelectorPrivate *d;
};

#endif
