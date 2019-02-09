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

namespace cg
{
    QByteArray ParseRequest::_applicationId;
    QByteArray ParseRequest::_clientKey;
    QByteArray ParseRequest::_apiHost;
    QByteArray ParseRequest::_userAgent;

    ParseRequest::ParseRequest(HttpMethod method, const QByteArray & apiRoute)
        : _method(method),
        _apiRoute(apiRoute)
    {
    }

    ParseRequest::ParseRequest(HttpMethod method, const QByteArray & apiRoute, const QByteArray & content, ContentType contentType)
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

    QByteArray ParseRequest::apiHost()
    {
        return _apiHost;
    }

    void ParseRequest::setApiHost(const QByteArray &apiHost)
    {
        _apiHost = apiHost;
    }

    QByteArray ParseRequest::applicationId()
    {
        return _applicationId;
    }

    void ParseRequest::setApplicationId(const QByteArray &applicationId)
    {
        _applicationId = applicationId;
    }

    QByteArray ParseRequest::clientKey()
    {
        return _clientKey;
    }

    void ParseRequest::setClientKey(const QByteArray &clientKey)
    {
        _clientKey = clientKey;
    }

    QByteArray ParseRequest::userAgent()
    {
        return _userAgent;
    }

    void ParseRequest::setUserAgent(const QByteArray &userAgent)
    {
        _userAgent = userAgent;
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

    ParseRequest::ContentType ParseRequest::contentType() const
    {
        return _contentType;
    }

    void ParseRequest::setContentType(ContentType contentType)
    {
        _contentType = contentType;
    }

    QByteArray ParseRequest::apiRoute() const
    {
        return _apiRoute;
    }

    void ParseRequest::setApiRoute(const QByteArray & apiRoute)
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
        QString fullUrlStr = "https://" + _apiHost;
        if (_apiRoute.startsWith("/"))
            fullUrlStr += _apiRoute;
        else
            fullUrlStr += "/" + _apiRoute;

        QUrl url(fullUrlStr);
        if (!_urlQuery.isEmpty())
            url.setQuery(_urlQuery);
        QNetworkRequest request(url);

        request.setRawHeader("User-Agent", _userAgent);
        request.setRawHeader("X-Parse-Application-Id", _applicationId);
        request.setRawHeader("X-Parse-REST-API-Key", _clientKey);

        if (_contentType == JsonContentType)
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        for (auto & header : _headers.keys())
            request.setRawHeader(header, _headers.value(header));

        return request;
    }
}