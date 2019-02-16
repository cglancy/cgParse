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
#ifndef CGPARSE_PARSEUSERHELPER_H
#define CGPARSE_PARSEUSERHELPER_H
#pragma once

#include "parse.h"
#include "parsehelperbase.h"
#include "parsetypes.h"
#include "parseresult.h"

#include <QObject>
#include <QWeakPointer>

namespace cg
{
    class CGPARSE_API ParseUserHelper : public QObject, public ParseHelperBase
    {
        Q_OBJECT
    public:
        ParseUserHelper();
        ~ParseUserHelper();

    public slots:
        void login(const QString &username, const QString &password);
        void logout(ParseUserPtr pUser);
        void become(const QString &sessionToken);
        void requestPasswordReset(const QString &email);
        void signUpUser(ParseUserPtr pUser);
        void deleteUser(ParseUserPtr pUser);

    private:
        void privateLoginFinished();
        void privateLogoutFinished();
        void privateBecomeFinished();
        void privateRequestPasswordResetFinished();
        void privateSignUpUserFinished();
        void privateDeleteUserFinished();

    signals:
        void loginFinished(ParseUserResult userReply);
        void logoutFinished(int status);
        void becomeFinished(ParseUserResult userReply);
        void requestPasswordResetFinished(int status);
        void signUpUserFinished(ParseUserResult userReply);
        void deleteUserFinished(int status);

    private:
        QWeakPointer<ParseUser> _pUser;
    };
}

#endif // CGPARSE_PARSEUSERHELPER_H
