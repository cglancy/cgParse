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

        //QNetworkReply *pReply = sendRequest(request);
        QNetworkReply *pReply = _pNam->get(request.networkRequest());
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

        userReply.setErrorCode(status);
        emit loginFinished(userReply);
        pReply->deleteLater();
    }

    void ParseRequestObject::logout(ParseUserPtr pUser)
    {
        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/logout");
        request.setHeader("X-Parse-Session-Token", pUser->sessionToken().toUtf8());
        QNetworkReply *pReply = _pNam->post(request.networkRequest(), QByteArray());
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

}