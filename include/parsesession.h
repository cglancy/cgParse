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
#ifndef CGPARSE_PARSESESSION_H
#define CGPARSE_PARSESESSION_H
#pragma once

#include "parseobject.h"
#include "parsequery.h"

class QNetworkAccessManager;

namespace cg
{
    class ParseSessionHelper;
    class ParseReply;

    class CGPARSE_API ParseSession : public ParseObject
    {
    public:
        static ParseSession create();
        static ParseQuery<ParseSession> query();

        static ParseReply* currentSession(QNetworkAccessManager* pNam = nullptr);
        static ParseReply* deleteSession(const QString &sessionToken, QNetworkAccessManager* pNam = nullptr);

    public:
        ParseSession();
        ParseSession(const ParseSession& session);
        ParseSession(const ParseObject& object);
        ~ParseSession();

        void assign(const ParseObject& object) override;
        ParseSession& operator=(const ParseSession& user);

        QString sessionToken() const;
    };
}

Q_DECLARE_METATYPE(cg::ParseSession);
#endif // CGPARSE_PARSESESSION_H
