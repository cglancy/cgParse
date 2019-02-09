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
#include "parserelation.h"
#include "parsequery.h"
#include <QString>
#include <QDateTime>
#include <QVariant>

namespace cg
{
    class CGPARSE_API ParseObject
    {
    public:
        static const QString ObjectIdKey;
        static const QString CreatedAtKey;
        static const QString UpdatedAtKey;

    public:
        ParseObject();
        ParseObject(const QString &className);
        virtual ~ParseObject();

        static ParseObject* create(const QString &className);
        static ParseObject* createWithoutData(const QString &className, const QString &objectId);

        template <class T>
        static T* create()
        {
            return new T;
        }
        template <class T>
        static T* createWithoutData(const QString &objectId)
        {
            T* pObject = new T;
            pObject->setValue(ParseObject::ObjectIdKey, objectId);
            return pObject;
        }

        QString className() const;
        QString objectId() const;
        QDateTime createdAt() const;
        QDateTime updatedAt() const;

        bool hasSameId(ParseObject* pObject) const;
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
        T* object(const QString &key) const
        {
            return dynamic_cast(objectValue(key));
        }
        void setObject(const QString &key, ParseObject* pObject);

        ParseFile* file(const QString &key) const;
        void setFile(const QString &key, ParseFile *pFile);

        ParseUser* user(const QString &key) const;
        void setUser(const QString &key, ParseUser *pFile);

        //template <class T>
        //ParseRelation<T>* relation(const QString &key)
        //{
        //    return new ParseRelation<T>(className(), objectId(), key);
        //}

        template <class T>
        ParseRelation<T>* relation(const QString &key)
        {

        }

        bool contains(const QString &key) const;
        QStringList keys(bool onlyUserValues = true) const;
        QVariantMap valueMap(bool onlyUserValues = true) const;
        void setValues(const QVariantMap &valueMap);
        void clearDirtyState();
        static bool isUserValue(const QString &key);

        void save();
        void fetch();
        void deleteObject();

    private:
        ParseObject* objectValue(const QString &key) const;

    private:
        ParseObject *_pParent;
        QString _className;
        QVariantMap _valueMap, _savedValueMap;
    };

    Q_DECLARE_METATYPE(ParseObject*);
}

#endif // CGPARSE_PARSEOBJECT_H
