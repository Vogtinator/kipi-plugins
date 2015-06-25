/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a kipi plugin to export images to Google-Drive web service
 *
 * Copyright (C) 2013 by Pankaj Kumar <me at panks dot me>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GDWIDGET_H
#define GDWIDGET_H

//Qt includes

#include <QWidget>

class QLabel;
class QSpinBox;
class QCheckBox;
class QButtonGroup;

class KComboBox;
class KPushButton;

namespace KIPI
{
    class Interface;
    class UploadWidget;
}

namespace KIPIPlugins
{
    class KPImagesList;
    class KPProgressWidget;
}

namespace KIPIGoogleDrivePlugin
{
  
enum PicasawebTagsBehaviour
{
    PwTagLeaf = 0,
    PwTagSplit,
    PwTagCombined
};

class GoogleDriveWidget : public QWidget
{
    Q_OBJECT

public:

    GoogleDriveWidget(QWidget* const parent, KIPI::Interface* const iface, const QString& serviceName);
    ~GoogleDriveWidget();

    void updateLabels(const QString& name = QString(), const QString& url = QString());

    KIPIPlugins::KPImagesList*     imagesList()  const;
    KIPIPlugins::KPProgressWidget* progressBar() const;

Q_SIGNALS:

    //void imageListChanged();

private Q_SLOTS:

    void slotResizeChecked();
    //void slotImageListChanged();

private:

    KIPIPlugins::KPImagesList*     m_imgList;
    KIPI::UploadWidget*            m_uploadWidget;
    QString                        m_serviceName;

    QLabel*                        m_headerLbl;
    QLabel*                        m_userNameDisplayLbl;
    KPushButton*                   m_changeUserBtn;
    KComboBox*                     m_dlDimensionCoB;

    KComboBox*                     m_albumsCoB;
    KPushButton*                   m_newAlbumBtn;
    KPushButton*                   m_reloadAlbumsBtn;

    QButtonGroup*                  m_tagsBGrp;
    QCheckBox*                     m_resizeChB;
    QSpinBox*                      m_dimensionSpB;
    QSpinBox*                      m_imageQualitySpB;

    KIPIPlugins::KPProgressWidget* m_progressBar;

    friend class GDWindow;
};

} // namespace KIPIGoogleDrivePlugin

#endif /* GDWIDGET_H */
