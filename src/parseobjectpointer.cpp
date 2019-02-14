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
#include "parseobjectpointer.h"
#include "parseobject.h"

namespace cg
{
    ParseObjectPointer::ParseObjectPointer()
    {
    }

    ParseObjectPointer::ParseObjectPointer(const QString &className, const QString &objectId)
        : _className(className),
        _objectId(objectId)
    {
    }

    ParseObjectPointer::ParseObjectPointer(const QVariant &variant)
    {
        if (variant.canConvert<QVariantMap>() && isPointer(variant))
        {
            QVariantMap map = variant.toMap();
            _className = map.value(ParseObject::ClassNameKey).toString();
            _objectId = map.value(ParseObject::ObjectIdKey).toString();
        }
    }

    ParseObjectPointer::ParseObjectPointer(const QJsonObject & jsonObject)
    {
        if (jsonObject.value("__type") == "Pointer")
        {
            _className = jsonObject.value(ParseObject::ClassNameKey).toString();
            _objectId = jsonObject.value(ParseObject::ObjectIdKey).toString();
        }
    }

    ParseObjectPointer::ParseObjectPointer(const ParseObjectPointer & ptr)
    {
        _className = ptr._className;
        _objectId = ptr._objectId;
    }

    bool ParseObjectPointer::isNull() const
    {
        return _className.isEmpty() || _objectId.isEmpty();
    }

    QString ParseObjectPointer::className() const
    {
        return _className;
    }

    QString ParseObjectPointer::objectId() const
    {
        return _objectId;
    }

    ParseObjectPointer & ParseObjectPointer::operator=(const ParseObjectPointer & ptr)
    {
        _className = ptr._className;
        _objectId = ptr._objectId;
        return *this;
    }

    bool ParseObjectPointer::operator==(const ParseObjectPointer & ptr) const
    {
        return _className == ptr._className && _objectId == ptr._objectId;
    }

    bool ParseObjectPointer::operator<(const ParseObjectPointer & ptr) const
    {
        if (_className < ptr._className)
            return true;
        else if (_className == ptr._className && _objectId < ptr._objectId)
            return true;

        return false;
    }

    QJsonObject ParseObjectPointer::toJsonObject() const
    {
        QJsonObject jsonObject;
        jsonObject.insert(ParseObject::ParseTypeKey, "Pointer");
        jsonObject.insert(ParseObject::ClassNameKey, _className);
        jsonObject.insert(ParseObject::ObjectIdKey, _objectId);
        return jsonObject;
    }

    QVariantMap ParseObjectPointer::toMap() const
    {
        QVariantMap map;
        map.insert(ParseObject::ParseTypeKey, "Pointer");
        map.insert(ParseObject::ClassNameKey, _className);
        map.insert(ParseObject::ObjectIdKey, _objectId);
        return map;
    }

    bool ParseObjectPointer::isPointer(const QVariant &variant)
    {
        bool pointer = false;
        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            pointer = map.contains(ParseObject::ParseTypeKey) && 
                map.value(ParseObject::ParseTypeKey).toString() == "Pointer";
        }

        return pointer;
    }

    bool ParseObjectPointer::isPointer(const QJsonValue &jsonValue)
    {
        bool pointer = false;
        if (jsonValue.isObject())
        {
            QJsonObject jsonObject = jsonValue.toObject();
            pointer = jsonObject.contains(ParseObject::ParseTypeKey) &&
                jsonObject.value(ParseObject::ParseTypeKey).toString() == "Pointer";
        }

        return pointer;
    }

    bool ParseObjectPointer::isObject(const QVariant &variant)
    {
        bool object = false;
        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            object = map.contains(ParseObject::ParseTypeKey) &&
                map.value(ParseObject::ParseTypeKey).toString() == "Object";
        }

        return object;
    }

    bool ParseObjectPointer::isObject(const QJsonValue &jsonValue)
    {
        bool object = false;
        if (jsonValue.isObject())
        {
            QJsonObject jsonObject = jsonValue.toObject();
            object = jsonObject.contains(ParseObject::ParseTypeKey) &&
                jsonObject.value(ParseObject::ParseTypeKey).toString() == "Object";
        }

        return object;
    }
}