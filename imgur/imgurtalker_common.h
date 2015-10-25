/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef IMGURTALKER_COMMON_H
#define IMGURTALKER_COMMON_H

// API key from imgur
#define _IMGUR_API_ANONYMOUS_KEY "2da1cc4923f33dc72885aa32adede5c3";

#ifdef OAUTH_ENABLED
#   define _IMGUR_API_CONSUMER_KEY "9ad9d48124b7137d9cc9e0397219f52004fcba12a";
#   define _IMGUR_API_CONSUMER_SECRET "26b83f19f6d045006d769869b9c623e0";
#endif //OAUTH_ENABLED

// Qt includes

#include <QDateTime>
#include <QString>
#include <QUrl>

namespace KIPIImgurPlugin
{

class ImgurConnection
{
public:

    static QString APIuploadURL()
    {
        return QStringLiteral("https://api.imgur.com/2/upload.json");
    }

    static QString APIdeleteURL()
    {
        return QStringLiteral("https://api.imgur.com/2/delete.json");
    }

    static QString OAuthTokenEndPoint()
    {
        return QStringLiteral("https://api.imgur.com/oauth/request_token");
    }

    static QString OAuthAccessEndPoint()
    {
        return QStringLiteral("https://api.imgur.com/oauth/access_token");
    }

    static QString OAuthAuthorizationEndPoint()
    {
        return QStringLiteral("https://api.imgur.com/oauth/authorize");
    }

    static QUrl originalURL(QString imageHash)
    {
        return QUrl(QStringLiteral("http://i.imgur.com/") + imageHash + QStringLiteral(".jpg"));
    } // this is wrong

    static QUrl pageURL(QString imageHash)
    {
        return QUrl(QStringLiteral("http://imgur.com/") + imageHash);
    }

    static QUrl deleteURL(QString deleteHash)
    {
        return QUrl(QStringLiteral("http://imgur.com/delete/") + deleteHash);
    }
};

/*
QString ImgurConnection::uploadURL = QStringLiteral("https://api.imgur.com/2/upload.json");
QString ImgurConnection::deleteURL = QStringLiteral("https://api.imgur.com/2/delete.json");
QString ImgurConnection::OAuthTokenEndPoint = QStringLiteral("https://api.imgur.com/oauth/request_token");
QString ImgurConnection::OAuthAccessEndPoint = QStringLiteral("https://api.imgur.com/oauth/access_token");
QString ImgurConnection::OAuthAuthorizationEndPoint = QStringLiteral("https://api.imgur.com/oauth/authorize");
*/

struct ImgurError
{
    enum ImgurMethod
    {
        POST = 0,
        GET,
        HEAD
    };

    enum ImgurFormat
    {
        XML = 0,
        JSON
    };

    ImgurError()
    {
        method = POST;
        format = XML;
    }

    ImgurMethod method;
    ImgurFormat format;
    QString     message;
    QString     request;
    QVariant    parameters;
};

// -----------------------------------------------------------------------------

struct ImgurUploadData
{
    QString title;
    QString caption;
    QUrl    fileUrl;
};

// -----------------------------------------------------------------------------

struct ImgurSuccess
{
    struct ImgurImage
    {
        QString    name;
        QString    title;
        QString    caption;
        QString    hash;
        QString    deletehash;
        QDateTime  datetime;
        QString    type;       // maybe enum
        bool       animated;
        uint       width;
        uint       height;
        uint       size;
        uint       views;
        qulonglong bandwidth;
    } image;

    struct ImgurLinks
    {
        QUrl original;
        QUrl imgur_page;
        QUrl delete_page;
        QUrl small_square;
        QUrl large_thumbnail;
    } links;
};

} // namespace KIPIImgurPlugin

#endif // IMGURTALKER_COMMON_H
