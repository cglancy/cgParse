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
#ifndef CGPARSE_PARSEREQUEST_H
#define CGPARSE_PARSEREQUEST_H
#pragma once

#include <QVariant>
#include <QByteArray>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QMap>

namespace cg
{
    class ParseRequest
    {
    public:
        enum HttpMethod
        {
            GetHttpMethod,
            PutHttpMethod,
            PostHttpMethod,
            DeleteHttpMethod
        };

        enum ContentType
        {
            NoContentType,
            JsonContentType
        };

        static QByteArray apiHost();
        static void setApiHost(const QByteArray &apiHost);

        static QByteArray applicationId();
        static void setApplicationId(const QByteArray &applicationId);

        static QByteArray clientKey();
        static void setClientKey(const QByteArray &clientKey);

        static QByteArray userAgent();
        static void setUserAgent(const QByteArray &userAgent);

    public:
        ParseRequest(HttpMethod method, const QByteArray &apiRoute);
        ParseRequest(HttpMethod method, const QByteArray &apiRoute, const QByteArray &content, ContentType contentType = JsonContentType);
        ParseRequest(const ParseRequest &request);

        HttpMethod httpMethod() const;
        void setHttpMethod(HttpMethod method);

        QUrlQuery urlQuery() const;
        void setUrlQuery(const QUrlQuery &urlQuery);

        ContentType contentType() const;
        void setContentType(ContentType contentType);

        QByteArray apiRoute() const;
        void setApiRoute(const QByteArray &apiRoute);

        QByteArray content() const;

        QByteArray header(const QByteArray &header) const;
        void setHeader(const QByteArray &header, const QByteArray &value);

        QNetworkRequest networkRequest() const;

    private:
        static QByteArray _applicationId, _clientKey, _apiHost, _userAgent;
        HttpMethod _method;
        QByteArray _apiRoute;
        ContentType _contentType;
        QUrlQuery _urlQuery;
        QByteArray _content;
        QMap<QByteArray, QByteArray> _headers;
    };
}

#endif // CGPARSE_PARSEREQUEST_H
