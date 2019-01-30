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

#include <QMetaProperty>

namespace cg {
    const QByteArray ParseObject::ObjectIdPropertyName = "objectId";
    const QByteArray ParseObject::CreatedAtPropertyName = "createdAt";
    const QByteArray ParseObject::UpdatedAtPropertyName = "updatedAt";

    ParseObject::ParseObject(const QString &className)
        : QObject(ParseClient::instance()),
        _className(className)
    {
    }

    ParseObject::~ParseObject()
    {
    }

    bool ParseObject::isNew() const
    {
        return _createdAt.isNull();
    }

    bool ParseObject::isDirty() const
    {
        for (auto & name : propertyNames())
        {
            if (isDirty(name))
                return true;
        }

        return false;
    }

    bool ParseObject::isDirty(const QByteArray &propertyName) const
    {
        if (_fetchedMap.contains(propertyName))
        {
            if (property(propertyName) == _fetchedMap.value(propertyName))
                return false;
        }

        return true;
    }
    
    void ParseObject::setDirty(bool dirty)
    {
        _fetchedMap.clear();

        if (!dirty)
        {
            for (auto & name : propertyNames())
                _fetchedMap.insert(name, property(name));
        }
    }

    bool ParseObject::hasSameId(ParseObject *pObject) const
    {
        return pObject && pObject->_objectId == _objectId;
    }

    QString ParseObject::className() const
    {
        return _className;
    }

    QString ParseObject::objectId() const
    {
        return _objectId;
    }

    void ParseObject::setObjectId(const QString & objectId)
    {
        _objectId = objectId;
        emit propertyChanged(ObjectIdPropertyName);
    }

    QDateTime ParseObject::createdAt() const
    {
        return _createdAt;
    }

    void ParseObject::setCreatedAt(const QDateTime & createdAt)
    {
        _createdAt = createdAt;
        emit propertyChanged(CreatedAtPropertyName);
    }

    QDateTime ParseObject::updatedAt() const
    {
        return _updatedAt;
    }

    void ParseObject::setUpdatedAt(const QDateTime & updatedAt)
    {
        _updatedAt = updatedAt;
        emit propertyChanged(UpdatedAtPropertyName);
    }

    bool ParseObject::hasProperty(const QByteArray &propertyName) const
    {
        QList<QByteArray> names = propertyNames();
        return names.contains(propertyName);
    }

    QList<QByteArray> ParseObject::propertyNames() const
    {
        QList<QByteArray> names = dynamicPropertyNames();

        int count = metaObject()->propertyCount();
        for (int i = 0; i < count; i++)
        {
            QMetaProperty property = metaObject()->property(i);
            QByteArray name = property.name();
            if (name != ObjectIdPropertyName &&
                name != CreatedAtPropertyName &&
                name != UpdatedAtPropertyName &&
                name != "objectName")
            {
                names.append(name);
            }
        }

        return names;
    }

    void ParseObject::save()
    {
        if (_createdAt.isNull())
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
