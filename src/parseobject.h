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

    class CGPARSE_API ParseObject : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(ParseObject)
        Q_PROPERTY(QString objectId READ objectId)
        Q_PROPERTY(QDateTime createdAt READ createdAt)
        Q_PROPERTY(QDateTime updatedAt READ updatedAt)

    public:
        static const QString ObjectIdName;
        static const QString CreatedAtName;
        static const QString UpdatedAtName;

    public:
        Q_INVOKABLE ParseObject(const QString &className);
        virtual ~ParseObject();

        virtual ParseObject * clone() const;

        QString className() const;
        QString objectId() const;
        QDateTime createdAt() const;
        QDateTime updatedAt() const;

        bool hasSameId(ParseObject *pObject) const;
        bool isDirty() const;
        bool isDirty(const QString &valueName) const;
        void setDirty(bool dirty);

        QVariant value(const QString &name) const;
        void setValue(const QString &name, const QVariant &variant);

        template <class T>
        T object(const QString &name) const
        {
            return qobject_cast<T>(objectValue(name));
        }
        void setObject(const QString &name, ParseObject *pObject);

        ParseFile* file(const QString &name) const;
        void setFile(const QString &name, ParseFile *pFile);

        bool hasValue(const QString &valueName) const;
        QStringList valueNames(bool onlyUserValues = true) const;
        QVariantMap valueMap(bool onlyUserValues = true) const;
        void setValues(const QVariantMap &valueMap);
        static bool isUserValue(const QString &name);

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
        ParseObject *objectValue(const QString &name) const;

    private:
        QString _className;
        QVariantMap _valueMap, _cachedValueMap;
    };

    Q_DECLARE_METATYPE(ParseObject*);
}

#endif // CGPARSE_PARSEOBJECT_H
