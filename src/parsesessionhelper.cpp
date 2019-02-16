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
#include "parsesessionhelper.h"
#include "parserequest.h"
#include "parseerror.h"
#include "parsesession.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

namespace cg
{
    ParseSessionHelper::ParseSessionHelper()
    {
    }

    ParseSessionHelper::~ParseSessionHelper()
    {
    }

    void ParseSessionHelper::deleteSession(const QString &sessionToken)
    {
        if (sessionToken.isEmpty())
        {
            emit deleteSessionFinished(ParseError::UnknownError);
            return;
        }

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/logout");
        request.setHeader("X-Parse-Session-Token", sessionToken.toUtf8());
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseSessionHelper::privateDeleteSessionFinished);
    }

    void ParseSessionHelper::privateDeleteSessionFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status;
        QByteArray data;
        isError(pReply, status, data);
        emit deleteSessionFinished(status);
        pReply->deleteLater();
    }

    void ParseSessionHelper::currentSession(const QString &sessionToken)
    {
        if (sessionToken.isEmpty())
        {
            emit currentSessionFinished(ParseSessionResult());
            return;
        }

        ParseRequest request(ParseRequest::GetHttpMethod, "parse/sessions/me");
        request.setHeader("X-Parse-Session-Token", sessionToken.toUtf8());
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseSessionHelper::privateCurrentSessionFinished);
    }

    void ParseSessionHelper::privateCurrentSessionFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        ParseSessionResult sessionResult;

        int status;
        QByteArray data;

        if (!isError(pReply, status, data))
        {
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (doc.isObject())
            {
                ParseSessionPtr pSession = QSharedPointer<ParseSession>::create();
                pSession->setValues(doc.object());
                pSession->clearDirtyState();
                sessionResult.setSession(pSession);
            }
        }

        sessionResult.setStatusCode(status);
        emit currentSessionFinished(sessionResult);
        pReply->deleteLater();
    }
}