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
#include "parsequery.h"
#include "parsefile.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkInterface>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QCoreApplication>
#include <QMetaProperty>

namespace cg {
    ParseClient * ParseClientPrivate::instance = nullptr;
    const QString ParseClientPrivate::JsonContentType = "application/json";

    ParseClientPrivate::ParseClientPrivate(ParseClient *pParseClient)
        : QObject(pParseClient),
        q_ptr(pParseClient),
        currentUser(nullptr)
    {
        userAgent = QString("%1 %2").arg(QCoreApplication::applicationName())
            .arg(QCoreApplication::applicationVersion()).toUtf8();
        nam = new QNetworkAccessManager(this);

        // register so these types can be used in signals/slots
        qRegisterMetaType<ParseObject*>();
        qRegisterMetaType<ParseUser*>();
        qRegisterMetaType<ParseFile*>();
    }

    ParseClientPrivate::~ParseClientPrivate()
    {
    }

    void ParseClientPrivate::setCurrentUser(ParseUser *pUser)
    {
        if (currentUser)
        {
            delete currentUser;
            currentUser = nullptr;
        }

        if (pUser)
        {
            currentUser = pUser->clone();
        }
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
        d->apiHost = server;
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

    QByteArray ParseClient::apiHost() const
    {
        Q_D(const ParseClient);
        return d->apiHost;
    }

    QNetworkRequest ParseClientPrivate::buildRequest(const QString &apiRoute, const QString &contentType, const QUrlQuery &query) const
    {
        QString fullUrlStr = "https://" + apiHost;
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

        if (!contentType.isEmpty())
            req.setHeader(QNetworkRequest::ContentTypeHeader, contentType.toUtf8());

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
        
        QNetworkRequest request = d->buildRequest("/parse/login", QString(), query);
        request.setRawHeader("X-Parse-Revocable-Session", "1");
        QNetworkReply *pReply = d->nam->get(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::loginFinished);
    }

    void ParseClientPrivate::loginFinished()
    {
        ParseUser *pUser = nullptr;
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
            if (doc.isObject())
            {
                QVariantMap map;
                QJsonObject obj = doc.object();
                map = obj.toVariantMap();

                pUser = new ParseUser();
                pUser->setProperty(ParseObject::ObjectIdPropertyName, map.value(ParseObject::ObjectIdPropertyName));
                pUser->setProperty(ParseObject::CreatedAtPropertyName, map.value(ParseObject::CreatedAtPropertyName).toDateTime());
                pUser->setProperty(ParseObject::UpdatedAtPropertyName, map.value(ParseObject::UpdatedAtPropertyName).toDateTime());
                pUser->setProperty("username", map.value("username"));
                pUser->setProperty("sessionToken", map.value("sessionToken"));
                pUser->setDirty(false);

                setCurrentUser(pUser);
            }

            emit q_ptr->loginFinished(pUser, pReply->error());
        }

        pReply->deleteLater();
    }

