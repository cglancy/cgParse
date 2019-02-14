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
#ifndef CGPARSE_PARSEQUERYHELPER_H
#define CGPARSE_PARSEQUERYHELPER_H
#pragma once

#include "cgparse.h"
#include "parsehelperbase.h"
#include "parsetypes.h"
#include "parsereply.h"

#include <QObject>

class QUrlQuery;

namespace cg
{
    class CGPARSE_API ParseQueryHelper : public QObject, public ParseHelperBase
    {
        Q_OBJECT
    public:
        ParseQueryHelper();
        ~ParseQueryHelper();

    public slots:
        void getObject(const QString &className, const QString &objectId);
        void findObjects(const QString &className, const QUrlQuery &urlQuery);
        void countObjects(const QString &className, const QUrlQuery &urlQuery);

    private slots:
        void privateGetObjectFinished();
        void privateFindObjectsFinished();
        void privateCountObjectsFinished();

    signals:
        void getObjectFinished(ParseJsonArrayReply reply);
        void findObjectsFinished(ParseJsonArrayReply reply);
        void countObjectsFinished(ParseCountReply reply);
    };
}

#endif // CGPARSE_PARSEQUERYHELPER_H
