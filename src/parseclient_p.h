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
#ifndef CGPARSE_PARSECLIENT_P_H
#define CGPARSE_PARSECLIENT_P_H
#pragma once

#include "parseclient.h"
#include <QObject>
#include <QByteArray>
#include <QNetworkRequest>
#include <QMap>
#include <QUrlQuery>

class QNetworkAccessManager;
class QNetworkReply;

namespace cg
{
    class ParseObject;
    class ParseClientPrivate;
    class ParseUser;

    class CGPARSE_API ParseClientPrivate : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(ParseClientPrivate)
        Q_DECLARE_PUBLIC(ParseClient)
        ParseClient * const q_ptr;

    public:
        ParseClientPrivate(ParseClient *pParseClient);
        ~ParseClientPrivate();

        QNetworkRequest buildRequest(const QString &apiRoute, const QUrlQuery &query = QUrlQuery(),
            bool addContentHeader = true) const;
        void setObjectProperties(ParseObject *pObject, const QJsonObject &jsonObject);
        QJsonObject toJsonObject(ParseObject *pObject, bool onlyDirtyProperties = false);
        QVariant toVariant(const QJsonValue &jsonValue, ParseObject *pParent);
        QJsonValue toJsonValue(const QVariant &variant);
        static bool isWritableProperty(const QByteArray &name);

        static ParseClient *instance;
        QByteArray applicationId;
        QByteArray clientKey;
        QByteArray server;

        QNetworkAccessManager *nam;
        QByteArray userAgent;
        QMap<QNetworkReply*, ParseObject*> replyObjectMap;
        QMap<QNetworkReply*, QList<ParseObject*>> replyObjectListMap;
        QMap<QNetworkReply*, ParseQuery*> replyQueryMap;
        ParseUser *currentUser;

    public slots:
        void loginFinished();
        void logoutFinished();
        void becomeFinished();
        void requestPasswordResetFinished();
        void signUpUserFinished();
        void deleteUserFinished();

        void createObjectFinished();
        void fetchObjectFinished();
        void updateObjectFinished();
        void deleteObjectFinished();

        void createAllFinished();
        void updateAllFinished();
        void deleteAllFinished();

        void getObjectFinished();
        void findObjectsFinished();
        void countObjectsFinished();
    };
}

#endif // CGPARSE_PARSECLIENT_P_H
