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
        pUser->setProperties(propertyMap(true));
        return pUser;
    }

    bool ParseUser::isAuthenticated() const
    {
        return !_sessionToken.isEmpty();
    }

    QString ParseUser::username() const
    {
        return _username;
    }

    void ParseUser::setUsername(const QString &username)
    {
        _username = username;
        emit propertyChanged("username");
    }

    QString ParseUser::password() const
    {
        return _username;
    }

    void ParseUser::setPassword(const QString &password)
    {
        _password = password;
        emit propertyChanged("password");
    }

    QString ParseUser::email() const
    {
        return _email;
    }

    void ParseUser::setEmail(const QString &email)
    {
        _email = email;
        emit propertyChanged("email");
    }

    QString ParseUser::sessionToken() const
    {
        return _sessionToken;
    }

    void ParseUser::setSessionToken(const QString &sessionToken)
    {
        _sessionToken = sessionToken;
        emit propertyChanged("sessionToken");
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