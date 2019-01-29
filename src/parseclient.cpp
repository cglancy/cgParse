/**
* Copyright 2017 Charles Glancy (charles@glancyfamily.net)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without restriction, including  without limitation the rights to use, copy,
* modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
* is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "parseclient.h"
#include "parseclient_p.h"
#include "parseobject.h"
#include "parseuser.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkInterface>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QCoreApplication>

namespace cg {
    ParseClient * ParseClientPrivate::instance = nullptr;

    ParseClientPrivate::ParseClientPrivate(ParseClient *pParseClient)
        : QObject(pParseClient),
        q_ptr(pParseClient),
        currentUser(nullptr)
    {
        userAgent = QString("%1 %2").arg(QCoreApplication::applicationName())
            .arg(QCoreApplication::applicationVersion()).toUtf8();
        nam = new QNetworkAccessManager(this);
    }

    ParseClientPrivate::~ParseClientPrivate()
    {
    }

    //
    // ParseClient
    //

    ParseClient::ParseClient()
        : d_ptr(new ParseClientPrivate(this))
    {
    }

    ParseClient::~ParseClient()
    {
    }

    ParseClient * ParseClient::instance()
    {
        if (!ParseClientPrivate::instance)
            ParseClientPrivate::instance = new ParseClient();

        return ParseClientPrivate::instance;
    }

    void ParseClient::initialize(const QByteArray &appId, const QByteArray &clientKey, const QByteArray &server)
    {
        Q_D(ParseClient);
        d->applicationId = appId;
        d->clientKey = clientKey;
        d->server = server;
    }

    QByteArray ParseClient::applicationId() const
    {
        Q_D(const ParseClient);
        return d->applicationId;
    }

    QByteArray ParseClient::clientKey() const
    {
        Q_D(const ParseClient);
        return d->clientKey;
    }

    QByteArray ParseClient::server() const
    {
        Q_D(const ParseClient);
        return d->server;
    }

    QNetworkRequest ParseClientPrivate::buildRequest(const QString &apiRoute, const QUrlQuery &query, bool addContentHeader) const
    {
        QString fullUrlStr = "https://" + server;
        if (apiRoute.startsWith("/"))
            fullUrlStr += apiRoute;
        else
            fullUrlStr += "/" + apiRoute;

        QUrl url(fullUrlStr);
        if (!query.isEmpty())
            url.setQuery(query);
        QNetworkRequest req(url);

        req.setRawHeader("User-Agent", userAgent);
        req.setRawHeader("X-Parse-Application-Id", applicationId);
        req.setRawHeader("X-Parse-REST-API-Key", clientKey);

        if (addContentHeader)
            req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        return req;
    }

    ParseUser * ParseClient::currentUser() const
    {
        Q_D(const ParseClient);
        return d->currentUser;
    }

    void ParseClient::login(const QString &username, const QString &password)
    {
        Q_D(ParseClient);

        QUrlQuery query;
        query.addQueryItem("username", username);
        query.addQueryItem("password", password);
        
        QNetworkRequest request = d->buildRequest("/parse/login", query, false);
        request.setRawHeader("X-Parse-Revocable-Session", "1");
        QNetworkReply *pReply = d->nam->get(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::loginFinished);
    }

    void ParseClient::logout()
    {
        Q_D(ParseClient);

        if (!d->currentUser)
            return;

        QNetworkRequest request = d->buildRequest("/parse/logout", QUrlQuery(), false);
        request.setRawHeader("X-Parse-Session-Token", d->currentUser->sessionToken().toUtf8());
        QNetworkReply *pReply = d->nam->post(request, QByteArray());
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::logoutFinished);
    }

    void ParseClient::become(const QString &sessionToken)
    {
        sessionToken;
    }

    void ParseClient::requestPasswordReset(const QString &email)
    {
        email;
    }

    void ParseClient::signUpUser(ParseUser *pUser)
    {
        Q_D(ParseClient);

        if (!pUser)
            return;

        QVariantMap map;
        QList<QByteArray> names = pUser->propertyNames();
        for (auto & name : names)
            map.insert(name, pUser->property(name));

        QJsonObject object = QJsonObject::fromVariantMap(map);
        QJsonDocument doc(object);
        QByteArray content = doc.toJson();

        QNetworkRequest request = d->buildRequest("/parse/users");
        request.setRawHeader("X-Parse-Revocable-Session", "1");
        QNetworkReply *pReply = d->nam->post(request, content);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::signUpUserFinished);
        d->replyMap.insert(pReply, pUser);
    }

    void ParseClient::deleteUser(ParseUser * pUser)
    {
        Q_D(ParseClient);

        if (!pUser || pUser->objectId().isEmpty() || pUser->sessionToken().isEmpty())
            return;

        QNetworkRequest request = d->buildRequest("/parse/users/" + pUser->objectId(), QUrlQuery(), false);
        request.setRawHeader("X-Parse-Session-Token", pUser->sessionToken().toUtf8());
        QNetworkReply *pReply = d->nam->deleteResource(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::deleteUserFinished);
        d->replyMap.insert(pReply, pUser);
    }

    void ParseClient::createObject(ParseObject *pObject)
    {
        Q_D(ParseClient);

        if (!pObject)
            return;

        QVariantMap map;
        for (auto & name : pObject->propertyNames())
            map.insert(name, pObject->property(name));

        QJsonObject object = QJsonObject::fromVariantMap(map);
        QJsonDocument doc(object);
        QByteArray content = doc.toJson();

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pObject->className());
        QNetworkReply *pReply = d->nam->post(request, content);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::createObjectFinished);
        d->replyMap.insert(pReply, pObject);
    }

    void ParseClient::fetchObject(ParseObject *pObject)
    {
        Q_D(ParseClient);

        if (!pObject)
            return;

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pObject->className() + "/" + pObject->objectId(), 
            QUrlQuery(), false);
        QNetworkReply *pReply = d->nam->get(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::fetchObjectFinished);
        d->replyMap.insert(pReply, pObject);
    }

    void ParseClient::updateObject(ParseObject *pObject)
    {
        Q_D(ParseClient);

        if (!pObject || pObject->objectId().isEmpty())
            return;

        QVariantMap map;
        for (auto & name : pObject->propertyNames())
        {
            if (pObject->isDirty(name))
                map.insert(name, pObject->property(name));
        }

        QJsonObject object = QJsonObject::fromVariantMap(map);
        QJsonDocument doc(object);
        QByteArray content = doc.toJson();

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pObject->className() + "/" + pObject->objectId());
        QNetworkReply *pReply = d->nam->put(request, content);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::updateObjectFinished);
        d->replyMap.insert(pReply, pObject);
    }

    void ParseClient::deleteObject(ParseObject *pObject)
    {
        Q_D(ParseClient);

        if (!pObject || pObject->objectId().isEmpty())
            return;

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pObject->className() + "/" + pObject->objectId(), QUrlQuery(), false);
        QNetworkReply *pReply = d->nam->deleteResource(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::deleteObjectFinished);
        d->replyMap.insert(pReply, pObject);
    }

    void ParseClientPrivate::createObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseObject *pObject = replyMap.value(pReply);
            if (pObject)
            {
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QVariantMap map;
                    QJsonObject obj = doc.object();
                    map = obj.toVariantMap();

                    QDateTime createdAtDateTime = map.value(ParseObject::CreatedAtPropertyName).toDateTime();

                    pObject->setProperty(ParseObject::ObjectIdPropertyName, map.value(ParseObject::ObjectIdPropertyName));
                    pObject->setProperty(ParseObject::CreatedAtPropertyName, createdAtDateTime);
                    pObject->setProperty(ParseObject::UpdatedAtPropertyName, createdAtDateTime);
                    pObject->setDirty(false);
                }

                emit pObject->saveFinished(pReply->error());
                emit q_ptr->createObjectFinished(pObject, pReply->error());
            }

            replyMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClientPrivate::fetchObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseObject *pObject = replyMap.value(pReply);
            if (pObject)
            {
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QVariantMap map;
                    QJsonObject obj = doc.object();
                    map = obj.toVariantMap();

                    for (auto & key : map.keys())
                    {
                        pObject->setProperty(key.toUtf8(), map.value(key));
                    }

                    pObject->setDirty(false);
                }

                emit pObject->fetchFinished(pReply->error());
                emit q_ptr->fetchObjectFinished(pObject, pReply->error());
            }

            replyMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClientPrivate::updateObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseObject *pObject = replyMap.value(pReply);
            if (pObject)
            {
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QVariantMap map;
                    QJsonObject obj = doc.object();
                    map = obj.toVariantMap();

                    pObject->setProperty(ParseObject::UpdatedAtPropertyName, 
                        map.value(ParseObject::UpdatedAtPropertyName).toDateTime());
                    pObject->setDirty(false);
                }

                emit pObject->saveFinished(pReply->error());
                emit q_ptr->updateObjectFinished(pObject, pReply->error());
            }

            replyMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClientPrivate::deleteObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseObject *pObject = replyMap.value(pReply);
            if (pObject)
            {
                emit pObject->deleteFinished(pReply->error());
                emit q_ptr->deleteObjectFinished(pObject, pReply->error());
            }

            replyMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClientPrivate::loginFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
            if (doc.isObject())
            {
                QVariantMap map;
                QJsonObject obj = doc.object();
                map = obj.toVariantMap();

                if (currentUser)
                {
                    delete currentUser;
                    currentUser = nullptr;
                }

                currentUser = new ParseUser();
                currentUser->setProperty(ParseObject::ObjectIdPropertyName, map.value(ParseObject::ObjectIdPropertyName));
                currentUser->setProperty(ParseObject::CreatedAtPropertyName, map.value(ParseObject::CreatedAtPropertyName).toDateTime());
                currentUser->setProperty(ParseObject::UpdatedAtPropertyName, map.value(ParseObject::UpdatedAtPropertyName).toDateTime());
                currentUser->setProperty("username", map.value("username"));
                currentUser->setProperty("sessionToken", map.value("sessionToken"));
                currentUser->setDirty(false);
            }

            emit q_ptr->loginFinished(currentUser, pReply->error());
        }

        pReply->deleteLater();
    }

    void ParseClientPrivate::logoutFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            if (currentUser)
            {
                delete currentUser;
                currentUser = nullptr;
            }

            emit q_ptr->logoutFinished(pReply->error());
        }

        pReply->deleteLater();
    }

    void ParseClientPrivate::becomeFinished()
    {
    }

    void ParseClientPrivate::requestPasswordResetFinished()
    {
    }

    void ParseClientPrivate::signUpUserFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseUser *pUser = static_cast<ParseUser*>(replyMap.value(pReply));
            if (pUser)
            {
                int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QVariantMap map;
                    QJsonObject obj = doc.object();
                    map = obj.toVariantMap();

                    if (statusCode == 201) // Created
                    {
                        for (auto & key : map.keys())
                        {
                            pUser->setProperty(key.toUtf8(), map.value(key));
                        }

                        pUser->setDirty(false);
                    }
                }

                emit pUser->signUpFinished(statusCode);
                emit q_ptr->signUpUserFinished(pUser, statusCode);
            }

            replyMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClientPrivate::deleteUserFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseUser *pUser = static_cast<ParseUser*>(replyMap.value(pReply));
            if (pUser)
            {
                emit pUser->deleteUserFinished(pReply->error());
                emit q_ptr->deleteUserFinished(pUser, pReply->error());
            }

            replyMap.remove(pReply);
            pReply->deleteLater();
        }
    }
}
