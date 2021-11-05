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
#include "parsegraphql.h"
#include "parseclient.h"
#include "parsereply.h"

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace cg
{
    const QString ParseGraphQL::JsonContentType = QStringLiteral("application/json");

    ParseReply* ParseGraphQL::request(const QString& queryStr, const QString& operationStr, const QVariantMap& variables)
    {
        return new ParseReply(ParseGraphQL(queryStr, operationStr, variables));
    }

    ParseGraphQL::ParseGraphQL(const QString& queryStr, const QString& operationStr, const QVariantMap& variableMap)
        : _query(queryStr)
        , _operation(operationStr)
        , _variableMap(variableMap)
        , _contentType(JsonContentType)
    {
        init();
    }

    ParseGraphQL::ParseGraphQL(const ParseGraphQL & request)
    {
        _contentType = request._contentType;
        _query = request._query;
        _operation = request._operation;
        _variableMap = request._variableMap;
        _headers = request._headers;
    }

    ParseGraphQL & ParseGraphQL::operator=(const ParseGraphQL &request)
    {
        _contentType = request._contentType;
        _query = request._query;
        _operation = request._operation;
        _variableMap = request._variableMap;
        _headers = request._headers;
        return *this;
    }

    void ParseGraphQL::init()
    {
        _headers.insert("User-Agent", userAgent());
        _headers.insert("X-Parse-Application-Id", ParseClient::get()->applicationId());
        _headers.insert("X-Parse-Master-Key", ParseClient::get()->masterKey());
        _headers.insert("X-Parse-Client-Key", ParseClient::get()->clientKey());
    }

    QString ParseGraphQL::fullUrl() const
    {
        return ParseClient::get()->serverUrl() + "graphql";
    }

    QByteArray ParseGraphQL::userAgent()
    {
        return QString("%1 %2").arg(QCoreApplication::applicationName(), QCoreApplication::applicationVersion()).toUtf8();
    }

    QString ParseGraphQL::contentType() const
    {
        return _contentType;
    }

    void ParseGraphQL::setContentType(const QString &contentType)
    {
        _contentType = contentType;
    }

    QByteArray ParseGraphQL::content() const
    {
        QJsonObject jsonObject;
        jsonObject["query"] = _query;

        if (!_operation.isEmpty())
            jsonObject["operationName"] = _operation;

        if (!_variableMap.isEmpty())
        {
            QJsonObject variableObject = QJsonObject::fromVariantMap(_variableMap);
            jsonObject["variables"] = variableObject;
        }

        QJsonDocument doc;
        doc.setObject(jsonObject);
        
        return doc.toJson();
    }

    QByteArray ParseGraphQL::header(const QByteArray & header) const
    {
        return _headers.value(header);
    }

    void ParseGraphQL::setHeader(const QByteArray & header, const QByteArray & value)
    {
        _headers.insert(header, value);
    }

    void ParseGraphQL::removeHeader(const QByteArray & header)
    {
        _headers.remove(header);
    }

    QNetworkRequest ParseGraphQL::networkRequest() const
    {
        QUrl url(fullUrl());
        QNetworkRequest request(url);

        if (!_contentType.isEmpty())
            request.setHeader(QNetworkRequest::ContentTypeHeader, _contentType.toUtf8());

        for (auto & header : _headers.keys())
            request.setRawHeader(header, _headers.value(header));

        return request;
    }

    QNetworkReply* ParseGraphQL::sendRequest(QNetworkAccessManager *pNam) const
    {
        QNetworkReply *pReply = nullptr;

        if (ParseClient::get()->isLoggingEnabled())
            logRequest();

        if (!pNam)
            pNam = ParseClient::networkAccessManager();

        pReply = pNam->post(networkRequest(), content());

        return pReply;
    }

    void ParseGraphQL::logRequest() const
    {
        QString method = "POST";

        qDebug() << QString("Network Request: %1 %2").arg(method, fullUrl());
        
        for (auto const& key : _headers.keys())
            qDebug() << "Header " << key << ": " << _headers.value(key);

        qDebug() << "Content Type " << _contentType;
        qDebug().noquote() << content();
    }
}
