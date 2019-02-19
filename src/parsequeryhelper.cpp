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
#include "parsequeryhelper.h"
#include "parserequest.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaType>
#include <QDebug>

namespace cg
{
    ParseQueryHelper::ParseQueryHelper()
    {
    }

    ParseQueryHelper::~ParseQueryHelper()
    {
    }

    void ParseQueryHelper::getObject(const QString &className, const QString &objectId)
    {
        if (className.isEmpty() || objectId.isEmpty())
        {
            emit getObjectFinished(ParseObjectsResult());
            return;
        }

        QString queryStr = QString("where={\"objectId\":\"%1\"}").arg(objectId);
        QUrlQuery urlQuery;
        urlQuery.setQuery(queryStr);

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/classes/" + className);
        request.setUrlQuery(urlQuery);

        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseQueryHelper::privateGetObjectFinished);
    }

    void ParseQueryHelper::privateGetObjectFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseObjectsResult objectsResult(replyResult(pReply));

        if (!objectsResult.isError())
        {
            QJsonDocument doc = QJsonDocument::fromJson(objectsResult.data());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                jsonArray = obj.value("results").toArray();
            }
        }

        emit getObjectFinished(objectsResult);
        pReply->deleteLater();
    }

    void ParseQueryHelper::findObjects(const QString & className, const QUrlQuery & urlQuery)
    {
        if (className.isEmpty())
        {
            emit findObjectsFinished(ParseObjectsResult());
            return;
        }

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/classes/" + className);
        request.setUrlQuery(urlQuery);

        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseQueryHelper::privateFindObjectsFinished);
    }

    void ParseQueryHelper::privateFindObjectsFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseObjectsResult objectsResult(replyResult(pReply));

        if (!objectsResult.isError())
        {
            QJsonDocument doc = QJsonDocument::fromJson(objectsResult.data());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                jsonArray = obj.value("results").toArray();
            }
        }

        emit findObjectsFinished(objectsResult);
        pReply->deleteLater();
    }

    void ParseQueryHelper::countObjects(const QString & className, const QUrlQuery & urlQuery)
    {
        if (className.isEmpty())
        {
            emit countObjectsFinished(ParseCountResult());
            return;
        }

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/classes/" + className);
        request.setUrlQuery(urlQuery);

        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseQueryHelper::privateCountObjectsFinished);
    }

    void ParseQueryHelper::privateCountObjectsFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseCountResult countResult(replyResult(pReply));
        emit countObjectsFinished(countResult);
        pReply->deleteLater();
    }
}