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
#include "parsereply.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace cg
{
    ParseReply::ParseReply(QNetworkReply *pReply)
        : _pReply(pReply)
    {
        if (pReply)
            connect(pReply, &QNetworkReply::finished, this, &ParseReply::requestFinished);
    }

    ParseReply::~ParseReply()
    {
    }

    int ParseReply::statusCode(QNetworkReply * pReply)
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

    int ParseReply::errorCode(QNetworkReply * pReply)
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

    bool ParseReply::isError(int statusCode)
    {
        return statusCode >= 400 && statusCode < 500;
    }

    void ParseReply::requestFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        _statusCode = statusCode(pReply);

        if (isError(_statusCode))
        {
            _errorCode = errorCode(pReply);
        }
        else
        {
            _data = pReply->readAll();
        }

        emit finished();
        _pReply->deleteLater();
        _pReply = nullptr;
    }
}