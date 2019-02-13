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

#include "cgparse.h"
#include "parsetypes.h"
#include "parsequery.h"
#include "parserelation.h"
#include "parseerror.h"
#include "parseobjectpointer.h"

#include <QString>
#include <QDateTime>
#include <QVariant>
#include <QFuture>

namespace cg
{
    class CGPARSE_API ParseObject
    {
    public:
        static const QString ClassNameKey;
        static const QString ObjectIdKey;
        static const QString CreatedAtKey;
        static const QString UpdatedAtKey;

    public:
        ParseObject();
        ParseObject(const QString &className);
        virtual ~ParseObject();

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
            pObject->setValue(ParseObject::ObjectIdKey, objectId);
            return pObject;
        }

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
            QSharedPointer<T> pObject = QSharedPointer<T>::create();
            QJsonObject jsonObject = value(key).toJsonObject();
            pObject->setValues(jsonObject);
            return pObject;
        }
        template <class T>
        void setObject(const QString &key, QSharedPointer<T> pObject)
        {
            setValue(key, pObject->toPointer().toJsonObject());
        }

        ParseFilePtr file(const QString &key) const;
        void setFile(const QString &key, ParseFilePtr pFile);

        ParseUserPtr user(const QString &key) const;
        void setUser(const QString &key, ParseUserPtr pUser);

        template <class T>
        QSharedPointer<ParseRelation<T>> relation(const QString &key)
        {
            QSharedPointer<ParseRelation<T>> pRelation = QSharedPointer<ParseRelation<T>>::create();

            if (_valueMap.contains(key) && value(key).canConvert<QJsonObject>())
            {
                pRelation->setValues(value(key).toJsonObject());
            }

            return pRelation;
        }

        template <class T>
        void setRelation(const QString &key, QSharedPointer<ParseRelation<T>> pRelation)
        {
            setValue(key, pRelation->toJsonObject());
        }

        bool contains(const QString &key) const;

        QFuture<ParseError> save();
        QFuture<ParseError> fetch();
        QFuture<ParseError> deleteObject();

    private:
        friend class ParseRequestObject;
        ParseObjectPointer toPointer() const;
        QJsonObject toJsonObject() const;
        void setValues(const QJsonObject &jsonObject);
        QStringList keys(bool onlyUserValues = true) const;
        QVariantMap valueMap(bool onlyUserValues = true) const;
        void clearDirtyState();
        static bool isUserValue(const QString &key);

    private:
        QString _className;
        QVariantMap _valueMap, _savedValueMap;
    };
}

#endif // CGPARSE_PARSEOBJECT_H
