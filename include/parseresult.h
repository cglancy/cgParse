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
#ifndef CGPARSE_PARSERESULT_H
#define CGPARSE_PARSERESULT_H
#pragma once

#include "parse.h"
#include "parsetypes.h"
#include "parseerror.h"

#include <QString>
#include <QByteArray>
#include <QSharedPointer>
#include <QJsonArray>

namespace cg
{
    //
    // ParseResult
    //
	class CGPARSE_API ParseResult
	{
	public:
        ParseResult();
        ParseResult(int error);
        ParseResult(int status, const QByteArray &data, int error, const QString &errorMessage);
        ParseResult(const ParseResult &result);
        virtual ~ParseResult();

        bool isError() const { return _errorCode != ParseError::NoError; }
        int statusCode() const { return _statusCode; }
        QByteArray data() const { return _data; }
        const QByteArray & constData() const { return _data; }
        int errorCode() const { return _errorCode; }
        QString errorMessage() const { return _errorMessage; }

    private:
        int _statusCode, _errorCode;
        QByteArray _data;
        QString _errorMessage;
    };

    Q_DECLARE_METATYPE(ParseResult);

    //
    // ParseUserResult
    //
    class CGPARSE_API ParseUserResult : public ParseResult
    {
    public:
        ParseUserResult();
        ParseUserResult(int error);
        ParseUserResult(const ParseResult &result);
        ParseUserResult(const ParseUserResult &result);
        ~ParseUserResult();

        ParseUserPtr user() const;
    };

    Q_DECLARE_METATYPE(ParseUserResult);

    //
    // ParseObjectsResult
    //
    class CGPARSE_API ParseObjectsResult : public ParseResult
    {
    public:
        ParseObjectsResult();
        ParseObjectsResult(int error);
        ParseObjectsResult(const ParseResult &result);
        ParseObjectsResult(const ParseObjectsResult &result);
        ~ParseObjectsResult();

        template <class T>
        QList<QSharedPointer<T>> objects() const
        {
            QList<QSharedPointer<T>> list;

            QJsonDocument doc = QJsonDocument::fromJson(constData());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                QJsonArray jsonArray = obj.value("results").toArray();

                for (auto &jsonValue : jsonArray)
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
                                pObject->setValues(jsonObject);
                                pObject->clearDirtyState();
                                list.append(pObject);
                            }
                        }
                    }
                }
            }

            return list;
        }
    };

    Q_DECLARE_METATYPE(ParseObjectsResult);

    //
    // ParseCountResult
    //
    class CGPARSE_API ParseCountResult : public ParseResult
    {
    public:
        ParseCountResult();
        ParseCountResult(int error);
        ParseCountResult(const ParseResult &result);
        ParseCountResult(const ParseCountResult &result);
        ~ParseCountResult();

        int count() const;
    };

    Q_DECLARE_METATYPE(ParseCountResult);

    //
    // ParseSessionResult
    //
    class CGPARSE_API ParseSessionResult : public ParseResult
    {
    public:
        ParseSessionResult();
        ParseSessionResult(int error);
        ParseSessionResult(const ParseResult &result);
        ParseSessionResult(const ParseSessionResult &result);
        ~ParseSessionResult();

        ParseSessionPtr session() const;
    };

    Q_DECLARE_METATYPE(ParseSessionResult);
}

#endif // CGPARSE_PARSERESULT_H
