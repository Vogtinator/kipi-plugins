/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2016-05-27
 * Description : Implementation of OAuth 2
 *
 * Copyright (C) 2016 by Fabian Vogt <fabian at ritter dash vogt dot de>
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

#ifndef KOAUTH2_H
#define KOAUTH2_H

#include <QNetworkReply>
#include <QString>
#include <QUrl>

/* Minimalist OAuth 2 implementation using KIO and Qt. */
class KOAuth2 : public QObject
{
Q_OBJECT

public:
    KOAuth2(const QUrl &auth_url, const QUrl &token_url, const QString &client_id, const QString &client_secret);
    ~KOAuth2();

    void setPin(const QString &pin);
    
    /* Cancels all requests in flight. */
    void cancel();

    /* Returns false if neither access_token, refresh_token nor pin are available. */
    bool canAuthorize();

    /* Call this and use m_access_token (true)
     * or wait for the authorized() signal (false). */
    bool authorize();

    /* Call this if you receive an OAuth2 related error.
     * It will first try to acquire a new access token
     * and if that doesn't work, a new refresh token. */
    void refreshAuth();

    /* Adds the user authorization info to the request. */
    void addAuthToken(QNetworkRequest *request);
    /* Adds the client authorization info to the request. */
    void addAnonToken(QNetworkRequest *request);

Q_SIGNALS:
    /* Open url in a browser and let the user copy the pin.
     * Call setPin(pin) to authorize. */
    void requestPin(const QUrl &url);
    void authorized(bool success, const QJsonObject &response);
    void error(const QString &msg);
    
public Q_SLOTS:
    /* Called by m_reply for pin to access_token exchange. */
    void tokenResponse();

public:
    /* Tokens used for authentication.
     * Read and write whenever you want,
     * it's going to use the values straight away. */
    QString m_access_token, m_refresh_token;

private:
    /* Request a new access_token either with pin or refresh_token. */
    void requestAccessToken();
    
    /* Target application URLs */
    QUrl m_auth_url, m_token_url;
    
    /* Client application identification */
    const QString m_client_id, m_client_secret;

    /* PIN is only needed if no access or refresh token
     * available or not working. */
    QString m_pin;
    
    /* The QNetworkAccessManager used for connections */
    QNetworkAccessManager m_net;
    
    /* Pointer to currently running QNetworkReply. Can be nullptr. */
    QNetworkReply *m_reply = nullptr;
};

#endif // KOAUTH2_H
