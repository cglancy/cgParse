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
#include "parseutil.h"

#include <QJsonObject>
#include <QJsonArray>

namespace cg {
    const QString ParseObject::ObjectIdKey = QStringLiteral("objectId");
    const QString ParseObject::CreatedAtKey = QStringLiteral("createdAt");
    const QString ParseObject::UpdatedAtKey = QStringLiteral("updatedAt");

    ParseObject::ParseObject()
        : _className("ParseObject")
    {
    }

    ParseObject::ParseObject(const QString &className)
        : _className(className)
    {
    }

    ParseObject::~ParseObject()
    {
    }

    ParseObjectPtr ParseObject::create(const QString &className)
    {
        ParseObjectPtr pObject = QSharedPointer<ParseObject>::create(className);
    }

    ParseObjectPtr ParseObject::createWithoutData(const QString &className, const QString &objectId)
    {
        ParseObjectPtr pObject = create(className);
        if (pObject)
            pObject->setValue(ObjectIdKey, objectId);
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

    bool ParseObject::hasSameId(ParseObjectPtr pObject) const
    {
        return pObject && pObject->value(ObjectIdKey) == value(ObjectIdKey);
    }

    QString ParseObject::className() const
    {
        return _className;
    }

    QString ParseObject::objectId() const
    {
        return value(ObjectIdKey).toString();
    }

    QDateTime ParseObject::createdAt() const
    {
        return value(CreatedAtKey).toDateTime();
    }

    QDateTime ParseObject::updatedAt() const
    {
        return value(UpdatedAtKey).toDateTime();
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
        QJsonObject jsonObject;
        jsonObject.insert("__op", "Delete");
        setValue(key, jsonObject);
    }

    void ParseObject::add(const QString & key, const QVariant & value)
    {
        QJsonArray objectsArray;
        objectsArray.append(ParseUtil::toJsonValue(value));
        QJsonObject jsonObject;
        jsonObject.insert("__op", "Add");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    void ParseObject::addUnique(const QString & key, const QVariant & value)
    {
        QJsonArray objectsArray;
        objectsArray.append(ParseUtil::toJsonValue(value));
        QJsonObject jsonObject;
        jsonObject.insert("__op", "AddUnique");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    void ParseObject::addAll(const QString & key, const QVariantList & valueList)
    {
        QJsonArray objectsArray;
        for (auto & value : valueList)
            objectsArray.append(ParseUtil::toJsonValue(value));

        QJsonObject jsonObject;
        jsonObject.insert("__op", "Add");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    void ParseObject::addAllUnique(const QString & key, const QVariantList & valueList)
    {
        QJsonArray objectsArray;
        for (auto & value : valueList)
            objectsArray.append(ParseUtil::toJsonValue(value));

        QJsonObject jsonObject;
        jsonObject.insert("__op", "AddUnique");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    void ParseObject::removeAll(const QString & key, const QVariantList & valueList)
    {
        QJsonArray objectsArray;
        for (auto & value : valueList)
            objectsArray.append(ParseUtil::toJsonValue(value));

        QJsonObject jsonObject;
        jsonObject.insert("__op", "Remove");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    ParseObjectPtr ParseObject::objectValue(const QString &key) const
    {
        return value(key).value<ParseObjectPtr>();
    }

    void ParseObject::setObject(const QString &key, ParseObjectPtr pObject)
    {
        setValue(key, QVariant::fromValue(pObject));
    }

    ParseFile* ParseObject::file(const QString &key) const
    {
        return qvariant_cast<ParseFile*>(value(key));
    }

    void ParseObject::setFile(const QString &key, ParseFile *pFile)
    {
        setValue(key, QVariant::fromValue<ParseFile*>(pFile));
    }

    ParseUser * ParseObject::user(const QString & key) const
    {
        return qvariant_cast<ParseUser*>(value(key));
    }

    void ParseObject::setUser(const QString & key, ParseUser * pUser)
    {
        setValue(key, QVariant::fromValue<ParseUser*>(pUser));
    }

    ParseRelationPtr ParseObject::relation(const QString &key)
    {
        ParseRelationPtr pRelation;

        if (_valueMap.contains(key) && _valueMap.value(key).canConvert<ParseRelationPtr>())
        {
            return value(key).value<ParseRelationPtr>();
        }
        else
        {
            pRelation = QSharedPointer<ParseRelation>::create(className(), objectId(), key, this);
            setValue(key, QVariant::fromValue<ParseRelationPtr>(pRelation));
        }

        return pRelation;
    }

    bool ParseObject::contains(const QString &key) const
    {
        QStringList list = keys();
        return list.contains(key);
    }

    QStringList ParseObject::keys(bool onlyUserValues) const
    {
        QStringList list;
        for (auto & key : _valueMap.keys())
        {
            if (isUserValue(key) || !onlyUserValues)
                list.append(key);
        }

        return list;
    }

    bool ParseObject::isUserValue(const QString &key)
    {
        return key != ObjectIdKey && key !=CreatedAtKey && key != UpdatedAtKey;
    }

    QVariantMap ParseObject::valueMap(bool onlyUserValues) const
    {
        QVariantMap map;
        QStringList list = keys(onlyUserValues);
        for (auto & key : list)
            map.insert(key, value(key));

        return map;
    }

    void ParseObject::setValues(const QVariantMap &valueMap)
    {
        for (auto & key : valueMap.keys())
            setValue(key, valueMap.value(key));
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
