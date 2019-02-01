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
#include "parsequery.h"
#include "parseclient.h"
#include "parseobject.h"

namespace cg
{
    ParseQuery::ParseQuery(const QString &className)
        : QObject(ParseClient::instance())
    {
        _className = className;
    }

    ParseQuery::~ParseQuery()
    {
    }

    void ParseQuery::get(const QString &objectId)
    {
        ParseClient::instance()->getObject(this, objectId);
    }

    void ParseQuery::find()
    {
        QStringList parameters;
        ParseClient::instance()->findObjects(this);
    }

    void ParseQuery::count()
    {
        QStringList parameters;
        ParseClient::instance()->countObjects(this);
    }

    QUrlQuery ParseQuery::urlQuery() const
    {
        return QUrlQuery();
    }
}

