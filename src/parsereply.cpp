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
#include "parseerror.h"
#include "parseuser.h"
#include "parsesession.h"
#include "parseconvert.h"
#include "parserequest.h"
#include "parseclient.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QDebug>

namespace cg
{
    //
    // ParseReply
    //
    ParseReply::ParseReply(int error)
        : _pReply(nullptr),
        _statusCode(0),
        _errorCode(error)
    {
        QTimer::singleShot(200, this, &ParseReply::finished);
    }

    ParseReply::ParseReply(const ParseRequest &request, QNetworkAccessManager* pNam)
        : _pReply(nullptr),
        _statusCode(0),
        _errorCode(NoError)
    {
        sendRequest(request, pNam);
    }

    ParseReply::~ParseReply()
    {
    }

    void ParseReply::sendRequest(const ParseRequest &request, QNetworkAccessManager* pNam)
    {
        if (request.isNull())
            return;

        _pReply = request.sendRequest(pNam);
        if (_pReply)
            connect(_pReply, &QNetworkReply::finished, this, &ParseReply::replyFinished);
    }

    bool ParseReply::isError() const 
    { 
        return _errorCode != ParseError::NoError; 
    }

    int ParseReply::statusCode() const 
    { 
        return _statusCode; 
    }

    QByteArray ParseReply::data() const 
    { 
        return _data; 
    }

    const QByteArray & ParseReply::constData() const 
    { 
        return _data; 
    }

    int ParseReply::errorCode() const 
    { 
        return _errorCode; 
    }

    QString ParseReply::errorMessage() const 
    { 
        return _errorMessage; 
    }

    void ParseReply::replyFinished()
    {
        if (!_pReply)
            return;

        _errorCode = 0;

        _statusCode = statusCode(_pReply);
        _data = _pReply->readAll();

        if (isError(_statusCode))
        {
            _errorCode = errorCode(_data);
            _errorMessage = errorMessage(_data);

            if (_errorCode != ParseError::NoError)
                qWarning() << QString("Parse Error: %1 %2").arg(_errorCode).arg(_errorMessage);
        }

        if (ParseClient::get()->isLoggingEnabled())
        {
            QJsonDocument doc = QJsonDocument::fromJson(_data);
            QByteArray formatedContent = doc.toJson(QJsonDocument::Indented);

            qDebug() << QString("Network Reply: status = %1, error = %2 %3").arg(_statusCode).arg(_errorCode).arg(_errorMessage);
            qDebug().noquote() << formatedContent;
            qDebug();
        }

        emit preFinished();
        emit finished();

        _pReply->deleteLater();
    }

    int ParseReply::count() const
    {
        int count = 0;

        QJsonDocument doc = QJsonDocument::fromJson(constData());
        if (doc.isObject())
        {
            QJsonObject obj = doc.object();
            count = obj.value("count").toInt();
        }

        return count;
    }

    ParseUser ParseReply::user() const
    {
        ParseUser user = ParseUser::create();

        QJsonDocument doc = QJsonDocument::fromJson(constData());
        if (doc.isObject())
        {
            user.setValues(ParseConvert::toVariantMap(doc.object()));
            user.clearDirtyState();
        }

        return user;
    }

    ParseSession ParseReply::session() const
    {
        ParseSession session;

        QJsonDocument doc = QJsonDocument::fromJson(constData());
        if (doc.isObject())
        {
            session = ParseSession::create();
            session.setValues(ParseConvert::toVariantMap(doc.object()));
            session.clearDirtyState();
        }

        return session;
    }

    bool ParseReply::isError(int status)
    {
        return status >= 400 && status < 500;
    }

    int ParseReply::statusCode(QNetworkReply *pReply)
    {
        int status = 0;

        if (pReply)
            status = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        return status;
    }

    int ParseReply::errorCode(const QByteArray &data)
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

    QString ParseReply::errorMessage(const QByteArray &data)
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
