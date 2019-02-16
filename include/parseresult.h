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
        ParseResult(const ParseResult &result);
        virtual ~ParseResult();

        int statusCode() const { return _statusCode; }
        void setStatusCode(int statusCode) { _statusCode = statusCode; }

    private:
        int _statusCode;
    };

    Q_DECLARE_METATYPE(ParseResult);

    //
    // ParseUserResult
    //
    class CGPARSE_API ParseUserResult : public ParseResult
    {
    public:
        ParseUserResult();
        ParseUserResult(const ParseUserResult &result);
        ~ParseUserResult();

        ParseUserPtr user() const { return _pUser; }
        void setUser(ParseUserPtr pUser) { _pUser = pUser; }

    private:
        ParseUserPtr _pUser;
    };

    Q_DECLARE_METATYPE(ParseUserResult);

    //
    // ParseFileResult
    //
    class CGPARSE_API ParseFileResult: public ParseResult
    {
    public:
        ParseFileResult();
        ParseFileResult(const ParseFileResult &result);
        ~ParseFileResult();

        ParseFilePtr file() const { return _pFile; }
        void setFile(ParseFilePtr pFile) { _pFile = pFile; }

    private:
        ParseFilePtr _pFile;
    };

    Q_DECLARE_METATYPE(ParseFileResult);

    //
    // ParseObjectsResult
    //
    class CGPARSE_API ParseObjectsResult : public ParseResult
    {
    public:
        ParseObjectsResult();
        ParseObjectsResult(const ParseObjectsResult &result);
        ~ParseObjectsResult();

        template <class T>
        QList<QSharedPointer<T>> objects() const
        {
            QList<QSharedPointer<T>> list;
            for (auto &jsonValue : _jsonArray)
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

            return list;
        }

        QJsonArray jsonArray() const { return _jsonArray; }
        void setJsonArray(const QJsonArray &jsonArray) { _jsonArray = jsonArray; }

    private:
        QJsonArray _jsonArray;
    };

    Q_DECLARE_METATYPE(ParseObjectsResult);

    //
    // ParseCountResult
    //
    class CGPARSE_API ParseCountResult : public ParseResult
    {
    public:
        ParseCountResult();
        ParseCountResult(const ParseCountResult &result);
        ~ParseCountResult();

        int count() const { return _count; }
        void setCount(int count) { _count = count; }

    private:
        int _count;
    };

    Q_DECLARE_METATYPE(ParseCountResult);

    //
    // ParseSessionResult
    //
    class CGPARSE_API ParseSessionResult : public ParseResult
    {
    public:
        ParseSessionResult();
        ParseSessionResult(const ParseSessionResult &result);
        ~ParseSessionResult();

        ParseSessionPtr session() const { return _pSession; }
        void setSession(ParseSessionPtr pSession) { _pSession = pSession; }

    private:
        ParseSessionPtr _pSession;
    };

    Q_DECLARE_METATYPE(ParseSessionResult);
}

#endif // CGPARSE_PARSERESULT_H
