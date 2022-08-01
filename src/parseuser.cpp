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
#include "parseuserrequest.h"
#include "parserequest.h"
#include "parsereply.h"
#include "parseconvert.h"

namespace cg
{
    ParseUser ParseUser::_currentUser;

    ParseUser::ParseUser()
    {
        // constructs null object
    }

    ParseUser::ParseUser(const ParseUser & user)
        : ParseObject(user)
    {
    }

    ParseUser::ParseUser(const ParseObject& object)
    {
        if (object.className() == Parse::UserClassNameKey)
            ParseObject::assign(object);
        else
            nullify();
    }

    ParseUser::~ParseUser()
    {
    }

    ParseUser& ParseUser::operator=(const ParseUser& user)
    {
        ParseObject::operator=(user);
        return *this;
    }

    void ParseUser::assign(const ParseObject& object)
    {
        if (object.className() == Parse::UserClassNameKey)
            ParseObject::assign(object);
        else
            nullify();
    }

    // static 
    ParseUser ParseUser::create()
    {
        return ParseUser(ParseObject(Parse::UserClassNameKey));
    }

    // static
    ParseUser ParseUser::currentUser()
    {
        return _currentUser;
    }

    // static 
    ParseReply * ParseUser::login(const QString & username, const QString & password, QNetworkAccessManager* pNam)
    {
        return ParseUserRequest::get()->login(username, password, pNam);
    }

    // static 
    ParseReply * ParseUser::logout(QNetworkAccessManager* pNam)
    {
        return ParseUserRequest::get()->logout(pNam);
    }

    // static 
    ParseReply * ParseUser::requestPasswordReset(const QString & email, QNetworkAccessManager* pNam)
    {
        return ParseUserRequest::get()->requestPasswordReset(email, pNam);
    }

    // static
    ParseReply * ParseUser::become(const QString & sessionToken, QNetworkAccessManager* pNam)
    {
        return ParseUserRequest::get()->become(sessionToken, pNam);
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

    ParseReply * ParseUser::signUp(QNetworkAccessManager* pNam)
    {
        return ParseUserRequest::get()->signUp(*this, pNam);
    }

    ParseReply * ParseUser::deleteUser(QNetworkAccessManager* pNam)
    {
        return ParseUserRequest::get()->deleteUser(*this, pNam);
    }
}
