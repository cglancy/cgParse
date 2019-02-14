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

namespace cg
{
    //
    // ParseReply
    //
    ParseReply::ParseReply()
        : _statusCode(ParseError::UnknownError)
    {
    }

    ParseReply::ParseReply(const ParseReply &reply)
    {
        _statusCode = reply._statusCode;
    }

    ParseReply::~ParseReply()
    {
    }

    //
    // ParseUserReply
    //
    ParseUserReply::ParseUserReply()
    {
    }

    ParseUserReply::ParseUserReply(const ParseUserReply &reply)
        : ParseReply(reply)
    {
        _pUser = reply._pUser;
    }

    ParseUserReply::~ParseUserReply()
    {
    }

    //
    // ParseFileReply
    //
    ParseFileReply::ParseFileReply()
    {
    }

    ParseFileReply::ParseFileReply(const ParseFileReply &reply)
        : ParseReply(reply)
    {
        _pFile = reply._pFile;
    }

    ParseFileReply::~ParseFileReply()
    {
    }

    //
    // ParseJsonArrayReply
    //
    ParseJsonArrayReply::ParseJsonArrayReply()
    {
    }

    ParseJsonArrayReply::ParseJsonArrayReply(const ParseJsonArrayReply &reply)
        : ParseReply(reply)
    {
        _jsonArray = reply._jsonArray;
    }

    ParseJsonArrayReply::~ParseJsonArrayReply()
    {
    }

    //
    // ParseCountReply
    //
    ParseCountReply::ParseCountReply()
        : _count(0)
    {
    }

    ParseCountReply::ParseCountReply(const ParseCountReply &reply)
        : ParseReply(reply)
    {
        _count = reply._count;
    }

    ParseCountReply::~ParseCountReply()
    {
    }

}