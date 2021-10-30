/**
* Copyright 2019 Charles Glancy (charles@glancyfamily.net)
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
#ifndef CGPARSE_PARSELIVEQUERYCLIENT_H
#define CGPARSE_PARSELIVEQUERYCLIENT_H
#pragma once

#include "parse.h"
#include "parseobject.h"
#include <QObject>
#include <QWebSocket>
#include <QString>
#include <QJsonObject>
#include <QMap>

namespace cg
{
    class ParseLiveQuerySubscription;

    class CGPARSE_API ParseLiveQueryClient : public QObject
    {
        Q_OBJECT
    public:
        static ParseLiveQueryClient * get();

    public:
        bool isOpened() const;

        void open(const QString &appId, const QString &serverUrl, const QString &restApiKey, const QString &sessionToken = QString());
        void close();
        ParseLiveQuerySubscription* subscribe(const QJsonObject &queryObject, const QString &sessionToken = QString());
        void unsubscribe(int id);

    signals:
        void error(int code, const QString &message, bool reconnect);
        void opened();
        void closed();
        void subscribed(int id);
        void unsubscribed(int id);

    private slots:
        void connected();
        void disconnected();
        void textMessageReceived(const QString &message);

    private:
        ParseLiveQueryClient();
        ~ParseLiveQueryClient();

        void sendJsonObject(const QJsonObject &jsonObject);
        ParseObject createObject(const QJsonObject &jsonObject);

    private:
        static ParseLiveQueryClient *_pInstance;
        QString _appId, _serverUrl, _restApiKey, _sessionToken;
        int _nextId;
        QWebSocket _webSocket;
        QMap<int, ParseLiveQuerySubscription*> _subscriptionMap;
    };
}

#endif // CGPARSE_PARSELIVEQUERYCLIENT_H
