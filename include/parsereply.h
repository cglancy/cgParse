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
#ifndef CGPARSE_PARSEREPLY_H
#define CGPARSE_PARSEREPLY_H
#pragma once

#include "parse.h"
#include "parsetypes.h"
#include "parseerror.h"
#include "parseconvert.h"

#include <QString>
#include <QByteArray>
#include <QSharedPointer>
#include <QJsonArray>

class QNetworkReply;

namespace cg
{
    class ParseRequest;

    class CGPARSE_API ParseReply : public QObject
    {
        Q_OBJECT
    public:
        ParseReply(int error = NoError);
        ParseReply(const ParseRequest &request);
        virtual ~ParseReply();

        void sendRequest(const ParseRequest &request);

        bool isError() const;
        int statusCode() const;
        int errorCode() const;
        QString errorMessage() const;

        QByteArray data() const;
        const QByteArray & constData() const;

        int count() const;
        ParseUserPtr user() const;
        ParseSessionPtr session() const;

        template <class T>
        QList<QSharedPointer<T>> objects() const
        {
            QList<QSharedPointer<T>> list;

            QJsonDocument doc = QJsonDocument::fromJson(constData());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                QJsonArray jsonArray = obj.value("results").toArray();

                for (auto jsonValue : jsonArray)
                {
                    if (jsonValue.isObject())
                    {
                        QJsonObject jsonObject = jsonValue.toObject();
                        QString objectId = jsonObject.value(Parse::ObjectIdKey).toString();
                        if (!objectId.isEmpty())
                        {
                            QSharedPointer<T> pObject = QSharedPointer<T>::create();
                            if (pObject)
                            {
                                pObject->setValues(ParseConvert::toVariantMap(jsonObject));
                                pObject->clearDirtyState();
                                list.append(pObject);
                            }
                        }
                    }
                }
            }

            return list;
        }

    signals:
        void preFinished();
        void finished();

    private slots:
        void replyFinished();

    private:
        static int statusCode(QNetworkReply *pReply);
        static int errorCode(const QByteArray &data);
        static QString errorMessage(const QByteArray &data);
        static bool isError(int status);

    private:
        QNetworkReply *_pReply;
        int _statusCode, _errorCode;
        QString _errorMessage;
        QByteArray _data;
    };

}

#endif // CGPARSE_PARSEREPLY_H
