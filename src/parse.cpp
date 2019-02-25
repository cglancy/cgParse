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
#include "parse.h"

namespace cg
{
    namespace Parse
    {
        CGPARSE_API const QString ClassNameKey = QStringLiteral("className");
        CGPARSE_API const QString ObjectIdKey = QStringLiteral("objectId");
        CGPARSE_API const QString CreatedAtKey = QStringLiteral("createdAt");
        CGPARSE_API const QString UpdatedAtKey = QStringLiteral("updatedAt");
        CGPARSE_API const QString TypeKey = QStringLiteral("__type");
        CGPARSE_API const QString ACLKey = QStringLiteral("ACL");
        CGPARSE_API const QString PublicAccessKey = QStringLiteral("*");
        CGPARSE_API const QString ReadKey = QStringLiteral("read");
        CGPARSE_API const QString WriteKey = QStringLiteral("write");
        CGPARSE_API const QString IsoDateKey = QStringLiteral("iso");
        CGPARSE_API const QString LatitudeKey = QStringLiteral("latitude");
        CGPARSE_API const QString LongitudeKey = QStringLiteral("longitude");

        CGPARSE_API const QString PointerValue = QStringLiteral("Pointer");
        CGPARSE_API const QString ObjectValue = QStringLiteral("Object");
        CGPARSE_API const QString FileValue = QStringLiteral("File");
        CGPARSE_API const QString DateValue = QStringLiteral("Date");
        CGPARSE_API const QString GeoPointValue = QStringLiteral("GeoPoint");
    }
}