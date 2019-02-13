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
#include "parserequestobject.h"
#include "parseobject.h"
#include "parseuser.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSignalSpy>
#include <QJsonDocument>
#include <QJsonObject>

namespace cg
{
    ParseRequestObject * ParseRequestObject::_pInstance = nullptr;

    ParseRequestObject::ParseRequestObject()
        : _pNam(nullptr)
    {
        _pNam = new QNetworkAccessManager();
    }

    ParseRequestObject::~ParseRequestObject()
    {
        delete _pNam;
    }

    ParseRequestObject * ParseRequestObject::instance()
    {
        if (!_pInstance)
            _pInstance = new ParseRequestObject();

        return _pInstance;
    }

    QNetworkReply* ParseRequestObject::sendRequest(const ParseRequest &request)
    {
        QNetworkReply *pReply = nullptr;

        switch (request.httpMethod())
        {
        case ParseRequest::PutHttpMethod:
            pReply = _pNam->put(request.networkRequest(), request.content());
            break;
        case ParseRequest::PostHttpMethod:
            pReply = _pNam->post(request.networkRequest(), request.content());
            break;
        case ParseRequest::DeleteHttpMethod:
            pReply = _pNam->deleteResource(request.networkRequest());
            break;
        default:
        case ParseRequest::GetHttpMethod:
            pReply = _pNam->get(request.networkRequest());
            break;
        }

        return pReply;
    }

    bool ParseRequestObject::isError(int status)
    {
        return status >= 400 && status < 500;
    }

    int ParseRequestObject::statusCode(QNetworkReply *pReply)
    {
        int status = 0;

        if (pReply)
        {
            status = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
#if 0
            int replyError = pReply->error();
            qDebug() << "Status = " << status << ", Network Error = " << replyError;
#endif
        }

        return status;
    }

    int ParseRequestObject::errorCode(QNetworkReply *pReply)
    {
        int error = 0;

        if (pReply)
        {
            QByteArray data = pReply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject())
            {
                QJsonObject jsonObject = doc.object();
                error = jsonObject.value("code").toInt();

#if 1
                QString message = jsonObject.value("error").toString();
                qDebug() << "Error: " << message;
#endif
            }
        }
        else
        {
            error = -1;
        }

