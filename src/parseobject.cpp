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
#include "parseobjectimpl.h"
#include "parseclient.h"
#include "parsefile.h"
#include "parseuser.h"
#include "parserelation.h"
#include "parseobjectrequest.h"
#include "parsedatetime.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QMetaType>
#include <QDebug>

namespace cg 
{
    bool operator==(const ParseObject& object1, const ParseObject& object2)
    {
        return object1.objectId() == object2.objectId();
    }

    bool operator<(const ParseObject& object1, const ParseObject& object2)
    {
        return object1.objectId() < object2.objectId();
    }

    uint qHash(const ParseObject& object, uint seed)
    {
        return qHash(object.objectId(), seed);
    }

    ParseObject::ParseObject()
    {
        // constructs a null object
    }

    ParseObject::ParseObject(const ParseObject & object)
    {
        _pImpl = object._pImpl;
    }

    ParseObject::ParseObject(const QString &className)
    {
        _pImpl = QSharedPointer<ParseObjectImpl>::create(className);
    }

    ParseObject::~ParseObject()
    {
    }

    bool ParseObject::isNull() const
    {
        return nullptr == _pImpl;
    }

    void ParseObject::assign(const ParseObject& object)
    {
        _pImpl = object._pImpl;
    }

    void ParseObject::nullify()
    {
        _pImpl.reset();
    }

    ParseObject& ParseObject::operator=(const ParseObject& object)
    {
        assign(object);
        return *this;
    }

    bool ParseObject::operator==(const ParseObject& object)
    {
        return hasSameId(object);
    }

    QString ParseObject::removeNamespace(const QString& className)
    {
        return className.section("::", -1);
    }

    ParseObject ParseObject::create(const QString &className)
    {
        ParseObject object(className);
        return object;
    }

    ParseObject ParseObject::createWithoutData(const QString &className, const QString &objectId)
    {
        ParseObject object(className);
        object.setValue(Parse::ObjectIdKey, objectId);
        return object;
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
        if (!_pImpl)
            return false;

        bool dirty = true;

        bool inValueMap = _pImpl->valueMap.contains(key);
        bool inSavedMap = _pImpl->savedValueMap.contains(key);

        if (inValueMap && inSavedMap)
        {
            if (value(key) == _pImpl->savedValueMap.value(key))
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
        if (_pImpl)
            _pImpl->valueMap = _pImpl->savedValueMap;
    }

    void ParseObject::revert(const QString &key)
    {
        if (_pImpl)
            _pImpl->valueMap.insert(key, _pImpl->savedValueMap.value(key));
    }

    void ParseObject::clearDirtyState()
    {
        if (_pImpl)
            _pImpl->savedValueMap = _pImpl->valueMap;
    }

    bool ParseObject::hasSameId(const ParseObject& object) const
    {
        return value(Parse::ObjectIdKey) == object.value(Parse::ObjectIdKey);
    }

    QString ParseObject::className() const
    {
        return _pImpl ? _pImpl->className : QString();
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
        return _pImpl ? _pImpl->valueMap.value(key) : QVariant();
    }

    void ParseObject::setValue(const QString &key, const QVariant &variant)
    {
        if (_pImpl)
            _pImpl->valueMap.insert(key, variant);
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

    ParseFile ParseObject::file(const QString &key) const
    {
        return _pImpl ? _pImpl->valueMap.value(key).value<ParseFile>() : ParseFile();
    }

    void ParseObject::setFile(const QString &key, const ParseFile& file)
    {
        setValue(key, QVariant::fromValue(file));
    }

    ParseUser ParseObject::user(const QString & key) const
    {
        ParseObject object = value(key).value<ParseObject>();
        return static_cast<ParseUser>(object);
    }

    void ParseObject::setUser(const QString & key, const ParseUser& user)
    {
        setValue(key, QVariant::fromValue(user));
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
        return _pImpl ? _pImpl->valueMap.keys() : QStringList();
    }

    bool ParseObject::isUserValue(const QString &key)
    {
        return key != Parse::ObjectIdKey &&
            key != Parse::CreatedAtKey &&
            key != Parse::UpdatedAtKey &&
            !key.startsWith('_');
    }

    bool ParseObject::valueMapHasKey(const QString& key) const
    {
        return _pImpl->valueMap.contains(key);
    }

    ParseObjectPointer ParseObject::toPointer() const
    {
        return _pImpl ? ParseObjectPointer(_pImpl->className, objectId()) : ParseObjectPointer();
    }

    QVariantMap ParseObject::toMap() const
    {
        return _pImpl ? _pImpl->valueMap : QVariantMap();
    }

    void ParseObject::setValues(const QVariantMap &variantMap)
    {
        if (!_pImpl)
            return;

        for (auto& key : variantMap.keys())
        {
            if (key != Parse::TypeKey)
                _pImpl->valueMap.insert(key, variantMap.value(key));
        }
    }

    ParseReply* ParseObject::save(QNetworkAccessManager* pNam)
    {
        ParseReply *pReply = nullptr;

        if (_pImpl && objectId().isEmpty())
        {
            pReply = ParseObjectRequest::get()->createObject(*this, pNam);
        }
        else if (_pImpl)
        {
            pReply = ParseObjectRequest::get()->updateObject(*this, pNam);
        }
        
        return pReply;
    }

    ParseReply* ParseObject::fetch(QNetworkAccessManager* pNam)
    {
        return ParseObjectRequest::get()->fetchObject(*this, pNam);
    }

    ParseReply* ParseObject::deleteObject(QNetworkAccessManager* pNam)
    {
        return ParseObjectRequest::get()->deleteObject(*this, pNam);
    }

    ParseReply* ParseObject::saveAll(const QList<ParseObject> &objects, QNetworkAccessManager* pNam)
    {
        return ParseObjectRequest::get()->saveAll(objects, pNam);
    }

    ParseReply* ParseObject::deleteAll(const QList<ParseObject>& objects, QNetworkAccessManager* pNam)
    {
        return ParseObjectRequest::get()->deleteAll(objects, pNam);
    }
}
