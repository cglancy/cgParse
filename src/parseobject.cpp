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

    ParseObject::ParseObject(const QString &className)
        : _className(className),
        _pHelper(new ParseObjectHelper())
    {
    }

    ParseObject::~ParseObject()
    {
    }

    ParseObjectPtr ParseObject::create(const QString &className)
    {
        return QSharedPointer<ParseObject>::create(className);
    }

    ParseObjectPtr ParseObject::createWithoutData(const QString &className, const QString &objectId)
    {
        ParseObjectPtr pObject = create(className);
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

    bool ParseObject::hasSameId(ParseObjectPtr pObject) const
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
        map.insert("__op", "Delete");
        setValue(key, map);
    }

    void ParseObject::add(const QString & key, const QVariant & value)
    {
        QVariantList list;
        list.append(value);
        QVariantMap map;
        map.insert("__op", "Add");
        map.insert("objects", list);
        setValue(key, map);
    }

    void ParseObject::addUnique(const QString & key, const QVariant & value)
    {
        QVariantList list;
        list.append(value);
        QVariantMap map;
        map.insert("__op", "AddUnique");
        map.insert("objects", list);
        setValue(key, map);
    }

    void ParseObject::addAll(const QString & key, const QVariantList & valueList)
    {
        QVariantMap map;
        map.insert("__op", "Add");
        map.insert("objects", valueList);
        setValue(key, map);
    }

    void ParseObject::addAllUnique(const QString & key, const QVariantList & valueList)
    {
        QVariantMap map;
        map.insert("__op", "AddUnique");
        map.insert("objects", valueList);
        setValue(key, map);
    }

    void ParseObject::removeAll(const QString & key, const QVariantList & valueList)
    {
        QVariantMap map;
        map.insert("__op", "Remove");
        map.insert("objects", valueList);
        setValue(key, map);
    }

    ParseFilePtr ParseObject::file(const QString &key) const
    {
        ParseFilePtr pFile;
        if (_valueMap.contains(key) && ParseFile::isFile(_valueMap.value(key)))
        {
            QVariantMap map = value(key).toMap();
            pFile = QSharedPointer<ParseFile>::create();
            pFile->setValues(map);
        }

        return pFile;
    }

    void ParseObject::setFile(const QString &key, ParseFilePtr pFile)
    {
        setValue(key, pFile->toMap());
    }

    ParseUserPtr ParseObject::user(const QString & key) const
    {
        ParseUserPtr pUser;

        if (_valueMap.contains(key) && ParseObjectPointer::isPointer(_valueMap.value(key)))
        {
            ParseObjectPointer pointer(_valueMap.value(key));
            pUser = ParseObject::createWithoutData<ParseUser>(pointer.objectId());
        }

        return pUser;
    }

    void ParseObject::setUser(const QString & key, ParseUserPtr pUser)
    {
        setValue(key, pUser->toPointer().toMap());
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
        return key != Parse::ObjectIdKey &&
            key != Parse::CreatedAtKey &&
            key != Parse::UpdatedAtKey;
    }

    ParseObjectPointer ParseObject::toPointer() const
    {
        return ParseObjectPointer(_className, objectId());
    }

    QJsonObject ParseObject::toJsonObject(bool onlyUserValues) const
    {
        QJsonObject jsonObject = QJsonObject::fromVariantMap(_valueMap);

        if (onlyUserValues)
        {
            jsonObject.remove(Parse::ObjectIdKey);
            jsonObject.remove(Parse::CreatedAtKey);
            jsonObject.remove(Parse::UpdatedAtKey);
        }

        return jsonObject;
    }

    QVariantMap ParseObject::toMap(bool onlyUserValues) const
    {
        QVariantMap map = _valueMap;
        if (onlyUserValues)
        {
            map.remove(Parse::ObjectIdKey);
            map.remove(Parse::CreatedAtKey);
            map.remove(Parse::UpdatedAtKey);
        }

        return map;
    }

    void ParseObject::setValues(const QJsonObject &jsonObject)
    {
        QVariantMap variantMap = jsonObject.toVariantMap();
        setValues(variantMap);
    }

    void ParseObject::setValues(const QVariantMap &variantMap)
    {
        for (auto & key : variantMap.keys())
            _valueMap.insert(key, variantMap.value(key));
    }

    ParseReply* ParseObject::save()
    {
        ParseReply *pReply = nullptr;

        if (createdAt().isNull())
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

    ParseReply* ParseObject::createAll(const QList<ParseObjectPtr>& objects)
    {
        return staticHelper()->createAll(objects);
    }

    ParseReply* ParseObject::updateAll(const QList<ParseObjectPtr>& objects)
    {
        return staticHelper()->updateAll(objects);
    }

    ParseReply* ParseObject::deleteAll(const QList<ParseObjectPtr>& objects)
    {
        return staticHelper()->deleteAll(objects);
    }
}
