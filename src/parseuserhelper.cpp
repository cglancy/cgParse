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
#include "parseuserhelper.h"
#include "parseuser.h"
#include "parserequest.h"


#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

namespace cg
{
    ParseUserHelper::ParseUserHelper()
    {
    }

    ParseUserHelper::~ParseUserHelper()
    {
    }

    void ParseUserHelper::login(const QString &username, const QString &password)
    {
        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/login");
        request.setHeader("X-Parse-Revocable-Session", "1");

        QUrlQuery query;
        query.addQueryItem("username", username);
        query.addQueryItem("password", password);
        request.setUrlQuery(query);

        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseUserHelper::privateLoginFinished);
    }

    void ParseUserHelper::privateLoginFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseUserResult userReply(replyResult(pReply));

        if (!userReply.isError() && userReply.statusCode() == 200)
        {
            ParseUser::_pCurrentUser = userReply.user();
        }

        emit loginFinished(userReply);
        pReply->deleteLater();
    }

    void ParseUserHelper::logout(ParseUserPtr pUser)
    {
        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/logout");
        request.setHeader("X-Parse-Session-Token", pUser->sessionToken().toUtf8());
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseUserHelper::privateLogoutFinished);
    }

    void ParseUserHelper::privateLogoutFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseResult result = replyResult(pReply);

        if (!result.isError())
        {
            ParseUser::_pCurrentUser = nullptr;
        }

        emit logoutFinished(result.errorCode());
        pReply->deleteLater();
    }

    void ParseUserHelper::become(const QString &sessionToken)
    {
        if (sessionToken.isEmpty())
        {
            emit becomeFinished(ParseUserResult());
            return;
        }

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/users/me");
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseUserHelper::privateBecomeFinished);
    }

    void ParseUserHelper::privateBecomeFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseUserResult userReply(replyResult(pReply));

        if (!userReply.isError())
        {
            ParseUser::_pCurrentUser = userReply.user();
        }

        emit becomeFinished(userReply);
        pReply->deleteLater();
    }

    void ParseUserHelper::requestPasswordReset(const QString &email)
    {
        if (email.isEmpty())
        {
            emit requestPasswordResetFinished(1);
            return;
        }

        QByteArray content;
        QJsonObject jsonObject;
        jsonObject.insert("email", email);
        QJsonDocument doc(jsonObject);
        content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/requestPasswordReset", content);
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseUserHelper::privateRequestPasswordResetFinished);
    }

    void ParseUserHelper::privateRequestPasswordResetFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseResult result = replyResult(pReply);
        emit requestPasswordResetFinished(result.errorCode());
        pReply->deleteLater();
    }

    void ParseUserHelper::signUpUser(ParseUserPtr pUser)
    {
        if (!pUser)
        {
            emit signUpUserFinished(ParseError::UnknownError);
            return;
        }

        _pUser = pUser;
        QJsonObject object = pUser->toJsonObject();
        QJsonDocument doc(object);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/users", content);
        request.setHeader("X-Parse-Revocable-Session", "1");
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseUserHelper::privateSignUpUserFinished);
    }

    void ParseUserHelper::privateSignUpUserFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseUserPtr pUser = _pUser.lock();
        ParseResult result = replyResult(pReply);

        if (!result.isError() && pUser)
        {
            QJsonDocument doc = QJsonDocument::fromJson(result.data());
            if (doc.isObject())
            {
                if (result.statusCode() == 201) // Created
                {
                    QJsonObject obj = doc.object();
                    pUser->setValues(obj);
                    pUser->clearDirtyState();

                    ParseUser::_pCurrentUser = pUser;
                }
            }
        }

        emit signUpUserFinished(result.errorCode());
        pReply->deleteLater();
    }

    void ParseUserHelper::deleteUser(ParseUserPtr pUser)
    {
        if (!pUser || pUser->objectId().isEmpty() || pUser->sessionToken().isEmpty())
        {
            emit deleteUserFinished(ParseError::UnknownError);
            return;
        }

        _pUser = pUser;
        ParseRequest request(ParseRequest::DeleteHttpMethod, "/parse/users/" + pUser->objectId());
        request.setHeader("X-Parse-Session-Token", pUser->sessionToken().toUtf8());

        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseUserHelper::privateDeleteUserFinished);
    }

    void ParseUserHelper::privateDeleteUserFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseUserPtr pUser = _pUser.lock();
        ParseResult result = replyResult(pReply);

        if (!result.isError() && pUser)
        {
            if (pUser == ParseUser::_pCurrentUser)
                ParseUser::_pCurrentUser = nullptr;
        }

        emit deleteUserFinished(result.errorCode());
        pReply->deleteLater();
    }
}