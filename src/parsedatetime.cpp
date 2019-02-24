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
#include "parsedatetime.h"

namespace cg
{
    // static
    QDateTime ParseDateTime::toDateTime(const QVariant & variant)
    {
        QDateTime dateTime;

        if (variant.canConvert<QVariantMap>() && isDateTime(variant))
        {
            QVariantMap map = variant.toMap();
            dateTime = QDateTime::fromString(map.value(Parse::IsoDateKey).toString(), Qt::ISODateWithMs);
        }

        return dateTime;
    }

    ParseDateTime::ParseDateTime()
    {
    }

    ParseDateTime::ParseDateTime(const QDateTime & dateTime)
    {
        insert(Parse::TypeKey, Parse::DateValue);
        insert(Parse::IsoDateKey, dateTime.toString(Qt::ISODateWithMs));
    }

    ParseDateTime::ParseDateTime(const ParseDateTime & dateTime)
        : QVariantMap(dateTime)
    {
    }

    bool ParseDateTime::isNull() const
    {
        return isEmpty();
    }

    ParseDateTime & ParseDateTime::operator=(const ParseDateTime & dateTime)
    {
        if (dateTime.contains(Parse::TypeKey))
            insert(Parse::TypeKey, dateTime.value(Parse::TypeKey));
        if (dateTime.contains(Parse::IsoDateKey))
            insert(Parse::IsoDateKey, dateTime.value(Parse::IsoDateKey));
        return *this;
    }

    bool ParseDateTime::operator==(const ParseDateTime & dateTime) const
    {
        return value(Parse::IsoDateKey) == dateTime.value(Parse::IsoDateKey);
    }

    bool ParseDateTime::operator<(const ParseDateTime & dateTime) const
    {
        return value(Parse::IsoDateKey) < dateTime.value(Parse::IsoDateKey);
    }

    QDateTime ParseDateTime::toDateTime() const
    {
        return QDateTime::fromString(value(Parse::IsoDateKey).toString(), Qt::ISODateWithMs);
    }

    bool ParseDateTime::isDateTime(const QVariant & variant)
    {
        bool dateTime = false;
        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            dateTime = map.contains(Parse::TypeKey) &&
                map.value(Parse::TypeKey).toString() == Parse::DateValue;
        }

        return dateTime;
    }

    bool ParseDateTime::isDateTime(const QJsonValue & jsonValue)
    {
        bool dateTime = false;
        if (jsonValue.isObject())
        {
            QJsonObject jsonObject = jsonValue.toObject();
            dateTime = jsonObject.contains(Parse::TypeKey) &&
                jsonObject.value(Parse::TypeKey).toString() == Parse::DateValue;
        }

        return dateTime;
    }
}