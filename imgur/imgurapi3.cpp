#include <klocalizedstring.h>

#include <QDebug>
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimerEvent>
#include <QUrlQuery>

#include "imgurapi3.h"

static const QUrl imgur_auth_url = QUrl{QString::fromLatin1("https://api.imgur.com/oauth2/authorize")},
imgur_token_url = QUrl{QString::fromLatin1("https://api.imgur.com/oauth2/token")};

ImgurAPI3::ImgurAPI3(const QString &client_id, const QString &client_secret, QObject *parent)
    : QObject(parent), m_auth(imgur_auth_url, imgur_token_url, client_id, client_secret)
{
    connect(&m_auth, &KOAuth2::authorized, this, &ImgurAPI3::oauthAuthorized);
    connect(&m_auth, &KOAuth2::requestPin, this, &ImgurAPI3::oauthRequestPin);
    connect(&m_auth, &KOAuth2::error, this, &ImgurAPI3::authError);
}

ImgurAPI3::~ImgurAPI3()
{
    cancelAllWork();
}

KOAuth2 &ImgurAPI3::getAuth()
{
    return m_auth;
}

void ImgurAPI3::setPin(const QString &pin)
{
    m_auth.setPin(pin);
}

unsigned int ImgurAPI3::workQueueLength()
{
    return m_work_queue.size();
}

void ImgurAPI3::queueWork(const ImgurAPI3Action &action)
{
    m_work_queue.push(action);
    startWorkTimer();
}

void ImgurAPI3::cancelAllWork()
{
    stopWorkTimer();
    
    if(m_reply)
        m_reply->abort();
    
    /* Should error be emitted for those actions? */
    while(!m_work_queue.empty())
        m_work_queue.pop();
}

QUrl ImgurAPI3::urlForDeletehash(const QString &deletehash)
{
    return QUrl{QString::fromLatin1("https://imgur.com/delete/") + deletehash};
}

void ImgurAPI3::oauthAuthorized(bool success, const QJsonObject &response)
{
    if(success)
        startWorkTimer();
    else
        emit busy(false);

    emit authorized(success, response[QString::fromLatin1("account_username")].toString());
}

void ImgurAPI3::oauthRequestPin(const QUrl &url)
{
    emit busy(false);
    emit requestPin(url);
}

void ImgurAPI3::uploadProgress(qint64 sent, qint64 total)
{
    if(total > 0) /* Don't divide by 0 */
        emit progress((sent * 100) / total, m_work_queue.front());
}

void ImgurAPI3::replyFinished()
{
    auto *reply = m_reply;
    reply->deleteLater();
    m_reply = nullptr;
    
    if(this->m_image)
    {
        delete this->m_image;
        this->m_image = nullptr;
    }

    if(m_work_queue.empty())
    {
        qDebug() << "Received result without request";
        return;
    }

    /* toInt() returns 0 if conversion fails. That fits nicely already. */
    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    auto response = QJsonDocument::fromJson(reply->readAll());
    
    if(code == 200 && !response.isEmpty())
    {
        /* Success! */
        ImgurAPI3Result result;
        result.action = &m_work_queue.front();
        auto data = response.object()[QString::fromLatin1("data")].toObject();
        
        if(result.action->type == ImgurAPI3ActionType::IMG_UPLOAD
            || result.action->type == ImgurAPI3ActionType::ANON_IMG_UPLOAD)
        {
            result.image.animated = data[QString::fromLatin1("animated")].toBool();
            result.image.bandwidth = data[QString::fromLatin1("bandwidth")].toInt();
            result.image.datetime = data[QString::fromLatin1("datetime")].toInt();
            result.image.deletehash = data[QString::fromLatin1("deletehash")].toString();
            result.image.description = data[QString::fromLatin1("description")].toString();
            result.image.height = data[QString::fromLatin1("height")].toInt();
            result.image.hash = data[QString::fromLatin1("id")].toString();
            result.image.name = data[QString::fromLatin1("name")].toString();
            result.image.size = data[QString::fromLatin1("size")].toInt();
            result.image.title = data[QString::fromLatin1("title")].toString();
            result.image.type = data[QString::fromLatin1("type")].toString();
            result.image.url = data[QString::fromLatin1("link")].toString();
            result.image.views = data[QString::fromLatin1("views")].toInt();
            result.image.width = data[QString::fromLatin1("width")].toInt();
        }
        
        /* TODO PARSE */
        qDebug() << response.toJson();
        
        emit success(result);
    }
    else
    {
        if(code == 403)
        {
            /* HTTP 403 Forbidden -> Invalid token? 
             * That needs to be handled internally, so don't emit progress
             * and keep the action in the queue for later retries. */
            m_auth.refreshAuth();
            return;
        }
        else
        {
            /* Failed. */
            auto msg = response.object()[QString::fromLatin1("data")]
                    .toObject()[QString::fromLatin1("error")]
                    .toString(QString::fromLatin1("Could not read response."));

            emit error(msg, m_work_queue.front());
        }
    }
    
    /* Next work item. */
    m_work_queue.pop();
    startWorkTimer();
}

