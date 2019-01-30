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

#include "cgparse.h"
#include <QObject>
#include <QByteArray>

namespace cg
{
    class ParseClientPrivate;
    class ParseObject;
    class ParseUser;
    class ParseQuery;

    class CGPARSE_API ParseClient : public QObject
    {
        Q_OBJECT
        Q_DECLARE_PRIVATE(ParseClient)
        Q_DISABLE_COPY(ParseClient)
        ParseClientPrivate * const d_ptr;
        ParseClient();
        ~ParseClient();

    public:
        static ParseClient * instance();

        void initialize(const QByteArray &appId, const QByteArray &clientKey, const QByteArray &server);
        QByteArray applicationId() const;
        QByteArray clientKey() const;
        QByteArray server() const;

        ParseUser * currentUser() const;
        void login(const QString &username, const QString &password);
        void logout();
        void become(const QString &sessionToken);
        void requestPasswordReset(const QString &email);
        void signUpUser(ParseUser *pUser);
        void deleteUser(ParseUser *pUser);

        void createObject(ParseObject *pObject);
        void fetchObject(ParseObject *pObject);
        void updateObject(ParseObject *pObject);
        void deleteObject(ParseObject *pObject);

        void createAll(const QList<ParseObject*> &objects);
        void updateAll(const QList<ParseObject*> &objects);
        void deleteAll(const QList<ParseObject*> &objects);

        void getObject(ParseQuery *pQuery, const QString &objectId);
        void findObjects(ParseQuery *pQuery);
        void countObjects(ParseQuery *pQuery);

    signals:
        void loginFinished(ParseUser *pUser, int errorCode);
        void logoutFinished(int errorCode);
        void becomeFinished(ParseUser *pUser, int errorCode);
        void requestPasswordResetFinished(int errorCode);
        void signUpUserFinished(ParseUser *pUser, int errorCode);
        void deleteUserFinished(ParseUser *pUser, int errorCode);

        void createObjectFinished(ParseObject *pObject, int errorCode);
        void fetchObjectFinished(ParseObject *pObject, int errorCode);
        void updateObjectFinished(ParseObject *pObject, int errorCode);
        void deleteObjectFinished(ParseObject *pObject, int errorCode);

        void createAllFinished(int errorCode);
        void updateAllFinished(int errorCode);
        void deleteAllFinished(int errorCode);

        void getObjectFinished(int errorCode);
        void findObjectsFinished(int count, int errorCode);
        void countObjectsFinished(int count, int errorCode);
    };
}

#endif // CGPARSE_PARSECLIENT_H
