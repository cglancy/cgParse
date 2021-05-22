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
#include "parseuser.h"
#include "parserelation.h"
#include "parseobjecthelper.h"
#include "parsedatetime.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QMetaType>
#include <QDebug>

namespace cg {
    ParseObjectHelper * ParseObject::_pStaticHelper = nullptr;

    ParseObjectHelper * ParseObject::staticHelper()
    {
        if (!_pStaticHelper)
            _pStaticHelper = new ParseObjectHelper();

        return _pStaticHelper;
    }

    ParseObject::ParseObject()
        : _className("ParseObject"),
        _pHelper(new ParseObjectHelper())
    {
    }

    ParseObject::ParseObject(const ParseObject & object)
    {
        _className = object._className;
        _valueMap = object._valueMap; 
        _savedValueMap = object._savedValueMap;
    }

    ParseObject::ParseObject(const QString &className)
        : _className(className),
        _pHelper(new ParseObjectHelper())
    {
    }

    ParseObject::~ParseObject()
    {
    }

    QSharedPointer<ParseObject> ParseObject::create(const QString &className)
    {
        QSharedPointer<ParseObject> pObject;

        int id = QMetaType::type(className.toLocal8Bit());
        if (id != QMetaType::UnknownType)
        {
            void *newObject = QMetaType::create(id);
            ParseObject *pParseObject = static_cast<ParseObject*>(newObject);
            pObject = QSharedPointer<ParseObject>(pParseObject);
        }
        else
        {
            pObject = QSharedPointer<ParseObject>::create(className);
        }

        return pObject;
    }

    QSharedPointer<ParseObject> ParseObject::createWithoutData(const QString &className, const QString &objectId)
    {
        QSharedPointer<ParseObject> pObject = create(className);
        if (pObject)
            pObject->setValue(Parse::ObjectIdKey, objectId);
        return pObject;
    }

    bool ParseObject::isDirty() const
    {
        for (auto & key : keys())
        {
            if (isDirty(key))
                return true;
        }

        return false;
    }

    bool ParseObject::isDirty(const QString &key) const
    {
        bool dirty = true;

        bool inValueMap = _valueMap.contains(key);
        bool inSavedMap = _savedValueMap.contains(key);

        if (inValueMap && inSavedMap)
        {
            if (value(key) == _savedValueMap.value(key))
                dirty = false;
        }
        else if (!inValueMap && !inSavedMap)
        {
            dirty = false;
        }

        return dirty;
    }

    void ParseObject::revert()
    {
        _valueMap = _savedValueMap;
    }

    void ParseObject::revert(const QString &key)
    {
        _valueMap.insert(key, _savedValueMap.value(key));
    }

    void ParseObject::clearDirtyState()
    {
        _savedValueMap = _valueMap;
    }

    bool ParseObject::hasSameId(QSharedPointer<ParseObject> pObject) const
    {
        return pObject && pObject->value(Parse::ObjectIdKey) == value(Parse::ObjectIdKey);
    }

    QString ParseObject::className() const
    {
        return _className;
    }

    QString ParseObject::objectId() const
    {
        return value(Parse::ObjectIdKey).toString();
    }

    QDateTime ParseObject::createdAt() const
    {
        return value(Parse::CreatedAtKey).toDateTime();
    }

    QDateTime ParseObject::updatedAt() const
    {
        return value(Parse::UpdatedAtKey).toDateTime();
    }

    QVariant ParseObject::value(const QString &key) const
    {
        return _valueMap.value(key);
    }

    void ParseObject::setValue(const QString &key, const QVariant &variant)
    {
        _valueMap.insert(key, variant);
    }

    void ParseObject::remove(const QString & key)
    {
        QVariantMap map;
        map.insert(Parse::OperatorKey, Parse::DeleteValue);
        setValue(key, map);
    }

    void ParseObject::add(const QString & key, const QVariant & value)
    {
        QVariantList list;
        list.append(value);
        QVariantMap map;
        map.insert(Parse::OperatorKey, Parse::AddValue);
        map.insert("objects", list);
        setValue(key, map);
    }

    void ParseObject::addUnique(const QString & key, const QVariant & value)
    {
        QVariantList list;
        list.append(value);
        QVariantMap map;
        map.insert(Parse::OperatorKey, Parse::AddUniqueValue);
        map.insert("objects", list);
        setValue(key, map);
    }

