/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2012-04-21
 * Description : a kipi plugin to export images to the Imgur web service
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#ifndef IMGURUPLOADINFORMATION_H
#define IMGURUPLOADINFORMATION_H

// Qt includes

#include <QDialog>
#include <QString>
#include <QUrl>

// Local includes

#include "imgurtalker.h"

namespace KIPIImgurPlugin
{

class ImgurUploadInformation : public QDialog
{
    Q_OBJECT

public:

    ImgurUploadInformation(QWidget* const parent = 0);
    ~ImgurUploadInformation();

    void setTitle(const QString& text);
    QString title();

    void setCaption(const QString& text);
    QString Caption();

    void setFileUrl(const QUrl& filePath);
    QUrl fileUrl();

private:

    ImgurUpload d;
};

} // namespace KIPIImgurPlugin

#endif /* IMGURUPLOADINFORMATION_H */

