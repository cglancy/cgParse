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
#include <asyncfuture.h>

#include <QJsonObject>
#include <QJsonArray>

namespace cg {
    const QString ParseObject::ClassNameKey = QStringLiteral("className");
    const QString ParseObject::ObjectIdKey = QStringLiteral("objectId");
    const QString ParseObject::CreatedAtKey = QStringLiteral("createdAt");
    const QString ParseObject::UpdatedAtKey = QStringLiteral("updatedAt");

    ParseObject::ParseObject()
        : _className("ParseObject"),
        _pHelper(new ParseObjectHelper(sharedFromThis()))
    {
    }

    ParseObject::ParseObject(const QString &className)
        : _className(className),
        _pHelper(new ParseObjectHelper(sharedFromThis()))
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
        objectsArray.append(value.toJsonObject());
        QJsonObject jsonObject;
        jsonObject.insert("__op", "Add");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    void ParseObject::addUnique(const QString & key, const QVariant & value)
    {
        QJsonArray objectsArray;
        objectsArray.append(value.toJsonObject());
        QJsonObject jsonObject;
        jsonObject.insert("__op", "AddUnique");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    void ParseObject::addAll(const QString & key, const QVariantList & valueList)
    {
        QJsonArray objectsArray;
        for (auto & value : valueList)
            objectsArray.append(value.toJsonObject());

        QJsonObject jsonObject;
        jsonObject.insert("__op", "Add");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    void ParseObject::addAllUnique(const QString & key, const QVariantList & valueList)
    {
        QJsonArray objectsArray;
        for (auto & value : valueList)
            objectsArray.append(value.toJsonObject());

        QJsonObject jsonObject;
        jsonObject.insert("__op", "AddUnique");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    void ParseObject::removeAll(const QString & key, const QVariantList & valueList)
    {
        QJsonArray objectsArray;
        for (auto & value : valueList)
            objectsArray.append(value.toJsonObject());

        QJsonObject jsonObject;
        jsonObject.insert("__op", "Remove");
        jsonObject.insert("objects", objectsArray);
        setValue(key, jsonObject);
    }

    ParseFilePtr ParseObject::file(const QString &key) const
    {
        ParseFilePtr pFile = QSharedPointer<ParseFile>::create();
        pFile->setValues(value(key).toJsonObject());
        return pFile;
    }

    void ParseObject::setFile(const QString &key, ParseFilePtr pFile)
    {
        setValue(key, pFile->toJsonObject());
    }

    ParseUserPtr ParseObject::user(const QString & key) const
    {
        ParseUserPtr pUser = QSharedPointer<ParseUser>::create();
        pUser->setValues(value(key).toJsonObject());
        return pUser;
    }

    void ParseObject::setUser(const QString & key, ParseUserPtr pUser)
    {
        setValue(key, pUser->toPointer().toJsonObject());
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

    //QVariantMap ParseObject::valueMap(bool onlyUserValues) const
    //{
    //    QVariantMap map;
    //    QStringList list = keys(onlyUserValues);
    //    for (auto & key : list)
    //        map.insert(key, value(key));

    //    return map;
    //}

    ParseObjectPointer ParseObject::toPointer() const
    {
        return ParseObjectPointer(_className, objectId());
    }

    QJsonObject ParseObject::toJsonObject(bool onlyUserValues) const
    {
        QJsonObject jsonObject = QJsonObject::fromVariantMap(_valueMap);

        if (onlyUserValues)
        {
            jsonObject.remove(ObjectIdKey);
            jsonObject.remove(CreatedAtKey);
            jsonObject.remove(UpdatedAtKey);
        }

        return jsonObject;
    }

    void ParseObject::setValues(const QJsonObject &jsonObject)
    {
        QVariantMap newMap = jsonObject.toVariantMap();
        for (auto & key : newMap.keys())
            _valueMap.insert(key, newMap.value(key));
    }

    QFuture<int> ParseObject::save()
    {
        QFuture<int> future;
        if (createdAt().isNull())
        {
            _pHelper->createObject(sharedFromThis());
            future = AsyncFuture::observe(_pHelper.data(), &ParseObjectHelper::createObjectFinished).future();
        }
        else
        {
            _pHelper->updateObject(sharedFromThis());
            future = AsyncFuture::observe(_pHelper.data(), &ParseObjectHelper::updateObjectFinished).future();
        }
        
        return future;
    }

    QFuture<int> ParseObject::fetch()
    {
        _pHelper->fetchObject(sharedFromThis());
        return AsyncFuture::observe(_pHelper.data(), &ParseObjectHelper::fetchObjectFinished).future();
    }

    QFuture<int> ParseObject::deleteObject()
    {
        _pHelper->deleteObject(sharedFromThis());
        return AsyncFuture::observe(_pHelper.data(), &ParseObjectHelper::deleteObjectFinished).future();
    }
}
