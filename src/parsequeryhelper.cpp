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
#include "parsereply.h"

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

    ParseReply* ParseQueryHelper::getObject(const QString &className, const QString &objectId)
    {
        if (className.isEmpty() || objectId.isEmpty())
        {
            return new ParseReply(ParseError::UnknownError);
        }

        QString queryStr = QString("where={\"objectId\":\"%1\"}").arg(objectId);
        QUrlQuery urlQuery;
        urlQuery.setQuery(queryStr);

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/classes/" + className);
        request.setUrlQuery(urlQuery);

        ParseReply *pReply = new ParseReply(request.sendRequest());
        connect(pReply, &ParseReply::preFinished, this, &ParseQueryHelper::getObjectFinished);
        return pReply;
    }

    void ParseQueryHelper::getObjectFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        if (!pReply->isError())
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                jsonArray = obj.value("results").toArray();
            }
        }
    }

    ParseReply* ParseQueryHelper::findObjects(const QString & className, const QUrlQuery & urlQuery)
    {
        if (className.isEmpty())
        {
            return new ParseReply(ParseError::UnknownError);
        }

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/classes/" + className);
        request.setUrlQuery(urlQuery);

        ParseReply *pReply = new ParseReply(request.sendRequest());
        connect(pReply, &ParseReply::preFinished, this, &ParseQueryHelper::findObjectsFinished);
        return pReply;
    }

    void ParseQueryHelper::findObjectsFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        if (!pReply->isError())
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                jsonArray = obj.value("results").toArray();
            }
        }
    }

    ParseReply* ParseQueryHelper::countObjects(const QString & className, const QUrlQuery & urlQuery)
    {
        if (className.isEmpty())
        {
            return new ParseReply(ParseError::UnknownError);
        }

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/classes/" + className);
        request.setUrlQuery(urlQuery);

        return new ParseReply(request.sendRequest());
    }
}