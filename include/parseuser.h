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
#ifndef CGPARSE_PARSEUSER_H
#define CGPARSE_PARSEUSER_H
#pragma once

#include "parse.h"
#include "parseobject.h"

namespace cg
{
    class ParseReply;

    class CGPARSE_API ParseUser : public ParseObject
    {
    public:
        static ParseUser create();
        static ParseUser currentUser();

        static ParseReply* login(const QString &username, const QString &password, QNetworkAccessManager* pNam = nullptr);
        static ParseReply* logout(QNetworkAccessManager* pNam = nullptr);
        static ParseReply* requestPasswordReset(const QString &email, QNetworkAccessManager* pNam = nullptr);
        static ParseReply* become(const QString &sessionToken, QNetworkAccessManager* pNam = nullptr);

    public:
        ParseUser();
        ParseUser(const ParseUser &user);
        explicit ParseUser(const ParseObject& object);
        ~ParseUser();

        ParseUser& operator=(const ParseUser& user);

        void assign(const ParseObject& object) override;

        bool isAuthenticated() const;

        QString username() const;
        void setUsername(const QString &username);

        QString password() const;
        void setPassword(const QString &password);

        QString email() const;
        void setEmail(const QString &email);

        QString sessionToken() const;

        ParseReply* signUp(QNetworkAccessManager* pNam = nullptr);
        ParseReply* deleteUser(QNetworkAccessManager* pNam = nullptr);

    private:
        friend class ParseUserRequest;
        static ParseUser _currentUser;
    };
}

Q_DECLARE_METATYPE(cg::ParseUser);

#endif // CGPARSE_PARSEUSER_H
