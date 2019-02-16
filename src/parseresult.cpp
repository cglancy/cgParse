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

namespace cg
{
    //
    // ParseResult
    //
    ParseResult::ParseResult()
        : _statusCode(ParseError::UnknownError)
    {
    }

    ParseResult::ParseResult(const ParseResult &result)
    {
        _statusCode = result._statusCode;
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

    ParseUserResult::ParseUserResult(const ParseUserResult &result)
        : ParseResult(result)
    {
        _pUser = result._pUser;
    }

    ParseUserResult::~ParseUserResult()
    {
    }

    //
    // ParseFileResult
    //
    ParseFileResult::ParseFileResult()
    {
    }

    ParseFileResult::ParseFileResult(const ParseFileResult &result)
        : ParseResult(result)
    {
        _pFile = result._pFile;
    }

    ParseFileResult::~ParseFileResult()
    {
    }

    //
    // ParseObjectsResult
    //
    ParseObjectsResult::ParseObjectsResult()
    {
    }

    ParseObjectsResult::ParseObjectsResult(const ParseObjectsResult &result)
        : ParseResult(result)
    {
        _jsonArray = result._jsonArray;
    }

    ParseObjectsResult::~ParseObjectsResult()
    {
    }

    //
    // ParseCountResult
    //
    ParseCountResult::ParseCountResult()
        : _count(0)
    {
    }

    ParseCountResult::ParseCountResult(const ParseCountResult &result)
        : ParseResult(result)
    {
        _count = result._count;
    }

    ParseCountResult::~ParseCountResult()
    {
    }

    //
    // ParseSessionResult
    //    
    ParseSessionResult::ParseSessionResult()
    {
    }

    ParseSessionResult::ParseSessionResult(const ParseSessionResult &result)
        : ParseResult(result)
    {
        _pSession = result._pSession;
    }

    ParseSessionResult::~ParseSessionResult()
    {
    }

}