    void ParseObject::addAll(const QString & key, const QVariantList & valueList)
    {
        QVariantMap map;
        map.insert(Parse::OperatorKey, Parse::AddValue);
        map.insert("objects", valueList);
        setValue(key, map);
    }

    void ParseObject::addAllUnique(const QString & key, const QVariantList & valueList)
    {
        QVariantMap map;
        map.insert(Parse::OperatorKey, Parse::AddUniqueValue);
        map.insert("objects", valueList);
        setValue(key, map);
    }

    void ParseObject::remove(const QString& key, const QVariant& value)
    {
        QVariantList valueList;
        valueList.append(value);
        removeAll(key, valueList);
    }

    void ParseObject::removeAll(const QString & key, const QVariantList & valueList)
    {
        QVariantMap map;
        map.insert(Parse::OperatorKey, Parse::RemoveValue);
        map.insert("objects", valueList);
        setValue(key, map);
    }

    QSharedPointer<ParseFile> ParseObject::file(const QString &key) const
    {
        return _valueMap.value(key).value<QSharedPointer<ParseFile>>();
    }

    void ParseObject::setFile(const QString &key, QSharedPointer<ParseFile> pFile)
    {
        setValue(key, QVariant::fromValue(pFile));
    }

    QSharedPointer<ParseUser> ParseObject::user(const QString & key) const
    {
        QSharedPointer<ParseObject> pBaseObject = value(key).value<QSharedPointer<ParseObject>>();
        return pBaseObject.staticCast<ParseUser>();
    }

    void ParseObject::setUser(const QString & key, QSharedPointer<ParseUser> pUser)
    {
        QSharedPointer<ParseObject> pBaseObject = pUser.staticCast<ParseObject>();
        setValue(key, QVariant::fromValue(pBaseObject));
    }

    QDateTime ParseObject::dateTime(const QString & key) const
    {
        return ParseDateTime::toDateTime(value(key));
    }

    void ParseObject::setDateTime(const QString & key, const QDateTime & dateTime)
    {
        setValue(key, ParseDateTime(dateTime));
    }

    QDate ParseObject::date(const QString & key) const
    {
        return ParseDateTime::toDate(value(key));
    }

    void ParseObject::setDate(const QString & key, const QDate & date)
    {
        setValue(key, ParseDateTime(date));
    }

    ParseGeoPoint ParseObject::geoPoint(const QString & key) const
    {
        return ParseGeoPoint(value(key));
    }

    void ParseObject::setGeoPoint(const QString & key, const ParseGeoPoint & geoPoint)
    {
        setValue(key, geoPoint);
    }

    bool ParseObject::contains(const QString &key) const
    {
        QStringList list = keys();
        return list.contains(key);
    }

    QStringList ParseObject::keys() const
    {
        return _valueMap.keys();
    }

    bool ParseObject::isUserValue(const QString &key)
    {
        return key != Parse::ObjectIdKey &&
            key != Parse::CreatedAtKey &&
            key != Parse::UpdatedAtKey &&
            !key.startsWith('_');
    }

    ParseObjectPointer ParseObject::toPointer() const
    {
        return ParseObjectPointer(_className, objectId());
    }

    QVariantMap ParseObject::toMap() const
    {
        return _valueMap;
    }

    void ParseObject::setValues(const QVariantMap &variantMap)
    {
        for (auto& key : variantMap.keys())
        {
            if (key != Parse::TypeKey)
                _valueMap.insert(key, variantMap.value(key));
        }
    }

    ParseReply* ParseObject::save()
    {
        ParseReply *pReply = nullptr;

        if (objectId().isEmpty())
        {
            pReply = _pHelper->createObject(sharedFromThis());
        }
        else
        {
            pReply = _pHelper->updateObject(sharedFromThis());
        }
        
        return pReply;
    }

    ParseReply* ParseObject::fetch()
    {
        return _pHelper->fetchObject(sharedFromThis());
    }

    ParseReply* ParseObject::deleteObject()
    {
        return _pHelper->deleteObject(sharedFromThis());
    }

    ParseReply* ParseObject::saveAll(const QList<QSharedPointer<ParseObject>> &objects)
    {
        return staticHelper()->saveAll(objects);
    }

    ParseReply* ParseObject::deleteAll(const QList<QSharedPointer<ParseObject>>& objects)
    {
        return staticHelper()->deleteAll(objects);
    }
}