    void ParseClient::logout()
    {
        Q_D(ParseClient);

        if (!d->currentUser)
            return;

        QNetworkRequest request = d->buildRequest("/parse/logout");
        request.setRawHeader("X-Parse-Session-Token", d->currentUser->sessionToken().toUtf8());
        QNetworkReply *pReply = d->nam->post(request, QByteArray());
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::logoutFinished);
    }

    void ParseClientPrivate::logoutFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            setCurrentUser(nullptr);
            emit q_ptr->logoutFinished(pReply->error());
        }

        pReply->deleteLater();
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

        QNetworkRequest request = d->buildRequest("/parse/users", ParseClientPrivate::JsonContentType);
        request.setRawHeader("X-Parse-Revocable-Session", "1");
        QNetworkReply *pReply = d->nam->post(request, content);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::signUpUserFinished);
        d->replyObjectMap.insert(pReply, pUser);
    }

    void ParseClientPrivate::signUpUserFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseUser *pUser = static_cast<ParseUser*>(replyObjectMap.value(pReply));
            if (pUser)
            {
                int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    if (statusCode == 201) // Created
                    {
                        QJsonObject obj = doc.object();
                        setObjectProperties(pUser, obj);
                        pUser->setDirty(false);
                    }
                }

                emit pUser->signUpFinished(statusCode);
                emit q_ptr->signUpUserFinished(pUser, statusCode);
            }

            replyObjectMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::deleteUser(ParseUser * pUser)
    {
        Q_D(ParseClient);

        if (!pUser || pUser->objectId().isEmpty() || pUser->sessionToken().isEmpty())
            return;

        QNetworkRequest request = d->buildRequest("/parse/users/" + pUser->objectId());
        request.setRawHeader("X-Parse-Session-Token", pUser->sessionToken().toUtf8());
        QNetworkReply *pReply = d->nam->deleteResource(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::deleteUserFinished);
        d->replyObjectMap.insert(pReply, pUser);
    }

    void ParseClientPrivate::deleteUserFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseUser *pUser = static_cast<ParseUser*>(replyObjectMap.value(pReply));
            if (pUser)
            {
                emit pUser->deleteUserFinished(pReply->error());
                emit q_ptr->deleteUserFinished(pUser, pReply->error());
            }

            replyObjectMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::createObject(ParseObject *pObject)
    {
        Q_D(ParseClient);

        if (!pObject)
            return;

        QJsonObject object = d->toJsonObject(pObject);
        QJsonDocument doc(object);
        QByteArray content = doc.toJson();

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pObject->className(), ParseClientPrivate::JsonContentType);
        QNetworkReply *pReply = d->nam->post(request, content);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::createObjectFinished);
        d->replyObjectMap.insert(pReply, pObject);
    }

    void ParseClientPrivate::createObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseObject *pObject = replyObjectMap.value(pReply);
            if (pObject)
            {
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QJsonObject obj = doc.object();
                    setObjectProperties(pObject, obj);
                    pObject->setDirty(false);
                }

                emit pObject->saveFinished(pReply->error());
                emit q_ptr->createObjectFinished(pObject, pReply->error());
            }

            replyObjectMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::fetchObject(ParseObject *pObject)
    {
        Q_D(ParseClient);

        if (!pObject)
            return;

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pObject->className() + "/" + pObject->objectId());
        QNetworkReply *pReply = d->nam->get(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::fetchObjectFinished);
        d->replyObjectMap.insert(pReply, pObject);
    }

    void ParseClientPrivate::fetchObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseObject *pObject = replyObjectMap.value(pReply);
            if (pObject)
            {
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QJsonObject obj = doc.object();
                    setObjectProperties(pObject, obj);
                    pObject->setDirty(false);
                }

                emit pObject->fetchFinished(pReply->error());
                emit q_ptr->fetchObjectFinished(pObject, pReply->error());
            }

            replyObjectMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::updateObject(ParseObject *pObject)
    {
        Q_D(ParseClient);

        if (!pObject || pObject->objectId().isEmpty())
            return;

        QJsonObject object = d->toJsonObject(pObject, true);
        QJsonDocument doc(object);
        QByteArray content = doc.toJson();

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pObject->className() + "/" + pObject->objectId(), 
            ParseClientPrivate::JsonContentType);
        QNetworkReply *pReply = d->nam->put(request, content);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::updateObjectFinished);
        d->replyObjectMap.insert(pReply, pObject);
    }

    void ParseClientPrivate::updateObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseObject *pObject = replyObjectMap.value(pReply);
            if (pObject)
            {
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QJsonObject obj = doc.object();
                    setObjectProperties(pObject, obj);
                    pObject->setDirty(false);
                }

                emit pObject->saveFinished(pReply->error());
                emit q_ptr->updateObjectFinished(pObject, pReply->error());
            }

            replyObjectMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::deleteObject(ParseObject *pObject)
    {
        Q_D(ParseClient);

        if (!pObject || pObject->objectId().isEmpty())
            return;

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pObject->className() + "/" + pObject->objectId());
        QNetworkReply *pReply = d->nam->deleteResource(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::deleteObjectFinished);
        d->replyObjectMap.insert(pReply, pObject);
    }

    void ParseClientPrivate::deleteObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseObject *pObject = replyObjectMap.value(pReply);
            if (pObject)
            {
                emit pObject->deleteFinished(pReply->error());
                emit q_ptr->deleteObjectFinished(pObject, pReply->error());
            }

            replyObjectMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::createAll(const QList<ParseObject*> &objects)
    {
        Q_D(ParseClient);

        if (objects.size() == 0)
            return;

        QString pathStr = "/parse/classes/";
        QJsonArray requestsArray;

        for (auto & pObject : objects)
        {
            QJsonObject bodyObject = d->toJsonObject(pObject);

            QString apiPath = pathStr + pObject->className();
            QJsonObject requestObject;
            requestObject.insert("method", "POST");
            requestObject.insert("path", apiPath);
            requestObject.insert("body", bodyObject);
            requestsArray.append(requestObject);
        }

        QJsonObject contentObject;
        contentObject.insert("requests", requestsArray);
        QJsonDocument doc(contentObject);
        QByteArray content = doc.toJson();

        QNetworkRequest request = d->buildRequest("/parse/batch", ParseClientPrivate::JsonContentType);
        QNetworkReply *pReply = d->nam->post(request, content);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::createAllFinished);
        d->replyObjectListMap.insert(pReply, objects);
    }

    void ParseClientPrivate::createAllFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            QList<ParseObject*> objects = replyObjectListMap.value(pReply);
            QByteArray bytes = pReply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(bytes);
            if (doc.isArray())
            {
                QJsonArray resultsArray = doc.array();
                for (int i = 0; i < resultsArray.size(); i++)
                {
                    QJsonObject arrayObject = resultsArray.at(i).toObject();
                    if (arrayObject.contains("success"))
                    {
                        QJsonObject successObject = arrayObject.value("success").toObject();
                        ParseObject *pObject = objects.at(i);
                        setObjectProperties(pObject, successObject);
                        pObject->setDirty(false);
                    }
                }
            }

            emit q_ptr->createAllFinished(pReply->error());

            replyObjectListMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::updateAll(const QList<ParseObject*> &objects)
    {
        Q_D(ParseClient);

        if (objects.size() == 0)
            return;

        QString pathStr = "/parse/classes/";
        QJsonArray requestsArray;

        for (auto & pObject : objects)
        {
            QJsonObject bodyObject = d->toJsonObject(pObject, true);

            QString apiPath = pathStr + pObject->className() + "/" + pObject->objectId();
            QJsonObject requestObject;
            requestObject.insert("method", "PUT");
            requestObject.insert("path", apiPath);
            requestObject.insert("body", bodyObject);
            requestsArray.append(requestObject);
        }

        QJsonObject contentObject;
        contentObject.insert("requests", requestsArray);
        QJsonDocument doc(contentObject);
        QByteArray content = doc.toJson();

        QNetworkRequest request = d->buildRequest("/parse/batch", ParseClientPrivate::JsonContentType);
        QNetworkReply *pReply = d->nam->post(request, content);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::updateAllFinished);
        d->replyObjectListMap.insert(pReply, objects);
    }

    void ParseClientPrivate::updateAllFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            QList<ParseObject*> objects = replyObjectListMap.value(pReply);
            QByteArray bytes = pReply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(bytes);
            if (doc.isArray())
            {
                QJsonArray resultsArray = doc.array();
                for (int i = 0; i < resultsArray.size(); i++)
                {
                    QJsonObject arrayObject = resultsArray.at(i).toObject();
                    if (arrayObject.contains("success"))
                    {
                    }
                }
            }

            emit q_ptr->updateAllFinished(pReply->error());

            replyObjectListMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::deleteAll(const QList<ParseObject*> &objects)
    {
        Q_D(ParseClient);

        if (objects.size() == 0)
            return;

        QString pathStr = "/parse/classes/";
        QJsonArray requestsArray;

        for (auto & pObject : objects)
        {
            QString apiPath = pathStr + pObject->className() + "/" + pObject->objectId();
            QJsonObject requestObject;
            requestObject.insert("method", "DELETE");
            requestObject.insert("path", apiPath);
            requestsArray.append(requestObject);
        }

        QJsonObject contentObject;
        contentObject.insert("requests", requestsArray);
        QJsonDocument doc(contentObject);
        QByteArray content = doc.toJson();

        QNetworkRequest request = d->buildRequest("/parse/batch", ParseClientPrivate::JsonContentType);
        QNetworkReply *pReply = d->nam->post(request, content);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::deleteAllFinished);
        d->replyObjectListMap.insert(pReply, objects);
    }

    void ParseClientPrivate::deleteAllFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            QList<ParseObject*> objects = replyObjectListMap.value(pReply);
            QByteArray bytes = pReply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(bytes);
            if (doc.isArray())
            {
                QJsonArray resultsArray = doc.array();
                for (int i = 0; i < resultsArray.size(); i++)
                {
                    QJsonObject arrayObject = resultsArray.at(i).toObject();
                    if (arrayObject.contains("success"))
                    {
                    }
                }
            }

            emit q_ptr->deleteAllFinished(pReply->error());

            replyObjectListMap.remove(pReply);
            pReply->deleteLater();
        }
    }
        
    void ParseClientPrivate::becomeFinished()
    {
    }

    void ParseClientPrivate::requestPasswordResetFinished()
    {
    }

    void ParseClient::getObject(ParseQuery *pQuery, const QString &objectId)
    {
        Q_D(ParseClient);

        if (!pQuery || pQuery->className().isEmpty())
            return;

        QString queryStr = QString("where={\"objectId\":\"%1\"}").arg(objectId);

        QUrlQuery urlQuery;
        urlQuery.setQuery(queryStr);

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pQuery->className());
        QNetworkReply *pReply = d->nam->get(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::getObjectFinished);
        d->replyQueryMap.insert(pReply, pQuery);
    }

    void ParseClientPrivate::getObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseQuery *pQuery = replyQueryMap.value(pReply);
            if (pQuery)
            {
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QJsonObject obj = doc.object();
                    QJsonArray resultsArray = obj.value("results").toArray();

                    QList<ParseObject*> objects;
                    for (auto & value : resultsArray)
                    {
                        if (value.isObject())
                        {
                            QVariantMap map = value.toObject().toVariantMap();
                            QString objectId = map.value(ParseObject::ObjectIdPropertyName).toString();
                            if (!objectId.isEmpty())
                            {
                                ParseObject *pObject = new ParseObject(pQuery->className());
                                for (auto &key : map.keys())
                                {
                                    pObject->setProperty(key.toUtf8(), map.value(key));
                                }
                                pObject->setDirty(false);
                                objects.append(pObject);
                            }
                        }
                    }
                    if (objects.size() > 0)
                    {
                        pQuery->setResults(objects);
                    }
                }

                emit pQuery->getFinished(pReply->error());
                emit q_ptr->getObjectFinished(pReply->error());
            }

            replyQueryMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::findObjects(ParseQuery *pQuery)
    {
        Q_D(ParseClient);

        if (!pQuery || pQuery->className().isEmpty())
            return;

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pQuery->className(), QString(), pQuery->urlQuery());
        QNetworkReply *pReply = d->nam->get(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::findObjectsFinished);
        d->replyQueryMap.insert(pReply, pQuery);
    }

    void ParseClientPrivate::findObjectsFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseQuery *pQuery = replyQueryMap.value(pReply);
            if (pQuery)
            {
                int count = 0;
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QJsonObject obj = doc.object();
                    QJsonArray resultsArray = obj.value("results").toArray();

                    QList<ParseObject*> objects;
                    for (auto & value : resultsArray)
                    {
                        if (value.isObject())
                        {
                            QVariantMap map = value.toObject().toVariantMap();
                            QString objectId = map.value(ParseObject::ObjectIdPropertyName).toString();
                            if (!objectId.isEmpty())
                            {
                                ParseObject *pObject = new ParseObject(pQuery->className());
                                for (auto &key : map.keys())
                                {
                                    pObject->setProperty(key.toUtf8(), map.value(key));
                                }
                                pObject->setDirty(false);
                                objects.append(pObject);
                            }
                        }
                    }
                    if (objects.size() > 0)
                    {
                        pQuery->setResults(objects);
                        count = objects.size();
                    }
                }

                emit pQuery->findFinished(count, pReply->error());
                emit q_ptr->findObjectsFinished(count, pReply->error());
            }

            replyQueryMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::countObjects(ParseQuery *pQuery)
    {
        Q_D(ParseClient);

        if (!pQuery || pQuery->className().isEmpty())
            return;

        QNetworkRequest request = d->buildRequest("/parse/classes/" + pQuery->className(), QString(), pQuery->urlQuery());
        QNetworkReply *pReply = d->nam->get(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::countObjectsFinished);
        d->replyQueryMap.insert(pReply, pQuery);
    }

    void ParseClientPrivate::countObjectsFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseQuery *pQuery = replyQueryMap.value(pReply);
            if (pQuery)
            {
                int count = 0;
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject())
                {
                    QJsonObject obj = doc.object();
                    QJsonArray resultsArray = obj.value("results").toArray();
                    count = resultsArray.size();
                }

                emit pQuery->countFinished(count, pReply->error());
                emit q_ptr->countObjectsFinished(count, pReply->error());
            }

            replyQueryMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::saveFile(ParseFile * pFile)
    {
        Q_D(ParseClient);

        if (!pFile)
        {
            emit saveFileFinished(-1);
            return;
        }

        QNetworkRequest request = d->buildRequest("/parse/files/" + pFile->name(), pFile->contentType());
        QNetworkReply *pReply = d->nam->post(request, pFile->data());
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::saveFileFinished);
        d->replyFileMap.insert(pReply, pFile);
    }

    void ParseClientPrivate::saveFileFinished()
    {
        Q_Q(ParseClient);

        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            ParseFile *pFile = replyFileMap.value(pReply);
            if (pFile)
            {
                int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
                if (doc.isObject() && statusCode == 201)  // 201 = Created
                {
                    QJsonObject obj = doc.object();
                    pFile->setProperty("url", obj.value("url").toString());
                    pFile->setProperty("name", obj.value("name").toString());
                }

                emit pFile->saveFinished(statusCode);
                emit q->saveFileFinished(statusCode);
            }

            replyFileMap.remove(pReply);
            pReply->deleteLater();
        }
    }

    void ParseClient::deleteFile(const QString &urlStr, const QString &masterKey)
    {
        Q_D(ParseClient);

        if (urlStr.isEmpty() || masterKey.isEmpty())
        {
            emit deleteFileFinished(-1);
            return;
        }

        QUrl url("https://" + d->apiHost + "/parse/files/" + urlStr);
        QNetworkRequest request(url);
        request.setRawHeader("User-Agent", d->userAgent);
        request.setRawHeader("X-Parse-Application-Id", d->applicationId);
        request.setRawHeader("X-Parse-Master-Key", masterKey.toUtf8());

        QNetworkReply *pReply = d->nam->deleteResource(request);
        connect(pReply, &QNetworkReply::finished, d, &ParseClientPrivate::deleteFileFinished);
    }

    void ParseClientPrivate::deleteFileFinished()
    {
        Q_Q(ParseClient);

        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (pReply)
        {
            int statusCode = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            emit q->deleteFileFinished(statusCode);
            pReply->deleteLater();
        }
    }

    QVariant ParseClientPrivate::toVariant(const QJsonValue &jsonValue, ParseObject *pParent)
    {
        Q_UNUSED(pParent);
        QVariant variant;

        if (jsonValue.isObject())
        {
            QJsonObject object = jsonValue.toObject();
            QString typeStr = object.value("__type").toString();
            if (typeStr == "Pointer")
            {
                QString className = object.value("className").toString();
                QString objectId = object.value(ParseObject::ObjectIdPropertyName).toString();

                ParseObject *pObject = new ParseObject(className);
                pObject->setProperty(ParseObject::ObjectIdPropertyName, objectId);
                variant = QVariant::fromValue<ParseObject*>(pObject);
            }
            else if (typeStr == "File")
            {
                ParseFile *pFile = new ParseFile(pParent);
                pFile->setProperty("name", object.value("name").toString());
                pFile->setProperty("url", object.value("url").toString());
                variant = QVariant::fromValue<ParseFile*>(pFile);
            }
            else if (typeStr == "Date")
            {
                QString isoStr = object.value("iso").toString();
                QDateTime dateTime = QDateTime::fromString(isoStr, Qt::ISODateWithMs);
                variant = dateTime;
            }
        }
        else
        {
            variant = jsonValue.toVariant();
        }

        return variant;
    }

    void ParseClientPrivate::setObjectProperties(ParseObject *pObject, const QJsonObject &jsonObject)
    {
        if (!pObject)
            return;

        QList<QByteArray> dynamicNames = pObject->dynamicPropertyNames();
        for (auto &name : dynamicNames)
        {
            if (jsonObject.contains(name))
                pObject->setProperty(name, toVariant(jsonObject.value(name), pObject));
        }

        int count = pObject->metaObject()->propertyCount();
        for (int i = 0; i < count; i++)
        {
            QMetaProperty property = pObject->metaObject()->property(i);
            QByteArray name = property.name();
            if (jsonObject.contains(name))
                pObject->setProperty(name, toVariant(jsonObject.value(name), pObject));
        }
    }

    QJsonValue ParseClientPrivate::toJsonValue(const QVariant &variant)
    {
        QJsonValue jsonValue;

        if (variant.canConvert<ParseObject*>())
        {
            ParseObject *pObject = qvariant_cast<ParseObject*>(variant);
            if (pObject)
            {
                QJsonObject jsonObject;
                jsonObject.insert("__type", "Pointer");
                jsonObject.insert("className", pObject->className());
                jsonObject.insert(ParseObject::ObjectIdPropertyName, pObject->objectId());
                jsonValue = jsonObject;
            }
        }
        else if (variant.canConvert<ParseFile*>())
        {
            ParseFile *pFile = qvariant_cast<ParseFile*>(variant);
            if (pFile)
            {
                QJsonObject jsonObject;
                jsonObject.insert("__type", "File");
                jsonObject.insert("name", pFile->name());
                jsonObject.insert("url", pFile->url());
                jsonValue = jsonObject;
            }
        }
        else if (variant.type() == QVariant::DateTime)
        {
            QJsonObject jsonObject;
            jsonObject.insert("__type", "Date");
            jsonObject.insert("iso", variant.toDateTime().toString(Qt::ISODateWithMs));
            jsonValue = jsonObject;
        }
        else
        {
            jsonValue = QJsonValue::fromVariant(variant);
        }
            
        return jsonValue;
    }

    QJsonObject ParseClientPrivate::toJsonObject(ParseObject *pObject, bool onlyDirtyProperties)
    {
        QJsonObject jsonObject;
        if (!pObject)
            return jsonObject;

        QList<QByteArray> dynamicNames = pObject->dynamicPropertyNames();
        for (auto &name : dynamicNames)
        {
            if (onlyDirtyProperties && pObject->isDirty(name))
                jsonObject.insert(name, toJsonValue(pObject->property(name)));
            else if (!onlyDirtyProperties)
                jsonObject.insert(name, toJsonValue(pObject->property(name)));
        }

        int count = pObject->metaObject()->propertyCount();
        for (int i = 0; i < count; i++)
        {
            QMetaProperty property = pObject->metaObject()->property(i);
            QByteArray name = property.name();
            if (isWritableProperty(name))
            {
                if (onlyDirtyProperties && pObject->isDirty(name))
                    jsonObject.insert(name, toJsonValue(pObject->property(name)));
                else if (!onlyDirtyProperties)
                    jsonObject.insert(name, toJsonValue(pObject->property(name)));
            }
        }

        return jsonObject;
    }

    bool ParseClientPrivate::isWritableProperty(const QByteArray &name)
    {
        return (name != "objectName" &&
            name != ParseObject::ObjectIdPropertyName &&
            name != ParseObject::CreatedAtPropertyName &&
            name != ParseObject::UpdatedAtPropertyName);
    }
}
