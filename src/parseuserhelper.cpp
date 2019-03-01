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
#include "parseuserhelper.h"
#include "parseuser.h"
#include "parserequest.h"
#include "parsereply.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

namespace cg
{
    ParseUserHelper::ParseUserHelper()
    {
    }

    ParseUserHelper::~ParseUserHelper()
    {
    }

    void ParseUserHelper::loginFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        if (!pReply->isError() && pReply->statusCode() == 200)
        {
            ParseUser::_pCurrentUser = pReply->user();
        }
    }

    void ParseUserHelper::logoutFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        if (!pReply->isError())
        {
            ParseUser::_pCurrentUser = nullptr;
        }
    }

    void ParseUserHelper::becomeFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        if (!pReply->isError())
        {
            ParseUser::_pCurrentUser = pReply->user();
        }
    }

    void ParseUserHelper::signUpFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        ParseUserPtr pUser = _pUser.lock();

        if (!pReply->isError() && pReply->statusCode() == 201 && !pUser.isNull())
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isObject())
            {
                QJsonObject obj = doc.object();
                pUser->setValues(obj);
                pUser->clearDirtyState();

                ParseUser::_pCurrentUser = pUser;
            }
        }
    }

    void ParseUserHelper::deleteUserFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        ParseUserPtr pUser = _pUser.lock();

        if (!pReply->isError() && !pUser.isNull())
        {
            if (pUser->hasSameId(ParseUser::_pCurrentUser))
                ParseUser::_pCurrentUser = nullptr;
        }
    }
}