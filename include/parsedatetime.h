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
#ifndef CGPARSE_PARSEDATETIME_H
#define CGPARSE_PARSEDATETIME_H
#pragma once

#include "parse.h"
#include <QVariant>
#include <QJsonValue>
#include <QDateTime>

namespace cg
{
    class CGPARSE_API ParseDateTime : public QVariantMap
    {
    public:
        static QDateTime toDateTime(const QVariant &variant);
        static QDate toDate(const QVariant &variant);
        static bool isDateTime(const QVariant &variant);
        static bool isDateTime(const QJsonValue &jsonValue);

    public:
        ParseDateTime();
        ParseDateTime(const ParseDateTime &dateTime);
        ParseDateTime(const QDateTime &dateTime);
        ParseDateTime(const QDate &date);

        ParseDateTime & operator=(const ParseDateTime &dateTime);
        bool operator==(const ParseDateTime &dateTime) const;
        bool operator<(const ParseDateTime &dateTime) const;

        bool isNull() const;

        QDateTime toDateTime() const;
        QDate toDate() const;
    };
}


#endif // CGPARSE_PARSEDATETIME_H