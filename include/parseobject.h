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
#ifndef CGPARSE_PARSEOBJECT_H
#define CGPARSE_PARSEOBJECT_H
#pragma once

#include "parse.h"
#include "parserelation.h"
#include "parseerror.h"
#include "parseobjectpointer.h"
#include "parsegeopoint.h"
#include "parseconvert.h"

#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QSharedPointer>

class QNetworkAccessManager;

namespace cg
{
    class ParseObjectImpl;
    class ParseReply;
    class ParseFile;
    class ParseUser;
    class ParseACL;

    class CGPARSE_API ParseObject
    {
    public:
        template <class T>
        static T create()
        {
            return ParseObject(classNameFromType<T>());
        }

        template <class T>
        static T createWithoutData(const QString& objectId)
        {
            ParseObject object(classNameFromType<T>());
            object.setValue(Parse::ObjectIdKey, objectId);
            return object;
        }

        static ParseObject create(const QString &className);
        static ParseObject createWithoutData(const QString &className, const QString &objectId);

        static ParseReply* saveAll(const QList<ParseObject> &objects, QNetworkAccessManager* pNam = nullptr);
        static ParseReply* deleteAll(const QList<ParseObject> &objects, QNetworkAccessManager* pNam = nullptr);

    public: 
        ParseObject();
        explicit ParseObject(const QString &className);
        ParseObject(const ParseObject &object);
        virtual ~ParseObject();

        ParseObject& operator=(const ParseObject& object);
        bool operator==(const ParseObject& object);

        virtual void assign(const ParseObject& object);
        void nullify();

        QString className() const;
        QString objectId() const;
        QDateTime createdAt() const;
        QDateTime updatedAt() const;

        bool isNull() const;
        bool hasSameId(const ParseObject& object) const;
        bool isDirty() const;
        bool isDirty(const QString &key) const;
        void revert();
        void revert(const QString &key);

        QVariant value(const QString &key) const;
        void setValue(const QString &key, const QVariant &value);
        void remove(const QString &key);

        void add(const QString &key, const QVariant &value);
        void addUnique(const QString &key, const QVariant &value);
        void addAll(const QString &key, const QVariantList &valueList);
        void addAllUnique(const QString &key, const QVariantList &valueList);
        void remove(const QString& key, const QVariant& value);
        void removeAll(const QString &key, const QVariantList &valueList);

        ParseFile file(const QString &key) const;
        void setFile(const QString &key, const ParseFile& file);

        ParseUser user(const QString &key) const;
        void setUser(const QString &key, const ParseUser& user);

        QDateTime dateTime(const QString &key) const;
        void setDateTime(const QString &key, const QDateTime &dateTime);

        QDate date(const QString &key) const;
        void setDate(const QString &key, const QDate &date);

        ParseGeoPoint geoPoint(const QString &key) const;
        void setGeoPoint(const QString &key, const ParseGeoPoint &geoPoint);

        ParseACL ACL() const;
        void setACL(const ParseACL& acl);

        template <class T>
        T object(const QString &key) const
        {
            return value(key).value<ParseObject>();
        }

        void setObject(const QString &key, const ParseObject& object)
        {
            setValue(key, QVariant::fromValue<ParseObject>(object));
        }

        template <class T>
        ParseRelation<T> relation(const QString &key)
        {
            ParseRelation<T> relation = ParseRelation<T>(className(), objectId(), key);

            if (valueMapHasKey(key) && value(key).canConvert<QVariantMap>())
            {
                QVariantMap map = value(key).toMap();
                if (map.contains(Parse::OperatorKey) && map.value(Parse::OperatorKey).toString() == Parse::AddRelationValue)
                {
                    QVariantList list = map.value("objects").toList();
                    for (auto & variant : list)
                    {
                        ParseObject baseObject = ParseConvert::toObject(variant);
                        relation.add(baseObject);
                    }
                }
                else if (map.contains(Parse::OperatorKey) && map.value(Parse::OperatorKey).toString() == Parse::RemoveRelationValue)
                {
                    QVariantList list = map.value("objects").toList();
                    for (auto & variant : list)
                    {
                        ParseObject baseObject = ParseConvert::toObject(variant);
                        relation.remove(baseObject);
                    }
                }
            }

            return relation;
        }

        template <class T>
        void setRelation(const QString &key, const ParseRelation<T>& relation)
        {
            setValue(key, toMap(relation));
        }

        template <class T>
        void addAll(const QString &key, const QList<T> &list)
        {
            QVariantList variantList;
            for (auto & value : list)
                variantList.append(value);
            addAll(key, variantList);
        }

        template <class T>
        void addAllUnique(const QString &key, const QList<T> &list)
        {
            QVariantList variantList;
            for (auto & value : list)
                variantList.append(value);
            addAllUnique(key, variantList);
        }

        template <class T>
        void removeAll(const QString &key, const QList<T> &list)
        {
            QVariantList variantList;
            for (auto & value : list)
                variantList.append(value);
            removeAll(key, variantList);
        }

        template <class T>
        QList<T> list(const QString &key) const
        {
            QList<T> list;
            QVariantList variantList = value(key).toList();
            for (auto & variant : variantList)
            {
                if (variant.canConvert<T>())
                    list.append(variant.value<T>());
            }

            return list;
        }

        template <class T>
        QList<T> objects(const QString &key) const
        {
            QList<T> objectList;
            QVariantList variantList = value(key).toList();
            for (auto & variant : variantList)
            {
                ParseObject object = variant.value<ParseObject>();
                objectList.append(static_cast<T>(object));
            }

            return objectList;
        }

        ParseReply* save(QNetworkAccessManager* pNam = nullptr);
        ParseReply* fetch(QNetworkAccessManager* pNam = nullptr);
        ParseReply* deleteObject(QNetworkAccessManager* pNam = nullptr);

        bool contains(const QString &key) const;
        void clearDirtyState();
        QVariantMap toMap() const;
        ParseObjectPointer toPointer() const;
        void setValues(const QVariantMap &variantMap);
        QStringList keys() const;
        static bool isUserValue(const QString &key);

        friend bool operator==(const ParseObject& object1, const ParseObject& object2);
        friend bool operator<(const ParseObject& object1, const ParseObject& object2);
        friend uint qHash(const ParseObject& object, uint seed);

    private:
        bool valueMapHasKey(const QString& key) const;

        template <class T>
        QVariantMap toMap(const ParseRelation<T>& relation) const
        {
            QVariantMap map;
            QVariantList list;

            if (relation.addList().size() > 0)
            {
                for (auto const& object : relation.addList())
                {
                    list.append(object.toPointer().toMap());
                }

                map.insert(Parse::OperatorKey, Parse::AddRelationValue);
                map.insert("objects", list);
            }
            else if (relation.removeList().size() > 0)
            {
                for (auto const& object : relation.removeList())
                {
                    list.append(object.toPointer().toMap());
                }

                map.insert(Parse::OperatorKey, Parse::RemoveRelationValue);
                map.insert("objects", list);
            }

            return map;
        }

    private:
        QSharedPointer<ParseObjectImpl> _pImpl;
    };

}

Q_DECLARE_METATYPE(cg::ParseObject);

#endif // CGPARSE_PARSEOBJECT_H
