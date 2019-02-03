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
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariant>

namespace cg
{
    class ParseQuery;
    class ParseFile;
    class ParseUser;

    class CGPARSE_API ParseObject : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(ParseObject)
        Q_PROPERTY(QString objectId READ objectId)
        Q_PROPERTY(QDateTime createdAt READ createdAt)
        Q_PROPERTY(QDateTime updatedAt READ updatedAt)

    public:
        static const QString ObjectIdKey;
        static const QString CreatedAtKey;
        static const QString UpdatedAtKey;

    public:
        Q_INVOKABLE ParseObject(const QString &className);
        virtual ~ParseObject();

        static ParseObject * create(const QString &className);
        static ParseObject * createWithoutData(const QString &className, const QString &objectId);

        template <class T>
        static T* create()
        {
            return qobject_cast<T*>(create(&T::staticMetaObject));
        }
        template <class T>
        static T* createWithoutData(const QString &objectId)
        {
            return qobject_cast<T*>(createWithoutData(&T::staticMetaObject, objectId));
        }

        virtual ParseObject * clone() const;

        QString className() const;
        QString objectId() const;
        QDateTime createdAt() const;
        QDateTime updatedAt() const;

        bool hasSameId(ParseObject *pObject) const;
        bool isDirty() const;
        bool isDirty(const QString &key) const;
        void setDirty(bool dirty);

        QVariant value(const QString &key) const;
        void setValue(const QString &key, const QVariant &variant);
        void remove(const QString &key);

        template <class T>
        T* object(const QString &key) const
        {
            return qobject_cast<T*>(objectValue(key));
        }
        void setObject(const QString &key, ParseObject *pObject);

        ParseFile* file(const QString &key) const;
        void setFile(const QString &key, ParseFile *pFile);

        ParseUser* user(const QString &key) const;
        void setUser(const QString &key, ParseUser *pFile);

        bool contains(const QString &key) const;
        QStringList keys(bool onlyUserValues = true) const;
        QVariantMap valueMap(bool onlyUserValues = true) const;
        void setValues(const QVariantMap &valueMap);
        static bool isUserValue(const QString &key);

    public slots:
        void save();
        void fetch();
        void deleteObject();

    signals:
        void saveFinished(int errorCode);
        void fetchFinished(int errorCode);
        void deleteFinished(int errorCode);
        void valueChanged(const QString &valueName);

    private:
        ParseObject *objectValue(const QString &key) const;
        static ParseObject * create(const QMetaObject *pMetaObject);
        static ParseObject * createWithoutData(const QMetaObject *pMetaObject, const QString &objectId);

    private:
        QString _className;
        QVariantMap _valueMap, _cachedValueMap;
    };

    Q_DECLARE_METATYPE(ParseObject*);
}

#endif // CGPARSE_PARSEOBJECT_H
