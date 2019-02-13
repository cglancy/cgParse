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
#include "parsefilehelper.h"
#include "parseobject.h"
#include "parsefile.h"
#include "parserequest.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

namespace cg
{
    ParseFileHelper::ParseFileHelper(ParseFilePtr pFile)
        : _pFile(pFile)
    {
    }

    ParseFileHelper::~ParseFileHelper()
    {
    }

    void ParseFileHelper::saveFile(ParseFilePtr pFile)
    {
        if (!pFile)
        {
            emit saveFileFinished(ParseFileReply());
            return;
        }

        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/files/" + pFile->name(), pFile->data(), pFile->contentType());
        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseFileHelper::privateSaveFileFinished);
    }

    void ParseFileHelper::privateSaveFileFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status = ParseRequest::statusCode(pReply);
        ParseFilePtr pFile = _pFile.lock();
        ParseFileReply fileReply;

        if (ParseRequest::isError(status))
        {
            status = ParseRequest::errorCode(pReply);
        }
        else if (pFile)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->readAll());
            if (doc.isObject() && status == 201)  // 201 = Created
            {
                QJsonObject obj = doc.object();
                pFile->setUrl(obj.value("url").toString());
                pFile->setName(obj.value("name").toString());
                fileReply.setFile(pFile);
            }
        }

        fileReply.setStatusCode(status);
        emit saveFileFinished(fileReply);

        pReply->deleteLater();
    }

    void ParseFileHelper::deleteFile(const QString & urlStr, const QString & masterKey)
    {
        if (urlStr.isEmpty() || masterKey.isEmpty())
        {
            emit deleteFileFinished(ParseError::UnknownError);
            return;
        }

        ParseRequest request(ParseRequest::DeleteHttpMethod, "/parse/files/" + urlStr);
        request.removeHeader("X-Parse-REST-API-Key");
        request.setHeader("X-Parse-Master-Key", masterKey.toUtf8());

        QNetworkReply *pReply = request.sendRequest();
        connect(pReply, &QNetworkReply::finished, this, &ParseFileHelper::privateDeleteFileFinished);
    }

    void ParseFileHelper::privateDeleteFileFinished()
    {
        QNetworkReply *pReply = qobject_cast<QNetworkReply*>(sender());
        if (!pReply)
            return;

        int status = ParseRequest::statusCode(pReply);

        if (ParseRequest::isError(status))
            status = ParseRequest::errorCode(pReply);

        emit deleteFileFinished(status);

        pReply->deleteLater();
    }
}