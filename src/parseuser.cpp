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
#include "parsereply.h"
#include "parseconvert.h"

namespace cg
{
    QSharedPointer<ParseUser> ParseUser::_pCurrentUser;
    ParseUserHelper * ParseUser::_pStaticHelper = nullptr;

    ParseUserHelper * ParseUser::staticHelper()
    {
        if (_pStaticHelper == nullptr)
            _pStaticHelper = new ParseUserHelper();

        return _pStaticHelper;
    }

    QSharedPointer<ParseUser> ParseUser::create()
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

    ParseUser::ParseUser(const ParseUser & user)
        : ParseObject(user)
    {
    }

    ParseUser::~ParseUser()
    {
    }

    // static
    QSharedPointer<ParseUser> ParseUser::currentUser()
    {
        return _pCurrentUser;
    }

    // static 
    ParseReply * ParseUser::login(const QString & username, const QString & password, QNetworkAccessManager* pNam)
    {
        ParseRequest request(ParseRequest::GetHttpMethod, "/login");
        request.setHeader("X-Parse-Revocable-Session", "1");

        QUrlQuery query;
        query.addQueryItem("username", username);
        query.addQueryItem("password", password);
        request.setUrlQuery(query);

        ParseReply *pParseReply = new ParseReply(request, pNam);
        QObject::connect(pParseReply, &ParseReply::preFinished, staticHelper(), &ParseUserHelper::loginFinished);
        return pParseReply;
    }

    // static 
    ParseReply * ParseUser::logout(QNetworkAccessManager* pNam)
    {
        QString sessionToken;
        QSharedPointer<ParseUser> pUser = currentUser();
        if (!pUser.isNull())
            sessionToken = pUser->sessionToken();

        ParseRequest request(ParseRequest::PostHttpMethod, "/logout");
        request.setHeader("X-Parse-Session-Token", sessionToken.toUtf8());

        ParseReply *pParseReply = new ParseReply(request, pNam);
        QObject::connect(pParseReply, &ParseReply::preFinished, staticHelper(), &ParseUserHelper::logoutFinished);
        return pParseReply;
    }

    // static 
    ParseReply * ParseUser::requestPasswordReset(const QString & email, QNetworkAccessManager* pNam)
    {
        QByteArray content;
        QJsonObject jsonObject;
        jsonObject.insert("email", email);
        QJsonDocument doc(jsonObject);
        content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/requestPasswordReset", content);
        return new ParseReply(request, pNam);
    }

    // static
    ParseReply * ParseUser::become(const QString & sessionToken, QNetworkAccessManager* pNam)
    {
        ParseRequest request(ParseRequest::GetHttpMethod, "/users/me");
        request.setHeader("X-Parse-Session-Token", sessionToken.toUtf8());

        ParseReply *pParseReply = new ParseReply(request, pNam);
        QObject::connect(pParseReply, &ParseReply::preFinished, staticHelper(), &ParseUserHelper::becomeFinished);
        return pParseReply;
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
        _pHelper->_pUser = sharedFromThis().staticCast<ParseUser>();

        QJsonObject object = ParseConvert::toJsonObject(toMap());
        QJsonDocument doc(object);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/users", content);
        request.setHeader("X-Parse-Revocable-Session", "1");

        ParseReply *pParseReply = new ParseReply(request, pNam);
        QObject::connect(pParseReply, &ParseReply::preFinished, _pHelper.data(), &ParseUserHelper::signUpFinished);
        return pParseReply;
    }

    ParseReply * ParseUser::deleteUser(QNetworkAccessManager* pNam)
    {
        _pHelper->_pUser = sharedFromThis().staticCast<ParseUser>();

        ParseRequest request(ParseRequest::DeleteHttpMethod, "/users/" + objectId());
        request.setHeader("X-Parse-Session-Token", sessionToken().toUtf8());

        ParseReply *pParseReply = new ParseReply(request, pNam);
        QObject::connect(pParseReply, &ParseReply::preFinished, _pHelper.data(), &ParseUserHelper::deleteUserFinished);
        return pParseReply;
    }
}
