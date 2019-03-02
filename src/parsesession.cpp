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
#include "parsesession.h"
#include "parsesessionhelper.h"
#include "parserequest.h"
#include "parseuser.h"
#include "parsereply.h"

namespace cg
{
    ParseSessionHelper * ParseSession::_pStaticHelper = nullptr;

    // static
    ParseSessionHelper * ParseSession::staticHelper()
    {
        if (!_pStaticHelper)
            _pStaticHelper = new ParseSessionHelper();

        return _pStaticHelper;
    }

    // static
    ParseSessionPtr ParseSession::create()
    {
        return QSharedPointer<ParseSession>::create();
    }

    // static
    QSharedPointer<ParseQuery<ParseSession>> ParseSession::query()
    {
        return QSharedPointer<ParseQuery<ParseSession>>::create();
    }

    // static
    ParseReply* ParseSession::deleteSession(const QString &sessionToken)
    {
        ParseRequest request(ParseRequest::PostHttpMethod, "/parse/logout");
        request.setHeader("X-Parse-Session-Token", sessionToken.toUtf8());
        return new ParseReply(request.sendRequest());
    }

    // static
    ParseReply* ParseSession::currentSession()
    {
        ParseUserPtr pCurrentUser = ParseUser::currentUser();
        if (pCurrentUser.isNull())
            return new ParseReply(ParseError::UnknownError);

        ParseRequest request(ParseRequest::GetHttpMethod, "parse/sessions/me");
        request.setHeader("X-Parse-Session-Token", pCurrentUser->sessionToken().toUtf8());
        return new ParseReply(request.sendRequest());
    }

    ParseSession::ParseSession()
        : ParseObject("_Session")
    {
    }

    ParseSession::~ParseSession()
    {
    }

    QString ParseSession::sessionToken() const
    {
        return value("sessionToken").toString();
    }
}