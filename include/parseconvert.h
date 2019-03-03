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
#ifndef CGPARSE_CONVERT_H
#define CGPARSE_CONVERT_H
#pragma once

#include "parse.h"
#include "parsetypes.h"
#include <QVariant>
#include <QJsonObject>

namespace cg
{
    class CGPARSE_API ParseConvert
    {
    public:
        static QJsonObject toJsonObject(const QVariantMap &map);
        static QVariantMap toVariantMap(const QJsonObject &object);

        static bool isPointer(const QVariant &variant);
        static bool isObject(const QVariant &variant);

    private:
        static QVariantMap convertMap(const QVariantMap &map, bool toJson);
        static QVariantList convertList(const QVariantList &list, bool toJson);
        static bool canConvert(const QVariant &variant, bool toJson);
        static QVariant convertVariant(const QVariant &variant, bool toJson);

        static ParseObjectPtr objectFromVariant(const QVariant &variant);

        static bool isFile(const QVariant &variant);
        static ParseFilePtr fileFromVariant(const QVariant &variant);
        static bool isReadOnlyKey(const QString &key);
    };
}

#endif // CGPARSE_CONVERT_H
