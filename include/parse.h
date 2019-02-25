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
#ifndef CGPARSE_H
#define CGPARSE_H
#pragma once

#include <QtGlobal>

#ifdef Q_OS_WIN32

#ifdef CGPARSE_EXPORTS
#define CGPARSE_API __declspec(dllexport)
#else
#define CGPARSE_API __declspec(dllimport)
#endif

#else

#define CGPARSE_API __attribute__((visibility("default")))

#endif

#include <QString>

namespace cg
{
    namespace Parse
    {
        CGPARSE_API extern const QString ClassNameKey;
        CGPARSE_API extern const QString ObjectIdKey;
        CGPARSE_API extern const QString CreatedAtKey;
        CGPARSE_API extern const QString UpdatedAtKey;
        CGPARSE_API extern const QString TypeKey;
        CGPARSE_API extern const QString ACLKey;
        CGPARSE_API extern const QString PublicAccessKey;
        CGPARSE_API extern const QString ReadKey;
        CGPARSE_API extern const QString WriteKey;
        CGPARSE_API extern const QString IsoDateKey;
        CGPARSE_API extern const QString LatitudeKey;
        CGPARSE_API extern const QString LongitudeKey;

        CGPARSE_API extern const QString PointerValue;
        CGPARSE_API extern const QString ObjectValue;
        CGPARSE_API extern const QString FileValue;
        CGPARSE_API extern const QString DateValue;
        CGPARSE_API extern const QString GeoPointValue;
    }
}

#endif // CGPARSE_H
