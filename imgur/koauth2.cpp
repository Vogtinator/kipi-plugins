#include <QDebug>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>

#include <klocalizedstring.h>

#include "koauth2.h"

KOAuth2::KOAuth2(const QUrl &auth_url, const QUrl &token_url, const QString &client_id, const QString &client_secret)
    : m_auth_url(auth_url), m_token_url(token_url),
      m_client_id(client_id), m_client_secret(client_secret)
{}

KOAuth2::~KOAuth2()
{
    /* FIXME: This may cause a signal. */
    this->cancel();
}

void KOAuth2::setPin(const QString &pin)
{
    /* We have a PIN, now get an access_ and request_token. */
    m_pin = pin;
    requestAccessToken();
}

void KOAuth2::cancel()
{
    if(m_reply)
        m_reply->abort();
}

bool KOAuth2::canAuthorize()
{
    return !m_access_token.isEmpty() || !m_refresh_token.isEmpty() || !m_pin.isEmpty();
}

void KOAuth2::refreshAuth()
{
    if(!m_access_token.isEmpty())
        m_access_token = QString();
    else if(!m_refresh_token.isEmpty())
        m_refresh_token = QString();
    else if(!m_pin.isEmpty())
        m_pin = QString();

    authorize();
}

void KOAuth2::addAuthToken(QNetworkRequest *request)
{
    request->setRawHeader(QByteArray("Authorization"),
                          QString::fromLatin1("Bearer %1").arg(m_access_token).toUtf8());
}

void KOAuth2::addAnonToken(QNetworkRequest *request)
{
    request->setRawHeader(QByteArray("Authorization"),
                          QString::fromLatin1("Client-ID %1").arg(m_client_id).toUtf8());    
}

void KOAuth2::tokenResponse()
{
    /* Reply is complete. */
    auto *reply = m_reply;
    reply->deleteLater();
    this->m_reply = nullptr;

    auto response = QJsonDocument::fromJson(reply->readAll());
    if(response.isNull())
    {
        emit error(i18n("Could not read response: %1", reply->errorString()));
        emit authorized(false, QJsonObject{});
        return;
    }

    auto object = response.object();
    auto json_access_token = object[QString::fromLatin1("access_token")],
         json_refresh_token = object[QString::fromLatin1("refresh_token")];

    if(json_access_token.isUndefined())
    {
        if(!m_refresh_token.isEmpty())
        {
            /* Tried with refresh_token, didn't work. Try with pin next. */
            m_refresh_token = QString();
            authorize();
        }
        else
        {
            /* Tried with pin, probably invalid or expired. */
            m_pin = QString();
            emit error(i18n("Could not authorize with pin"));
            emit authorized(false, object);
        }
        return;
    }
    
    if(!json_access_token.isString() || !json_refresh_token.isString())
    {
        emit error(QString::fromLatin1("Invalid response"));
        emit authorized(false, object);
        return;
    }
    
    m_access_token = json_access_token.toString();
    m_refresh_token = json_refresh_token.toString();
    emit authorized(true, object);
}

bool KOAuth2::authorize()
{
    if(!m_refresh_token.isEmpty() && !m_access_token.isEmpty())
        return true;

    if(m_pin.isEmpty())
    {
        /* We need to request authorization before doing anything else. */
        QUrlQuery url_query;
        url_query.addQueryItem(QString::fromLatin1("client_id"), m_client_id);
        url_query.addQueryItem(QString::fromLatin1("response_type"), QString::fromLatin1("pin"));
        m_auth_url.setQuery(url_query);
    
        emit requestPin(m_auth_url);
    }
    else
    {
        /* Just request a (new) access_token. */
        requestAccessToken();
    }

    return false;
}

void KOAuth2::requestAccessToken()
{
    QUrlQuery url_query;
    url_query.addQueryItem(QString::fromLatin1("client_id"), m_client_id);
    url_query.addQueryItem(QString::fromLatin1("client_secret"), m_client_secret);
    if(!m_refresh_token.isEmpty())
    {
        url_query.addQueryItem(QString::fromLatin1("grant_type"), QString::fromLatin1("refresh_token"));
        url_query.addQueryItem(QString::fromLatin1("refresh_token"), m_refresh_token);
    }
    else
    {
        url_query.addQueryItem(QString::fromLatin1("grant_type"), QString::fromLatin1("pin"));
        url_query.addQueryItem(QString::fromLatin1("pin"), m_pin);
    }

    QNetworkRequest request(m_token_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromLatin1("application/x-www-form-urlencoded"));

    this->m_reply = this->m_net.post(request, url_query.toString().toLatin1());
    
    connect(m_reply, &QNetworkReply::finished, this, &KOAuth2::tokenResponse);
}
