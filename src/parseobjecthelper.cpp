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
#include "parseobjecthelper.h"
#include "parseobject.h"
#include "parseuser.h"
#include "parserequest.h"


#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace cg
{
    ParseObjectHelper::ParseObjectHelper()
    {
    }

    ParseObjectHelper::~ParseObjectHelper()
    {
    }

    void ParseObjectHelper::createObject(ParseObjectPtr pObject)
    {
        if (!pObject)
        {
            emit createObjectFinished(ParseError::UnknownError);
            return;
        }

        _pObject = pObject;
        QJsonObject object = pObject->toJsonObject();
        QJsonDocument doc(object);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/classes/" + pObject->className(), content);

        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseObjectHelper::privateCreateObjectFinished);
    }

    void ParseObjectHelper::privateCreateObjectFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status;
        QByteArray data;
        ParseObjectPtr pObject = _pObject.lock();

        if (!isError(pReply, status, data) && pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                pObject->setValues(obj);
                pObject->clearDirtyState();
            }
        }

        emit createObjectFinished(status);
        pReply->deleteLater();
    }

    void ParseObjectHelper::fetchObject(ParseObjectPtr pObject)
    {
        if (!pObject)
        {
            emit fetchObjectFinished(ParseError::UnknownError);
            return;
        }

        _pObject = pObject;
        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/classes/" + pObject->className() + "/" + pObject->objectId());
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseObjectHelper::privateFetchObjectFinished);
    }

    void ParseObjectHelper::privateFetchObjectFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status;
        QByteArray data;
        ParseObjectPtr pObject = _pObject.lock();

        if (!isError(pReply, status, data) && pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                pObject->setValues(obj);
                pObject->clearDirtyState();
            }
        }

        emit fetchObjectFinished(status);
        pReply->deleteLater();
    }

    void ParseObjectHelper::updateObject(ParseObjectPtr pObject)
    {
        if (!pObject || pObject->objectId().isEmpty())
        {
            emit updateObjectFinished(ParseError::UnknownError);
            return;
        }

        _pObject = pObject;
        QJsonObject object = pObject->toJsonObject();
        QJsonDocument doc(object);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PutHttpMethod, "/parse/classes/" + pObject->className() + "/" + pObject->objectId(), content);
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseObjectHelper::privateUpdateObjectFinished);
    }

    void ParseObjectHelper::privateUpdateObjectFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status;
        QByteArray data;
        ParseObjectPtr pObject = _pObject.lock();

        if (!isError(pReply, status, data) && pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                pObject->setValues(obj);
                pObject->clearDirtyState();
            }
        }

        emit updateObjectFinished(status);
        pReply->deleteLater();
    }

    void ParseObjectHelper::deleteObject(ParseObjectPtr pObject)
    {
        if (!pObject || pObject->objectId().isEmpty())
        {
            emit deleteObjectFinished(ParseError::UnknownError);
            return;
        }

        _pObject = pObject;
        ParseRequest request(ParseRequest::DeleteHttpMethod, "/parse/classes/" + pObject->className() + "/" + pObject->objectId());
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseObjectHelper::privateDeleteObjectFinished);
    }

    void ParseObjectHelper::privateDeleteObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status;
        QByteArray data;
        isError(pReply, status, data);
        emit deleteObjectFinished(status);
        pReply->deleteLater();
    }

    void ParseObjectHelper::createAll(const QList<ParseObjectPtr>& objects)
    {
        if (objects.size() == 0)
        {
            emit createAllFinished(-1);
            return;
        }

        QString pathStr = "/parse/classes/";
        QJsonArray requestsArray;

        for (auto & pObject : objects)
        {
            QJsonObject bodyObject = pObject->toJsonObject();

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
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/batch", content);
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseObjectHelper::privateCreateAllFinished);
        _replyObjectListMap.insert(pReply, objects);
    }

    void ParseObjectHelper::privateCreateAllFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status;
        QByteArray data;
        QList<ParseObjectPtr> objects = _replyObjectListMap.take(pReply);

        if (!isError(pReply, status, data))
        {
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isArray())
            {
                QJsonArray resultsArray = doc.array();
                for (int i = 0; i < resultsArray.size(); i++)
                {
                    QJsonObject arrayObject = resultsArray.at(i).toObject();
                    if (arrayObject.contains("success"))
                    {
                        QJsonObject successObject = arrayObject.value("success").toObject();
                        ParseObjectPtr pObject = objects.at(i);
                        pObject->setValues(successObject);
                        pObject->clearDirtyState();
                    }
                }
            }
        }

        emit createAllFinished(status);
        pReply->deleteLater();
    }

    void ParseObjectHelper::updateAll(const QList<ParseObjectPtr>& objects)
    {
        if (objects.size() == 0)
        {
            emit updateAllFinished(-1);
            return;
        }

        QString pathStr = "/parse/classes/";
        QJsonArray requestsArray;

        for (auto & pObject : objects)
        {
            QJsonObject bodyObject = pObject->toJsonObject();

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
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/batch", content);
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseObjectHelper::privateUpdateAllFinished);
    }

    void ParseObjectHelper::privateUpdateAllFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status;
        QByteArray data;

        if (!isError(pReply, status, data))
        {
            QJsonDocument doc = QJsonDocument::fromJson(data);
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
        }

        emit updateAllFinished(status);
        pReply->deleteLater();
    }

    void ParseObjectHelper::deleteAll(const QList<ParseObjectPtr>& objects)
    {
        if (objects.size() == 0)
        {
            emit deleteAllFinished(-1);
            return;
        }

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
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/batch", content);
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseObjectHelper::privateDeleteAllFinished);
    }

    void ParseObjectHelper::privateDeleteAllFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status;
        QByteArray data;

        if (!isError(pReply, status, data))
        {
            QJsonDocument doc = QJsonDocument::fromJson(data);
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
        }

        emit deleteAllFinished(status);
        pReply->deleteLater();
    }
}