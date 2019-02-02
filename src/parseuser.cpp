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

namespace cg
{
    const QString ParseUser::UsernameName = QStringLiteral("username");
    const QString ParseUser::PasswordName = QStringLiteral("password");
    const QString ParseUser::EmailName = QStringLiteral("email");
    const QString ParseUser::SessionTokenName = QStringLiteral("sessionToken");

    ParseUser::ParseUser()
        : ParseObject("_User")
    {
    }

    ParseUser::~ParseUser()
    {
    }

    ParseUser * ParseUser::clone() const
    {
        ParseUser *pUser = new ParseUser();
        pUser->setValues(valueMap(false));
        return pUser;
    }

    bool ParseUser::isAuthenticated() const
    {
        return !sessionToken().isEmpty();
    }

    QString ParseUser::username() const
    {
        return value(UsernameName).toString();
    }

    void ParseUser::setUsername(const QString &username)
    {
        setValue(UsernameName, username);
    }

    QString ParseUser::password() const
    {
        return value(PasswordName).toString();
    }

    void ParseUser::setPassword(const QString &password)
    {
        setValue(PasswordName, password);
    }

    QString ParseUser::email() const
    {
        return value(EmailName).toString();
    }

    void ParseUser::setEmail(const QString &email)
    {
        setValue(EmailName, email);
    }

    QString ParseUser::sessionToken() const
    {
        return value(SessionTokenName).toString();
    }

    void ParseUser::signUp()
    {
        ParseClient::instance()->signUpUser(this);
    }

    void ParseUser::deleteUser()
    {
        ParseClient::instance()->deleteUser(this);
    }
}