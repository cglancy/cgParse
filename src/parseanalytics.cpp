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
#include "parseanalytics.h"
#include "parseclient.h"
#include "parsereply.h"
#include "parserequest.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace cg
{
    const QString ParseAnalytics::AppOpenedEvent = QStringLiteral("AppOpened");
    const QString ParseAnalytics::ErrorEvent = QStringLiteral("Error");
    const int ParseAnalytics::MaxDimensions = 8;
    const int ParseAnalytics::NoError = 0;

    ParseReply* ParseAnalytics::trackAppOpened(const QVariantMap& map, const QDateTime& dateTime)
    {
        return new ParseReply(ParseAnalytics(AppOpenedEvent, NoError, map, dateTime));
    }

    ParseReply* ParseAnalytics::trackEvent(const QString& eventName, const QVariantMap& map, const QDateTime& dateTime)
    {
        return new ParseReply(ParseAnalytics(eventName, NoError, map, dateTime));
    }

    ParseReply* ParseAnalytics::trackError(int errorCode, const QVariantMap& map, const QDateTime& dateTime)
    {
        return new ParseReply(ParseAnalytics(ErrorEvent, errorCode, map, dateTime));
    }

    ParseAnalytics::ParseAnalytics(const QString& eventName, int errorCode, const QVariantMap& map, const QDateTime& dateTime)
    {
        setApiRoute("events/" + eventName);
        setHttpMethod(PostHttpMethod);
        setContentType(JsonContentType);

        setHeader("User-Agent", ParseRequest::userAgent());
        setHeader("X-Parse-Application-Id", ParseClient::get()->applicationId());
        setHeader("X-Parse-REST-API-Key", ParseClient::get()->clientKey());

        QJsonObject jsonObject;
        QJsonObject dimensionsObject;

        if (!dateTime.isNull())
        {
            QJsonObject dateObject;
            dateObject.insert("__type", "Date");
            dateObject.insert("iso", dateTime.toString(Qt::ISODateWithMs));
            jsonObject.insert("at", dateObject);
        }

        if (eventName == ErrorEvent)
        {
            dimensionsObject.insert("code", errorCode);
        }

        if (!map.isEmpty())
        {
            QJsonObject mapObject = QJsonObject::fromVariantMap(map);
            int count = MaxDimensions - dimensionsObject.size();
            count = std::min(count, mapObject.size());
            auto it = mapObject.begin();
            for (int i = 0; i < count; i++, it++)
            {
                dimensionsObject.insert(it.key(), it.value());
            }
        }

        if (!dimensionsObject.isEmpty())
            jsonObject["dimensions"] = dimensionsObject;

        QJsonDocument doc;
        doc.setObject(jsonObject);
        
        setContent(doc.toJson());
    }
}
