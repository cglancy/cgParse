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

#include "parsereply.h"
#include "parsetypes.h"

#include <QVariant>
#include <QByteArray>
#include <QUrlQuery>
#include <QNetworkRequest>

class QNetworkReply;
class QNetworkAccessManager;

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

        static const QString JsonContentType;
        static QByteArray userAgent();

    public:
        ParseRequest(HttpMethod method, const QString &apiRoute);
        ParseRequest(HttpMethod method, const QString &apiRoute, const QByteArray &content,
            const QString &contentType = JsonContentType);
        ParseRequest(const ParseRequest &request);

        HttpMethod httpMethod() const;
        void setHttpMethod(HttpMethod method);

        QUrlQuery urlQuery() const;
        void setUrlQuery(const QUrlQuery &urlQuery);

        QString contentType() const;
        void setContentType(const QString &contentType);

        QString apiRoute() const;
        void setApiRoute(const QString &apiRoute);

        QByteArray content() const;

        QByteArray header(const QByteArray &header) const;
        void setHeader(const QByteArray &header, const QByteArray &value);
        void removeHeader(const QByteArray &header);

        QNetworkReply * sendRequest() const;

    private:
        void init();
        void logRequest() const;
        QString fullUrl() const;
        QNetworkRequest networkRequest() const;
        static QNetworkAccessManager* networkAccessManager();

    private:
        static QNetworkAccessManager *_pNam;
        HttpMethod _method;
        QString _apiRoute, _contentType;
        QByteArray _content;
        QUrlQuery _urlQuery;
        QMap<QByteArray, QByteArray> _headers;
    };
}

#endif // CGPARSE_PARSEREQUEST_H