void ImgurAPI3::timerEvent(QTimerEvent *event)
{
    if(event->timerId() != m_work_timer)
        return QObject::timerEvent(event);

    event->accept();

    /* One-shot only. */
    QObject::killTimer(event->timerId());
    m_work_timer = 0;

    doWork();
}

void ImgurAPI3::startWorkTimer()
{
    if(!m_work_queue.empty() && m_work_timer == 0)
    {
        m_work_timer = QObject::startTimer(0);
        emit busy(true);
    }
    else
        emit busy(false);
}

void ImgurAPI3::stopWorkTimer()
{
    if(m_work_timer != 0)
    {
        QObject::killTimer(m_work_timer);
        m_work_timer = 0;
    }
}

void ImgurAPI3::doWork()
{
    if(m_work_queue.empty() || m_reply != nullptr)
        return;
    
    auto &work = m_work_queue.front();
    
    if(work.type != ImgurAPI3ActionType::ANON_IMG_UPLOAD && !m_auth.authorize())
        return; /* Wait for the authenticated() signal. */

    switch(work.type)
    {
    case ImgurAPI3ActionType::ACCT_INFO:
    {
        QNetworkRequest request(QUrl(QString::fromLatin1("https://api.imgur.com/3/account/%1")
                                     .arg(QString::fromLatin1(work.account.username.toUtf8().toPercentEncoding()))));
        m_auth.addAuthToken(&request);
        
        this->m_reply = m_net.get(request);
        break;
    }
    case ImgurAPI3ActionType::ANON_IMG_UPLOAD:
    case ImgurAPI3ActionType::IMG_UPLOAD:
    {
        this->m_image = new QFile(work.upload.imgpath);
        if(!m_image->open(QIODevice::ReadOnly))
        {
            delete this->m_image;
            this->m_image = nullptr;

            /* Failed. */
            emit error(i18n("Could not open file"), m_work_queue.front());

            m_work_queue.pop();
            return doWork();
        }

        /* Set ownership to m_image to delete that as well. */
        auto *multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType, m_image);
        QHttpPart title;
        title.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QString::fromLatin1("form-data; name=\"title\""));
        title.setBody(work.upload.title.toUtf8().toPercentEncoding());
        multipart->append(title);
        
        QHttpPart description;
        description.setHeader(QNetworkRequest::ContentDispositionHeader,
                              QString::fromLatin1("form-data; name=\"description\""));
        description.setBody(work.upload.description.toUtf8().toPercentEncoding());
        multipart->append(description);
        
        QHttpPart image;
        image.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant(QString::fromLatin1("form-data; name=\"image\"; filename=\"%1\"")
                                 .arg(QString::fromLatin1(QFileInfo(work.upload.imgpath).fileName().toUtf8().toPercentEncoding()))));
        image.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromLatin1("application/octet-stream"));
        image.setBodyDevice(this->m_image);
        multipart->append(image);
        
        QNetworkRequest request(QUrl(QString::fromLatin1("https://api.imgur.com/3/image")));
        if(work.type == ImgurAPI3ActionType::IMG_UPLOAD)
            m_auth.addAuthToken(&request);
        else
            m_auth.addAnonToken(&request);

        this->m_reply = this->m_net.post(request, multipart);

        break;
    }
    }
    
    if(this->m_reply)
    {
        connect(m_reply, &QNetworkReply::uploadProgress, this, &ImgurAPI3::uploadProgress);
        connect(m_reply, &QNetworkReply::finished, this, &ImgurAPI3::replyFinished);
    }
}
