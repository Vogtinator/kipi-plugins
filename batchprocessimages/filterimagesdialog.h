//////////////////////////////////////////////////////////////////////////////
//
//    FILTERIMAGESDIALOG.H
//
//    Copyright (C) 2003-2004 Gilles CAULIER <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin Steet, Fifth Floor, Cambridge, MA 02110-1301, USA.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef FILTERIMAGESDIALOG_H
#define FILTERIMAGESDIALOG_H

// Local includes

#include "kpaboutdata.h"
#include "batchprocessimagesdialog.h"

namespace KIPIBatchProcessImagesPlugin
{

class BatchProcessImagesItem;

class FilterImagesDialog : public BatchProcessImagesDialog
{
Q_OBJECT

 public:
 
   FilterImagesDialog( KURL::List images, KIPI::Interface* interface, QWidget *parent=0 );
   ~FilterImagesDialog();

 private slots:
 
   void slotHelp(void);
   void slotOptionsClicked(void);
   void slotTypeChanged(int type);

 protected:
 
   QString                m_noiseType;
   int                    m_blurRadius;
   int                    m_blurDeviation;
   int                    m_medianRadius;
   int                    m_noiseRadius;
   int                    m_sharpenRadius;
   int                    m_sharpenDeviation;
   int                    m_unsharpenRadius;
   int                    m_unsharpenDeviation;
   int                    m_unsharpenPercent;
   int                    m_unsharpenThreshold;

   QString makeProcess(KProcess* proc, BatchProcessImagesItem *item,
                       const QString& albumDest, bool previewMode);

   void readSettings(void);
   void saveSettings(void);

 private:

   KIPIPlugins::KPAboutData *m_about;
};

}  // NameSpace KIPIBatchProcessImagesPlugin

#endif  // FILTERIMAGESDIALOG_H
