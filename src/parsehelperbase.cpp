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
#include "parsehelperbase.h"
#include "parseerror.h"
#include "parseclient.h"

#include <QNetworkReply>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace cg
{
    bool ParseHelperBase::isError(QNetworkReply *pReply, int &status, QByteArray &data)
    {
        bool rtn = false;

        if (!pReply)
            return true;
        
        int error = 0;
        QString message;

        status = statusCode(pReply);
        data = pReply->readAll();

        if (isError(status))
        {
            error = errorCode(data);
            message = errorMessage(data);
            rtn = true;

            if (error != 0)
                qWarning() << QString("Parse Error: %1 %2").arg(error).arg(message);
        }

        if (ParseClient::get()->isLoggingEnabled())
        {
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QByteArray formatedContent = doc.toJson(QJsonDocument::Indented);

            qDebug() << QString("Network Reply: status = %1, error = %2 %3").arg(status).arg(error).arg(message);
            qDebug().noquote() << formatedContent;
            qDebug();
        }

        status = error == 0 ? status : error;
        return rtn;
    }

    bool ParseHelperBase::isError(int status)
    {
        return status >= 400 && status < 500;
    }

    int ParseHelperBase::statusCode(QNetworkReply *pReply)
    {
        int status = 0;

        if (pReply)
            status = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        return status;
    }

    int ParseHelperBase::errorCode(const QByteArray &data)
    {
        int error = 0;

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject())
        {
            QJsonObject jsonObject = doc.object();
            error = jsonObject.value("code").toInt();
        }

        return error;
    }

    QString ParseHelperBase::errorMessage(const QByteArray &data)
    {
        QString message;

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject())
        {
            QJsonObject jsonObject = doc.object();
            message = jsonObject.value("error").toString();
        }

        return message;
    }

}