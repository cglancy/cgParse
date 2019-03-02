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

#include "parse.h"
#include "parsetypes.h"

#include <QObject>
#include <QJsonArray>

class QUrlQuery;

namespace cg
{
    class ParseReply;

    class CGPARSE_API ParseQueryHelper : public QObject
    {
        Q_OBJECT
    public:
        ParseQueryHelper();
        ~ParseQueryHelper();

        QJsonArray jsonArray;

        ParseReply* getObject(const QString &className, const QString &objectId);
        ParseReply* findObjects(const QString &className, const QUrlQuery &urlQuery);
        ParseReply* countObjects(const QString &className, const QUrlQuery &urlQuery);

    private slots:
        void getObjectFinished();
        void findObjectsFinished();
    };
}

#endif // CGPARSE_PARSEQUERYHELPER_H