        return error;
    }

    void ParseRequestObject::login(const QString &username, const QString &password)
    {
        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/login");
        request.setHeader("X-Parse-Revocable-Session", "1");

        QUrlQuery query;
        query.addQueryItem("username", username);
        query.addQueryItem("password", password);
        request.setUrlQuery(query);

        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateLoginFinished);
    }

    void ParseRequestObject::privateLoginFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseUserReply userReply;

        int status = statusCode(pReply);
        if (isError(status))
        {
           status = errorCode(pReply);
        }
        else if (status == 200)
        {
            QByteArray data = pReply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject())
            {
                ParseUserPtr pUser = QSharedPointer<ParseUser>::create();
                pUser->setValues(doc.object());
                pUser->clearDirtyState();
                userReply.setUser(pUser);

                ParseUser::_pCurrentUser = pUser;
            }
        }

        userReply.setStatusCode(status);
        emit loginFinished(userReply);
        pReply->deleteLater();
    }

    void ParseRequestObject::logout(ParseUserPtr pUser)
    {
        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/logout");
        request.setHeader("X-Parse-Session-Token", pUser->sessionToken().toUtf8());
        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateLogoutFinished);
    }

    void ParseRequestObject::privateLogoutFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status = statusCode(pReply);
        if (isError(status))
        {
            status = errorCode(pReply);
        }
        else
        {
            ParseUser::_pCurrentUser = nullptr;
        }

        emit logoutFinished(status);
        pReply->deleteLater();
    }

    void ParseRequestObject::become(const QString &sessionToken)
    {
        if (sessionToken.isEmpty())
        {
            emit becomeFinished(ParseUserReply());
            return;
        }

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/users/me");
        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateBecomeFinished);
        //_replyStringMap.insert(pReply, sessionToken);
    }

    void ParseRequestObject::privateBecomeFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseUserReply userReply;
        int status = statusCode(pReply);
        if (isError(status))
        {
            status = errorCode(pReply);
        }
        else
        {
            QByteArray data = pReply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject())
            {
                ParseUserPtr pUser = QSharedPointer<ParseUser>::create();
                pUser->setValues(doc.object());
                pUser->clearDirtyState();
                userReply.setUser(pUser);

                ParseUser::_pCurrentUser = pUser;
            }
        }

        userReply.setStatusCode(status);
        emit becomeFinished(userReply);
        pReply->deleteLater();
    }

    void ParseRequestObject::requestPasswordReset(const QString &email)
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
        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateRequestPasswordResetFinished);
        //d->replyStringMap.insert(pReply, email);
    }

    void ParseRequestObject::privateRequestPasswordResetFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status = statusCode(pReply);
        if (isError(status))
            status = errorCode(pReply);

        emit requestPasswordResetFinished(status);
        pReply->deleteLater();
    }

    void ParseRequestObject::signUpUser(ParseUserPtr pUser)
    {
        if (!pUser)
        {
            emit signUpUserFinished(ParseUserReply());
            return;
        }

        QJsonObject object = pUser->toJsonObject();
        QJsonDocument doc(object);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/users", content);
        request.setHeader("X-Parse-Revocable-Session", "1");
        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateSignUpUserFinished);
        _replyUserMap.insert(pReply, pUser);
    }

    void ParseRequestObject::privateSignUpUserFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status = statusCode(pReply);
        ParseUserPtr pUser = _replyUserMap.take(pReply);

        ParseUserReply userReply;
        if (isError(status))
        {
            status = errorCode(pReply);
        }
        else if (pUser)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
            if (doc.isObject())
            {
                if (status == 201) // Created
                {
                    QJsonObject obj = doc.object();
                    pUser->setValues(obj);
                    pUser->clearDirtyState();

                    userReply.setUser(pUser);
                    ParseUser::_pCurrentUser = pUser;
                }
            }
        }

        userReply.setStatusCode(status);
        emit signUpUserFinished(userReply);
        pReply->deleteLater();
    }

    void ParseRequestObject::deleteSession(const QString &sessionToken)
    {
        if (sessionToken.isEmpty())
        {
            emit deleteSessionFinished(ParseError::UnknownError);
            return;
        }

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/logout");
        request.setHeader("X-Parse-Session-Token", sessionToken.toUtf8());
        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateDeleteSessionFinished);
        //d->replyStringMap.insert(pReply, sessionToken);
    }

    void ParseRequestObject::privateDeleteSessionFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status = statusCode(pReply);

        if (isError(status))
            status = errorCode(pReply);

        emit deleteSessionFinished(status);
        pReply->deleteLater();
    }

    void ParseRequestObject::deleteUser(ParseUserPtr pUser)
    {
        if (!pUser || pUser->objectId().isEmpty() || pUser->sessionToken().isEmpty())
        {
            emit deleteUserFinished(ParseError::UnknownError);
            return;
        }

        ParseRequest request(ParseRequest::DeleteHttpMethod, "/parse/users/" + pUser->objectId());
        request.setHeader("X-Parse-Session-Token", pUser->sessionToken().toUtf8());

        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateDeleteUserFinished);
        _replyUserMap.insert(pReply, pUser);
    }

    void ParseRequestObject::privateDeleteUserFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseUserPtr pUser = _replyUserMap.take(pReply);
        int status = statusCode(pReply);

        if (isError(status))
        {
            status = errorCode(pReply);
        }
        else if (pUser)
        {
            if (pUser == ParseUser::_pCurrentUser)
                ParseUser::_pCurrentUser = nullptr;
        }

        emit deleteUserFinished(status);
        pReply->deleteLater();
    }

    void ParseRequestObject::createObject(ParseObjectPtr pObject)
    {
        if (!pObject)
        {
            emit createObjectFinished(ParseError::UnknownError);
            return;
        }

        QJsonObject object = pObject->toJsonObject();
        QJsonDocument doc(object);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/classes/" + pObject->className(), content);

        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateCreateObjectFinished);
        _replyObjectMap.insert(pReply, pObject);
    }

    void ParseRequestObject::privateCreateObjectFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseObjectPtr pObject = _replyObjectMap.take(pReply);
        int status = statusCode(pReply);

        if (isError(status))
        {
            status = errorCode(pReply);
        }
        else if (pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                pObject->setValues(obj);
                pObject->clearDirtyState();
            }
        }

        emit createObjectFinished(status);
        pReply->deleteLater();
    }

    void ParseRequestObject::fetchObject(ParseObjectPtr pObject)
    {
        if (!pObject)
        {
            emit fetchObjectFinished(ParseError::UnknownError);
            return;
        }

        ParseRequest request(ParseRequest::GetHttpMethod, "/parse/classes/" + pObject->className() + "/" + pObject->objectId());
        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateFetchObjectFinished);
        _replyObjectMap.insert(pReply, pObject);
    }

    void ParseRequestObject::privateFetchObjectFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseObjectPtr pObject = _replyObjectMap.take(pReply);
        int status = statusCode(pReply);

        if (isError(status))
        {
            status = errorCode(pReply);
        }
        else if (pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                pObject->setValues(obj);
                pObject->clearDirtyState();
            }
        }

        emit fetchObjectFinished(status);
        pReply->deleteLater();
    }

    void ParseRequestObject::updateObject(ParseObjectPtr pObject)
    {
        if (!pObject || pObject->objectId().isEmpty())
        {
            emit updateObjectFinished(ParseError::UnknownError);
            return;
        }

        QJsonObject object = pObject->toJsonObject();
        QJsonDocument doc(object);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PutHttpMethod, "/parse/classes/" + pObject->className() + "/" + pObject->objectId(), content);
        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateUpdateObjectFinished);
        _replyObjectMap.insert(pReply, pObject);
    }

    void ParseRequestObject::privateUpdateObjectFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseObjectPtr pObject = _replyObjectMap.take(pReply);
        int status = statusCode(pReply);

        if (isError(status))
        {
            status = errorCode(pReply);
        }
        else if (pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                pObject->setValues(obj);
                pObject->clearDirtyState();
            }
        }

        emit updateObjectFinished(status);
        pReply->deleteLater();
    }

    void ParseRequestObject::deleteObject(ParseObjectPtr pObject)
    {
        if (!pObject || pObject->objectId().isEmpty())
        {
            emit deleteObjectFinished(ParseError::UnknownError);
            return;
        }

        ParseRequest request(ParseRequest::DeleteHttpMethod, "/parse/classes/" + pObject->className() + "/" + pObject->objectId());
        QNetworkReply *pReply = sendRequest(request);
        connect(pReply, &QNetworkReply::finished, this, &ParseRequestObject::privateDeleteObjectFinished);
        _replyObjectMap.insert(pReply, pObject);
    }

    void ParseRequestObject::privateDeleteObjectFinished()
    {
        QNetworkReply *pReply = static_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseObjectPtr pObject = _replyObjectMap.take(pReply);
        int status = statusCode(pReply);

        if (isError(status))
        {
            status = errorCode(pReply);
        }

        emit deleteObjectFinished(status);
        pReply->deleteLater();
    }
}