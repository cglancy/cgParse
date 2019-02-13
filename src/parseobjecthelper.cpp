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

namespace cg
{
    ParseObjectHelper::ParseObjectHelper(ParseObjectPtr pObject)
        : _pObject(pObject)
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

        int status = ParseRequest::statusCode(pReply);
        ParseObjectPtr pObject = _pObject.lock();

        if (ParseRequest::isError(status))
        {
            status = ParseRequest::errorCode(pReply);
        }
        else if (pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
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

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/classes/" + pObject->className() + "/" + pObject->objectId());
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseObjectHelper::privateFetchObjectFinished);
    }

    void ParseObjectHelper::privateFetchObjectFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseObjectPtr pObject = _pObject.lock();
        int status = ParseRequest::statusCode(pReply);

        if (ParseRequest::isError(status))
        {
            status = ParseRequest::errorCode(pReply);
        }
        else if (pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
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

        ParseObjectPtr pObject = _pObject.lock();
        int status = ParseRequest::statusCode(pReply);

        if (ParseRequest::isError(status))
        {
            status = ParseRequest::errorCode(pReply);
        }
        else if (pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
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

        ParseRequest request(ParseRequest::DeleteHttpMethod, "/parse/classes/" + pObject->className() + "/" + pObject->objectId());
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseObjectHelper::privateDeleteObjectFinished);
    }

    void ParseObjectHelper::privateDeleteObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status = ParseRequest::statusCode(pReply);

        if (ParseRequest::isError(status))
        {
            status = ParseRequest::errorCode(pReply);
        }

        emit deleteObjectFinished(status);
        pReply->deleteLater();
    }
}