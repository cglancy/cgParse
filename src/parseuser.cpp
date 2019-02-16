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
#include "parseuser.h"
#include "parseclient.h"
#include "parserequest.h"
#include "parseuserhelper.h"
#include <asyncfuture.h>

#include <QNetworkReply>

namespace cg
{
    ParseUserPtr ParseUser::_pCurrentUser;
    ParseUserHelper * ParseUser::_pStaticHelper = nullptr;

    ParseUserHelper * ParseUser::staticHelper()
    {
        if (_pStaticHelper == nullptr)
            _pStaticHelper = new ParseUserHelper();

        return _pStaticHelper;
    }

    ParseUserPtr ParseUser::create()
    {
        return QSharedPointer<ParseUser>::create();
    }

    QSharedPointer<ParseQuery<ParseUser>> ParseUser::query()
    {
        return QSharedPointer<ParseQuery<ParseUser>>::create();
    }

    ParseUser::ParseUser()
        : ParseObject("_User"),
        _pHelper(new ParseUserHelper())
    {
    }

    ParseUser::~ParseUser()
    {
    }

    ParseUserPtr ParseUser::currentUser()
    {
        return _pCurrentUser;
    }

    QFuture<ParseUserResult> ParseUser::login(const QString &username, const QString &password)
    {
        ParseUserHelper *pHelper = staticHelper();
        pHelper->login(username, password);
        return AsyncFuture::observe(pHelper, &ParseUserHelper::loginFinished).future();
    }

    QFuture<int> ParseUser::logout()
    {
        ParseUserHelper *pHelper = staticHelper();
        pHelper->logout(_pCurrentUser);
        return AsyncFuture::observe(pHelper, &ParseUserHelper::logoutFinished).future();
    }

    QFuture<int> ParseUser::requestPasswordReset(const QString &email)
    {
        ParseUserHelper *pHelper = staticHelper();
        pHelper->requestPasswordReset(email);
        return AsyncFuture::observe(pHelper, &ParseUserHelper::requestPasswordResetFinished).future();
    }

    bool ParseUser::isAuthenticated() const
    {
        return !sessionToken().isEmpty();
    }

    QString ParseUser::username() const
    {
        return value("username").toString();
    }

    void ParseUser::setUsername(const QString &username)
    {
        setValue("username", username);
    }

    QString ParseUser::password() const
    {
        return value("password").toString();
    }

    void ParseUser::setPassword(const QString &password)
    {
        setValue("password", password);
    }

    QString ParseUser::email() const
    {
        return value("email").toString();
    }

    void ParseUser::setEmail(const QString &email)
    {
        setValue("email", email);
    }

    QString ParseUser::sessionToken() const
    {
        return value("sessionToken").toString();
    }

    QFuture<ParseUserResult> ParseUser::signUp()
    {
        _pHelper->signUpUser(sharedFromThis().staticCast<ParseUser>());
        return AsyncFuture::observe(_pHelper.data(), &ParseUserHelper::signUpUserFinished).future();
    }

    QFuture<int> ParseUser::deleteUser()
    {
        _pHelper->deleteUser(sharedFromThis().staticCast<ParseUser>());
        return AsyncFuture::observe(_pHelper.data(), &ParseUserHelper::deleteUserFinished).future();
    }
}