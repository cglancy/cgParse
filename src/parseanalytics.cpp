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
    const QString ParseAnalytics::JsonContentType = QStringLiteral("application/json");
    const QString ParseAnalytics::AppOpenedEvent = QStringLiteral("AppOpened");
    const QString ParseAnalytics::ErrorEvent = QStringLiteral("Error");
    const int ParseAnalytics::MaxDimensions = 8;
    const int ParseAnalytics::NoError = 0;

    ParseReply* ParseAnalytics::trackAppOpened(const QVariantMap& map, const QDateTime& dateTime)
    {
        return new ParseReply(ParseAnalytics(AppOpenedEvent, map, dateTime));
    }

    ParseReply* ParseAnalytics::trackEvent(const QString& eventName, const QVariantMap& map, const QDateTime& dateTime)
    {
        return new ParseReply(ParseAnalytics(eventName, map, dateTime));
    }

    ParseReply* ParseAnalytics::trackError(int errorCode, const QVariantMap& map, const QDateTime& dateTime)
    {
        return new ParseReply(ParseAnalytics(errorCode, map, dateTime));
    }

    ParseAnalytics::ParseAnalytics(const QString& eventName, const QVariantMap& map, const QDateTime& dateTime)
        : _eventName(eventName)
        , _errorCode(0)
        , _variableMap(map)
        , _dateTime(dateTime)
        , _contentType(JsonContentType)
    {
        init();
    }

    ParseAnalytics::ParseAnalytics(int errorCode, const QVariantMap& map, const QDateTime& dateTime)
        : _eventName(ErrorEvent)
        , _errorCode(errorCode)
        , _variableMap(map)
        , _dateTime(dateTime)
        , _contentType(JsonContentType)
    {
        init();
    }

    ParseAnalytics::ParseAnalytics(const ParseAnalytics & request)
    {
        _contentType = request._contentType;
        _eventName = request._eventName;
        _errorCode = request._errorCode;
        _variableMap = request._variableMap;
        _headers = request._headers;
    }

    ParseAnalytics & ParseAnalytics::operator=(const ParseAnalytics &request)
    {
        _contentType = request._contentType;
        _eventName = request._eventName;
        _errorCode = request._errorCode;
        _variableMap = request._variableMap;
        _headers = request._headers;
        return *this;
    }

    void ParseAnalytics::init()
    {
        _eventName.remove(" ");

        _headers.insert("User-Agent", ParseRequest::userAgent());
        _headers.insert("X-Parse-Application-Id", ParseClient::get()->applicationId());
        _headers.insert("X-Parse-REST-API-Key", ParseClient::get()->clientKey());
    }

    QString ParseAnalytics::fullUrl() const
    {
        return ParseClient::get()->serverUrl() + "events/" + _eventName;
    }

    QString ParseAnalytics::contentType() const
    {
        return _contentType;
    }

    void ParseAnalytics::setContentType(const QString &contentType)
    {
        _contentType = contentType;
    }

    QByteArray ParseAnalytics::content() const
    {
        QJsonObject jsonObject;
        QJsonObject dimensionsObject;

        if (!_dateTime.isNull())
        {
            QJsonObject dateObject;
            dateObject.insert("__type", "Date");
            dateObject.insert("iso", _dateTime.toString(Qt::ISODateWithMs));
            jsonObject.insert("at", dateObject);
        }

        if (_eventName == ErrorEvent)
        {
            dimensionsObject.insert("code", _errorCode);
        }

        if (!_variableMap.isEmpty())
        {
            QJsonObject mapObject = QJsonObject::fromVariantMap(_variableMap);
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
        
        return doc.toJson(QJsonDocument::Compact);
    }

    QByteArray ParseAnalytics::header(const QByteArray & header) const
    {
        return _headers.value(header);
    }

    void ParseAnalytics::setHeader(const QByteArray & header, const QByteArray & value)
    {
        _headers.insert(header, value);
    }

    void ParseAnalytics::removeHeader(const QByteArray & header)
    {
        _headers.remove(header);
    }

    QNetworkRequest ParseAnalytics::networkRequest() const
    {
        QUrl url(fullUrl());
        QNetworkRequest request(url);

        if (!_contentType.isEmpty())
            request.setHeader(QNetworkRequest::ContentTypeHeader, _contentType.toUtf8());

        for (auto & header : _headers.keys())
            request.setRawHeader(header, _headers.value(header));

        return request;
    }

    QNetworkReply* ParseAnalytics::sendRequest(QNetworkAccessManager *pNam) const
    {
        QNetworkReply *pReply = nullptr;

        if (ParseClient::get()->isLoggingEnabled())
            logRequest();

        if (!pNam)
            pNam = ParseClient::networkAccessManager();

        pReply = pNam->post(networkRequest(), content());

        return pReply;
    }

    void ParseAnalytics::logRequest() const
    {
        QString method = "POST";

        qDebug() << QString("Network Request: %1 %2").arg(method, fullUrl());
        
        for (auto const& key : _headers.keys())
            qDebug() << "Header " << key << ": " << _headers.value(key);

        qDebug() << "Content Type " << _contentType;
        qDebug().noquote() << content();
    }
}
