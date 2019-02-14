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

#include "cgparse.h"
#include "parsetypes.h"
#include <QJsonArray>

namespace cg
{
    //
    // ParseReply
    //
	class CGPARSE_API ParseReply
	{
	public:
        ParseReply();
        ParseReply(const ParseReply &reply);
        virtual ~ParseReply();

        int statusCode() const { return _statusCode; }
        void setStatusCode(int statusCode) { _statusCode = statusCode; }

    private:
        int _statusCode;
    };

    Q_DECLARE_METATYPE(ParseReply);

    //
    // ParseReply
    //
    class CGPARSE_API ParseUserReply : public ParseReply
    {
    public:
        ParseUserReply();
        ParseUserReply(const ParseUserReply &reply);
        ~ParseUserReply();

        ParseUserPtr user() const { return _pUser; }
        void setUser(ParseUserPtr pUser) { _pUser = pUser; }

    private:
        ParseUserPtr _pUser;
    };

    Q_DECLARE_METATYPE(ParseUserReply);

    //
    // ParseFileReply
    //
    class CGPARSE_API ParseFileReply: public ParseReply
    {
    public:
        ParseFileReply();
        ParseFileReply(const ParseFileReply &reply);
        ~ParseFileReply();

        ParseFilePtr file() const { return _pFile; }
        void setFile(ParseFilePtr pFile) { _pFile = pFile; }

    private:
        ParseFilePtr _pFile;
    };

    Q_DECLARE_METATYPE(ParseFileReply);

    //
    // ParseJsonObjectReply
    //
    class CGPARSE_API ParseJsonArrayReply : public ParseReply
    {
    public:
        ParseJsonArrayReply();
        ParseJsonArrayReply(const ParseJsonArrayReply &reply);
        ~ParseJsonArrayReply();

        QJsonArray jsonArray() const { return _jsonArray; }
        void setJsonArray(const QJsonArray &jsonArray) { _jsonArray = jsonArray; }

    private:
        QJsonArray _jsonArray;
    };

    Q_DECLARE_METATYPE(ParseJsonArrayReply);

    //
    // ParseCountReply
    //
    class CGPARSE_API ParseCountReply : public ParseReply
    {
    public:
        ParseCountReply();
        ParseCountReply(const ParseCountReply &reply);
        ~ParseCountReply();

        int count() const { return _count; }
        void setCount(int count) { _count = count; }

    private:
        int _count;
    };

    Q_DECLARE_METATYPE(ParseCountReply);
}

#endif // CGPARSE_PARSEREPLY_H
