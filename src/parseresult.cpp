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
#include "parseresult.h"
#include "parseerror.h"
#include "parseuser.h"
#include "parsesession.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace cg
{
    //
    // ParseResult
    //
    ParseResult::ParseResult()
        : _errorCode(ParseError::NoError),
        _statusCode(0)
    {
    }

    ParseResult::ParseResult(int error)
        : _errorCode(error),
        _statusCode(0)
    {
    }

    ParseResult::ParseResult(int status, const QByteArray &data, int error, const QString &errorMessage)
        : _statusCode(status),
        _data(data),
        _errorCode(error),
        _errorMessage(errorMessage)
    {
    }

    ParseResult::ParseResult(const ParseResult &result)
    {
        _statusCode = result._statusCode;
        _data = result._data;
        _errorCode = result._errorCode;
        _errorMessage = result._errorMessage;
    }

    ParseResult::~ParseResult()
    {
    }

    //
    // ParseUserResult
    //
    ParseUserResult::ParseUserResult()
    {
    }

    ParseUserResult::ParseUserResult(int error)
        : ParseResult(error)
    {
    }

    ParseUserResult::ParseUserResult(const ParseResult & result)
        : ParseResult(result)
    {
    }

    ParseUserResult::ParseUserResult(const ParseUserResult &result)
        : ParseResult(result)
    {
    }

    ParseUserResult::~ParseUserResult()
    {
    }

    ParseUserPtr ParseUserResult::user() const
    {
        ParseUserPtr pUser;

        QJsonDocument doc = QJsonDocument::fromJson(constData());
        if (doc.isObject())
        {
            pUser = QSharedPointer<ParseUser>::create();
            pUser->setValues(doc.object());
            pUser->clearDirtyState();
        }

        return pUser;
    }

    //
    // ParseObjectsResult
    //
    ParseObjectsResult::ParseObjectsResult()
    {
    }

    ParseObjectsResult::ParseObjectsResult(int error)
        : ParseResult(error)
    {
    }

    ParseObjectsResult::ParseObjectsResult(const ParseResult & result)
        : ParseResult(result)
    {
    }

    ParseObjectsResult::ParseObjectsResult(const ParseObjectsResult &result)
        : ParseResult(result)
    {
    }

    ParseObjectsResult::~ParseObjectsResult()
    {
    }

    //
    // ParseCountResult
    //
    ParseCountResult::ParseCountResult()
    {
    }

    ParseCountResult::ParseCountResult(int error)
        : ParseResult(error)
    {
    }

    ParseCountResult::ParseCountResult(const ParseResult & result)
        : ParseResult(result)
    {
    }

    ParseCountResult::ParseCountResult(const ParseCountResult &result)
        : ParseResult(result)
    {
    }

    ParseCountResult::~ParseCountResult()
    {
    }

    int ParseCountResult::count() const
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

    //
    // ParseSessionResult
    //    
    ParseSessionResult::ParseSessionResult()
    {
    }

    ParseSessionResult::ParseSessionResult(int error)
        : ParseResult(error)
    {
    }

    ParseSessionResult::ParseSessionResult(const ParseResult & result)
        : ParseResult(result)
    {
    }

    ParseSessionResult::ParseSessionResult(const ParseSessionResult &result)
        : ParseResult(result)
    {
    }

    ParseSessionResult::~ParseSessionResult()
    {
    }

    ParseSessionPtr ParseSessionResult::session() const
    {
        ParseSessionPtr pSession;

        QJsonDocument doc = QJsonDocument::fromJson(constData());
        if (doc.isObject())
        {
            pSession = QSharedPointer<ParseSession>::create();
            pSession->setValues(doc.object());
            pSession->clearDirtyState();
        }

        return pSession;
    }

}