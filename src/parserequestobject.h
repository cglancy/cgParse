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
#ifndef CGPARSE_PARSEREQUESTOBJECT_H
#define CGPARSE_PARSEREQUESTOBJECT_H
#pragma once

#include "cgparse.h"
#include "parsetypes.h"
#include "parserequest.h"
#include "parsereply.h"

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

namespace cg
{
    class CGPARSE_API ParseRequestObject : public QObject
    {
        Q_OBJECT
    public:
        static ParseRequestObject * instance();

    public slots:
        void login(const QString &username, const QString &password);
        void logout(ParseUserPtr pUser);

    private:
        void privateLoginFinished();
        void privateLogoutFinished();

    signals:
        void loginFinished(ParseUserReply userReply);
        void logoutFinished(int error);

    private:
        ParseRequestObject();
        ~ParseRequestObject();

        QNetworkReply* sendRequest(const ParseRequest &request);
        static int statusCode(QNetworkReply *pReply);
        static int errorCode(QNetworkReply *pReply);
        static bool isError(int status);

    private:
        static ParseRequestObject *_pInstance;
        QNetworkAccessManager *_pNam;
    };
}

#endif // CGPARSE_PARSEREQUESTOBJECT_H
