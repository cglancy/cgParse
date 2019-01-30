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
#include <QByteArray>
#include <QMap>

namespace cg
{
    class ParseQuery;

    class CGPARSE_API ParseObject : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString objectId READ objectId WRITE setObjectId)
        Q_PROPERTY(QDateTime createdAt READ createdAt WRITE setCreatedAt)
        Q_PROPERTY(QDateTime updatedAt READ updatedAt WRITE setUpdatedAt)

    public:
        static const QByteArray ObjectIdPropertyName;
        static const QByteArray CreatedAtPropertyName;
        static const QByteArray UpdatedAtPropertyName;

    public:
        Q_INVOKABLE ParseObject(const QString &className);
        ParseObject(const ParseObject &object);
        virtual ~ParseObject();

        QString className() const;
        QString objectId() const;
        QDateTime createdAt() const;
        QDateTime updatedAt() const;

        bool isNew() const;
        bool isDirty() const;
        bool isDirty(const QByteArray &propertyName) const;
        void setDirty(bool dirty);
        bool hasSameId(ParseObject *pObject) const;

        bool hasProperty(const QByteArray &propertyName) const;
        QList<QByteArray> propertyNames() const;

    public slots:
        void save();
        void fetch();
        void deleteObject();

    signals:
        void saveFinished(int errorCode);
        void fetchFinished(int errorCode);
        void deleteFinished(int errorCode);
        void propertyChanged(const QByteArray &propertyName);

    private:
        void setObjectId(const QString &objectId);
        void setCreatedAt(const QDateTime &createdAt);
        void setUpdatedAt(const QDateTime &updatedAt);

    private:
        QString _className, _objectId;
        QDateTime _createdAt, _updatedAt;
        QMap<QByteArray, QVariant> _fetchedMap;
    };

    Q_DECLARE_METATYPE(ParseObject*);
}

#endif // CGPARSE_PARSEOBJECT_H
