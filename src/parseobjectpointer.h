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
#ifndef CGPARSE_PARSEOBJECTPOINTER_H
#define CGPARSE_PARSEOBJECTPOINTER_H
#pragma once

#include "parse.h"
#include <QString>
#include <QJsonObject>
#include <QVariant>
#include <QHash>

namespace cg
{
    class CGPARSE_API ParseObjectPointer
    {
    public:
        ParseObjectPointer();
        ParseObjectPointer(const QString &className, const QString &objectId);
        ParseObjectPointer(const QJsonObject &jsonObject);
        ParseObjectPointer(const QVariant &variant);
        ParseObjectPointer(const ParseObjectPointer &ptr);

        bool isNull() const;
        QString className() const;
        QString objectId() const;

        ParseObjectPointer & operator=(const ParseObjectPointer &ptr);
        bool operator==(const ParseObjectPointer &ptr) const;
        bool operator<(const ParseObjectPointer &ptr) const;

        QJsonObject toJsonObject() const;
        QVariantMap toMap() const;

        static bool isPointer(const QVariant &variant);
        static bool isPointer(const QJsonValue &jsonValue);
        static bool isObject(const QVariant &variant);
        static bool isObject(const QJsonValue &variant);

    private:
        QString _className, _objectId;
    };

    inline bool operator==(const ParseObjectPointer &p1, const ParseObjectPointer &p2)
    {
        return p1.className() == p2.className() && p1.objectId() == p2.objectId();
    }

    inline uint qHash(const ParseObjectPointer &p, uint seed)
    {
        return qHash(p.className() + p.objectId(), seed);
    }
}

#endif // CGPARSE_PARSEOBJECTPOINTER_H