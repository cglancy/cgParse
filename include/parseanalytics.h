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
#ifndef CGPARSE_PARSEANALYTICS_H
#define CGPARSE_PARSEANALYTICS_H
#pragma once

#include <QVariantMap>
#include <QByteArray>
#include <QNetworkRequest>
#include <QDateTime>

class QNetworkReply;
class QNetworkAccessManager;

namespace cg
{
    class ParseReply;

    class ParseAnalytics
    {
    public:
        static const QString JsonContentType;
        static const QString AppOpenedEvent;
        static const QString ErrorEvent;
        static const int MaxDimensions;
        static const int NoError;

        static ParseReply* trackAppOpened(const QVariantMap& map = QVariantMap(), const QDateTime& dateTime = QDateTime());
        static ParseReply* trackEvent(const QString& eventName, const QVariantMap& map = QVariantMap(), const QDateTime& dateTime = QDateTime());
        static ParseReply* trackError(int errorCode, const QVariantMap& map = QVariantMap(), const QDateTime& dateTime = QDateTime());

    public:
        ParseAnalytics(const QString& eventName, const QVariantMap& map, const QDateTime& dateTime);
        ParseAnalytics(int errorCode, const QVariantMap& map, const QDateTime& dateTime);
        ParseAnalytics(const ParseAnalytics &pa);

        ParseAnalytics& operator=(const ParseAnalytics &pa);

        QString contentType() const;
        void setContentType(const QString &contentType);

        QByteArray content() const;

        QByteArray header(const QByteArray &header) const;
        void setHeader(const QByteArray &header, const QByteArray &value);
        void removeHeader(const QByteArray &header);

        QNetworkReply* sendRequest(QNetworkAccessManager *pNam) const;

    private:
        void init();
        QString fullUrl() const;
        void logRequest() const;
        QNetworkRequest networkRequest() const;

    private:
        QString _contentType;
        QString _eventName;
        int _errorCode;
        QVariantMap _variableMap;
        QDateTime _dateTime;
        QMap<QByteArray, QByteArray> _headers;
    };
}

#endif // CGPARSE_PARSEANALYTICS_H
