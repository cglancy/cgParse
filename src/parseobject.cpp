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
#include "parseobject.h"
#include "parseclient.h"
#include "parsefile.h"

#include <QMetaProperty>

namespace cg {
    const QString ParseObject::ObjectIdName = QStringLiteral("objectId");
    const QString ParseObject::CreatedAtName = QStringLiteral("createdAt");
    const QString ParseObject::UpdatedAtName = QStringLiteral("updatedAt");

    ParseObject::ParseObject(const QString &className)
        : QObject(ParseClient::instance()),
        _className(className)
    {
    }

    ParseObject::~ParseObject()
    {
    }

    ParseObject * ParseObject::clone() const
    {
        ParseObject *pClone = new ParseObject(_className);
        pClone->setValues(valueMap(false));
        return pClone;
    }

    bool ParseObject::isDirty() const
    {
        for (auto & name : valueNames())
        {
            if (isDirty(name))
                return true;
        }

        return false;
    }

    bool ParseObject::isDirty(const QString &name) const
    {
        if (_cachedValueMap.contains(name))
        {
            if (value(name) == _cachedValueMap.value(name))
                return false;
        }

        return true;
    }
    
    void ParseObject::setDirty(bool dirty)
    {
        _cachedValueMap.clear();

        if (!dirty)
        {
            for (auto & name : valueNames())
                _cachedValueMap.insert(name, value(name));
        }
    }

    bool ParseObject::hasSameId(ParseObject *pObject) const
    {
        return pObject && pObject->value(ObjectIdName) == value(ObjectIdName);
    }

    QString ParseObject::className() const
    {
        return _className;
    }

    QString ParseObject::objectId() const
    {
        return value(ObjectIdName).toString();
    }

    QDateTime ParseObject::createdAt() const
    {
        return value(CreatedAtName).toDateTime();
    }

    QDateTime ParseObject::updatedAt() const
    {
        return value(UpdatedAtName).toDateTime();
    }

    QVariant ParseObject::value(const QString &name) const
    {
        return _valueMap.value(name);
    }

    void ParseObject::setValue(const QString &name, const QVariant &variant)
    {
        _valueMap.insert(name, variant);
        emit valueChanged(name);
    }

    ParseObject * ParseObject::objectValue(const QString &name) const
    {
        return qvariant_cast<ParseObject*>(value(name));
    }

    void ParseObject::setObject(const QString &name, ParseObject *pObject)
    {
        setValue(name, QVariant::fromValue<ParseObject*>(pObject));
    }

    ParseFile* ParseObject::file(const QString &name) const
    {
        return qvariant_cast<ParseFile*>(value(name));
    }

    void ParseObject::setFile(const QString &name, ParseFile *pFile)
    {
        setValue(name, QVariant::fromValue<ParseFile*>(pFile));
    }

    bool ParseObject::hasValue(const QString &name) const
    {
        QStringList names = valueNames();
        return names.contains(name);
    }

    QStringList ParseObject::valueNames(bool onlyUserValues) const
    {
        QStringList names;
        for (auto & name : _valueMap.keys())
        {
            if (!(isUserValue(name) && onlyUserValues))
                names.append(name);
        }

        return _valueMap.keys();
    }

    bool ParseObject::isUserValue(const QString &name)
    {
        return name != ObjectIdName && name !=CreatedAtName && name != UpdatedAtName;
    }

    QVariantMap ParseObject::valueMap(bool onlyUserValues) const
    {
        QVariantMap map;
        QStringList names = valueNames(onlyUserValues);
        for (auto & name : names)
            map.insert(name, value(name));

        return map;
    }

    void ParseObject::setValues(const QVariantMap &valueMap)
    {
        for (auto & name : valueMap.keys())
            setValue(name, valueMap.value(name));
    }

    void ParseObject::save()
    {
        if (createdAt().isNull())
            ParseClient::instance()->createObject(this);
        else
            ParseClient::instance()->updateObject(this);
    }

    void ParseObject::fetch()
    {
        ParseClient::instance()->fetchObject(this);
    }

    void ParseObject::deleteObject()
    {
        ParseClient::instance()->deleteObject(this);
    }
}
