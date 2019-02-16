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

#include <QEnableSharedFromThis>
#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QFuture>
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

        static QFuture<int> createAll(const QList<ParseObjectPtr> &objects);
        static QFuture<int> updateAll(const QList<ParseObjectPtr> &objects);
        static QFuture<int> deleteAll(const QList<ParseObjectPtr> &objects);

        template <class T>
        static QSharedPointer<ParseQuery<T>> query()
        {
            return QSharedPointer<ParseQuery<T>>::create();
        }

    public:
        ParseObject();
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

        template <class T>
        QSharedPointer<T> object(const QString &key) const
        {
            QSharedPointer<T> pObject;

            if (_valueMap.contains(key) && ParseObjectPointer::isPointer(_valueMap.value(key)))
            {
                ParseObjectPointer pointer(_valueMap.value(key));
                pObject = ParseObject::createWithoutData<T>(pointer.objectId());
            }
            else if (_valueMap.contains(key) && ParseObjectPointer::isObject(_valueMap.value(key)))
            {
                QVariantMap map = _valueMap.value(key).toMap();
                pObject = ParseObject::create<T>();
                map.remove(Parse::TypeKey);
                pObject->setValues(map);
            }

            return pObject;
        }
        template <class T>
        void setObject(const QString &key, QSharedPointer<T> pObject)
        {
            setValue(key, pObject->toPointer().toMap());
        }

        ParseFilePtr file(const QString &key) const;
        void setFile(const QString &key, ParseFilePtr pFile);

        ParseUserPtr user(const QString &key) const;
        void setUser(const QString &key, ParseUserPtr pUser);

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

        bool contains(const QString &key) const;
        void setValues(const QJsonObject &jsonObject);
        void setValues(const QVariantMap &variantMap);
        void clearDirtyState();
        QJsonObject toJsonObject(bool onlyUserValues = true) const;
        QVariantMap toMap(bool onlyUserValues = true) const;
        ParseObjectPointer toPointer() const;

        QFuture<int> save();
        QFuture<int> fetch();
        QFuture<int> deleteObject();

    private:
        QStringList keys(bool onlyUserValues = true) const;
        static bool isUserValue(const QString &key);
        static ParseObjectHelper * staticHelper();

    private:
        QString _className;
        QVariantMap _valueMap, _savedValueMap;
        QScopedPointer<ParseObjectHelper> _pHelper;
        static ParseObjectHelper *_pStaticHelper;
    };
}

#endif // CGPARSE_PARSEOBJECT_H
