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
#include "parserequest.h"
#include "parseclient.h"

#include <QCoreApplication>

namespace cg
{
    const QString ParseRequest::JsonContentType = QStringLiteral("application/json");

    ParseRequest::ParseRequest(HttpMethod method, const QString & apiRoute)
        : _method(method),
        _apiRoute(apiRoute)
    {
    }

    ParseRequest::ParseRequest(HttpMethod method, const QString & apiRoute, const QByteArray & content, const QString &contentType)
        : _method(method),
        _apiRoute(apiRoute),
        _content(content),
        _contentType(contentType)
    {
    }

    ParseRequest::ParseRequest(const ParseRequest & request)
    {
        _method = request._method;
        _apiRoute = request._apiRoute;
        _contentType = request._contentType;
        _urlQuery = request._urlQuery;
        _content = request._content;
        _headers = request._headers;
    }

    QByteArray ParseRequest::userAgent()
    {
        return QString("%1 %2").arg(QCoreApplication::applicationName())
            .arg(QCoreApplication::applicationVersion()).toUtf8();
    }

    ParseRequest::HttpMethod ParseRequest::httpMethod() const
    {
        return _method;
    }

    void ParseRequest::setHttpMethod(HttpMethod method)
    {
        _method = method;
    }

    QUrlQuery ParseRequest::urlQuery() const
    {
        return _urlQuery;
    }

    void ParseRequest::setUrlQuery(const QUrlQuery & urlQuery)
    {
        _urlQuery = urlQuery;
    }

    QString ParseRequest::contentType() const
    {
        return _contentType;
    }

    void ParseRequest::setContentType(const QString &contentType)
    {
        _contentType = contentType;
    }

    QString ParseRequest::apiRoute() const
    {
        return _apiRoute;
    }

    void ParseRequest::setApiRoute(const QString & apiRoute)
    {
        _apiRoute = apiRoute;
    }

    QByteArray ParseRequest::content() const
    {
        return _content;
    }

    QByteArray ParseRequest::header(const QByteArray & header) const
    {
        return _headers.value(header);
    }

    void ParseRequest::setHeader(const QByteArray & header, const QByteArray & value)
    {
        _headers.insert(header, value);
    }

    QNetworkRequest ParseRequest::networkRequest() const
    {
        QString fullUrlStr = "https://" + ParseClient::instance()->apiHost();
        if (_apiRoute.startsWith("/"))
            fullUrlStr += _apiRoute;
        else
            fullUrlStr += "/" + _apiRoute;

        QUrl url(fullUrlStr);
        if (!_urlQuery.isEmpty())
            url.setQuery(_urlQuery);
        QNetworkRequest request(url);

        request.setRawHeader("User-Agent", userAgent());
        request.setRawHeader("X-Parse-Application-Id", ParseClient::instance()->applicationId());
        request.setRawHeader("X-Parse-REST-API-Key", ParseClient::instance()->clientKey());

        if (!_contentType.isEmpty())
            request.setHeader(QNetworkRequest::ContentTypeHeader, _contentType.toUtf8());

        for (auto & header : _headers.keys())
            request.setRawHeader(header, _headers.value(header));

        return request;
    }
}