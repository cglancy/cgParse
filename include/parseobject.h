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
#include "parsetypes.h"
#include "parserelation.h"
#include "parseerror.h"
#include "parseobjectpointer.h"
#include "parsegeopoint.h"

#include <QEnableSharedFromThis>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QScopedPointer>

namespace cg
{
    class ParseObjectHelper;

    class CGPARSE_API ParseObject : public QEnableSharedFromThis<ParseObject>
    {
    public:
        static ParseObjectPtr create(const QString &className);
        static ParseObjectPtr createWithoutData(const QString &className, const QString &objectId);

        template <class T>
        static QSharedPointer<T> create()
        {
            return QSharedPointer<T>::create();
        }
        template <class T>
        static QSharedPointer<T> createWithoutData(const QString &objectId)
        {
            QSharedPointer<T> pObject = QSharedPointer<T>::create();
            pObject->setValue(Parse::ObjectIdKey, objectId);
            return pObject;
        }

        static ParseReply* saveAll(const QList<ParseObjectPtr> &objects);
        static ParseReply* deleteAll(const QList<ParseObjectPtr> &objects);

        template <class T>
        static QSharedPointer<ParseQuery<T>> query()
        {
            return QSharedPointer<ParseQuery<T>>::create();
        }

    public:
        ParseObject();
        ParseObject(const ParseObject &object);
        ParseObject(const QString &className);
        virtual ~ParseObject();

        QString className() const;
        QString objectId() const;
        QDateTime createdAt() const;
        QDateTime updatedAt() const;

        bool hasSameId(ParseObjectPtr pObject) const;
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
        void removeAll(const QString &key, const QVariantList &valueList);

        ParseFilePtr file(const QString &key) const;
        void setFile(const QString &key, ParseFilePtr pFile);

        ParseUserPtr user(const QString &key) const;
        void setUser(const QString &key, ParseUserPtr pUser);

        QDateTime dateTime(const QString &key) const;
        void setDateTime(const QString &key, const QDateTime &dateTime);

        QDate date(const QString &key) const;
        void setDate(const QString &key, const QDate &date);

        ParseGeoPoint geoPoint(const QString &key) const;
        void setGeoPoint(const QString &key, const ParseGeoPoint &geoPoint);

        template <class T>
        QSharedPointer<T> object(const QString &key) const
        {
            ParseObjectPtr pBaseObject = value(key).value<ParseObjectPtr>();
            return pBaseObject.staticCast<T>();
        }

        template <class T>
        void setObject(const QString &key, QSharedPointer<T> pObject)
        {
            ParseObjectPtr pBaseObject = pObject.staticCast<ParseObject>();
            setValue(key, QVariant::fromValue(pBaseObject));
        }

        template <class T>
        QSharedPointer<ParseRelation<T>> relation(const QString &key)
        {
            QSharedPointer<ParseRelation<T>> pRelation = QSharedPointer<ParseRelation<T>>::create(_className, objectId(), key);

            if (_valueMap.contains(key) && value(key).canConvert<QVariantMap>())
            {
                pRelation->setValues(value(key).toMap());
            }

            return pRelation;
        }

        template <class T>
        void setRelation(const QString &key, QSharedPointer<ParseRelation<T>> pRelation)
        {
            setValue(key, pRelation->toMap());
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
        void addObject(const QString &key, QSharedPointer<T> pObject)
        {
            ParseObjectPtr pBaseObject = pObject.staticCast<ParseObject>();
            add(key, QVariant::fromValue(pBaseObject));
        }

        template <class T>
        void addUniqueObject(const QString &key, QSharedPointer<T> pObject)
        {
            ParseObjectPtr pBaseObject = pObject.staticCast<ParseObject>();
            addUnique(key, QVariant::fromValue(pBaseObject));
        }

        template <class T>
        void addAllObjects(const QString &key, const QList<QSharedPointer<T>> &objectList)
        {
            QVariantList variantList;
            for (auto & pObject : objectList)
            {
                ParseObjectPtr pBaseObject = pObject.staticCast<ParseObject>();
                variantList.append(QVariant::fromValue(pBaseObject));
            }
            addAll(key, variantList);
        }

        template <class T>
        void addAllUniqueObjects(const QString &key, const QList<QSharedPointer<T>> &objectList)
        {
            QVariantList variantList;
            for (auto & pObject : objectList)
            {
                ParseObjectPtr pBaseObject = pObject.staticCast<ParseObject>();
                variantList.append(QVariant::fromValue(pBaseObject));
            }
            addAllUnique(key, variantList);
        }

        template <class T>
        void removeAllObjects(const QString &key, const QList<QSharedPointer<T>> &objectList)
        {
            QVariantList variantList;
            for (auto & pObject : objectList)
            {
                ParseObjectPtr pBaseObject = pObject.staticCast<ParseObject>();
                variantList.append(QVariant::fromValue(pBaseObject));
            }
            removeAll(key, variantList);
        }

        template <class T>
        QList<QSharedPointer<T>> objects(const QString &key) const
        {
            QList<QSharedPointer<T>> objectList;
            QVariantList variantList = value(key).toList();
            for (auto & variant : variantList)
            {
                ParseObjectPtr pBaseObject = variant.value<ParseObjectPtr>();
                objectList.append(pBaseObject.staticCast<T>());
            }

            return objectList;
        }

        ParseReply* save();
        ParseReply* fetch();
        ParseReply* deleteObject();

        bool contains(const QString &key) const;
        void clearDirtyState();
        QVariantMap toMap(bool onlyUserValues = true) const;
        ParseObjectPointer toPointer() const;
        void setValues(const QVariantMap &variantMap);
        QStringList keys(bool onlyUserValues = true) const;
        static bool isUserValue(const QString &key);

    private:
        static ParseObjectHelper * staticHelper();

    private:
        QString _className;
        QVariantMap _valueMap, _savedValueMap;
        QScopedPointer<ParseObjectHelper> _pHelper;
        static ParseObjectHelper *_pStaticHelper;
    };

    Q_DECLARE_METATYPE(ParseObject);
    Q_DECLARE_METATYPE(QSharedPointer<ParseObject>);
}

#endif // CGPARSE_PARSEOBJECT_H
