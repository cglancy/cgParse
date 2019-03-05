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
#ifndef CGPARSE_PARSECLIENT_H
#define CGPARSE_PARSECLIENT_H
#pragma once

#include "parse.h"
#include <QByteArray>

class QNetworkAccessManager;

namespace cg
{
    class CGPARSE_API ParseClient
    {
    public:
        static ParseClient * get();
        static QNetworkAccessManager* networkAccessManager();

    public:
        void initialize(const QByteArray &appId, const QByteArray &clientKey, const QByteArray &serverUrl);
        QByteArray applicationId() const;
        QByteArray clientKey() const;
        QByteArray serverUrl() const;
        bool isLoggingEnabled() const;
        void setLoggingEnabled(bool enabled);

    private:
        ParseClient();
        ~ParseClient();

        static ParseClient *_pInstance;
        static QNetworkAccessManager *_pNetworkAccessManager;
        QByteArray _appId, _clientKey, _serverUrl;
        bool _loggingEnabled;
    };
}

#endif // CGPARSE_PARSECLIENT_H
