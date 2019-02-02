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
#ifndef CGPARSE_PARSEQUERY_H
#define CGPARSE_PARSEQUERY_H
#pragma once

#include "cgparse.h"
#include "parseclient.h"
#include <QObject>
#include <QList>
#include <QUrlQuery>
#include <QJsonObject>

namespace cg
{
    class ParseObject;

    class CGPARSE_API ParseQuery : public QObject
    {
        Q_OBJECT
    public:
        ~ParseQuery();

        template <class T>
        static ParseQuery * createQuery()
        {
            return new ParseQuery(&T::staticMetaObject);
        }

        const QMetaObject* queryMetaObject() const;
        QString className() const;

        ParseQuery * orderByAscending(const QString &key);
        ParseQuery * orderByDescending(const QString &key);
        ParseQuery * setLimit(int limit);
        ParseQuery * setSkip(int skip);
        ParseQuery * selectKeys(const QStringList &keys);

        ParseQuery * whereEqualTo(const QString &key, const QVariant &value);
        ParseQuery * whereNotEqualTo(const QString &key, const QVariant &value);
        ParseQuery * whereGreaterThan(const QString &key, const QVariant &value);
        ParseQuery * whereGreaterThanOrEqualTo(const QString &key, const QVariant &value);
        ParseQuery * whereLessThan(const QString &key, const QVariant &value);
        ParseQuery * whereLessThanOrEqualTo(const QString &key, const QVariant &value);
        ParseQuery * whereExists(const QString &key);
        ParseQuery * whereDoesNotExists(const QString &key);

        ParseQuery * whereFullText(const QString &key, const QString &text);

        void clear(const QString &key);

        QUrlQuery urlQuery() const;

        void clearResults();
        ParseObject * firstObject() const;
        const QList<ParseObject*> & resultObjects() const { return _objects; }

        template <class T>
        T first() const
        {
            T object = nullptr;
            if (!_objects.isEmpty())
                object = qobject_cast<T>(_objects.first());
            return object;
        }

        template <class T>
        QList<T> results() const
        {
            QList<T> objects;
            for (auto & pObject : _objects)
                objects.append(qobject_cast<T>(pObject));
            return objects;
        }

    public slots:
        void get(const QString &objectId);
        void find();
        void count();

    signals:
        void getFinished(ParseObject *pObject, int errorCode);
        void findFinished(const QList<ParseObject*> &objects, int errorCode);
        void countFinished(int count, int errorCode);

    private:
        friend ParseClientPrivate;
        ParseQuery(const QMetaObject *pMetaObject);
        void setResults(const QList<ParseObject*> &results);
        void addConstraint(const QString &key, const QString &constraintKey, const QVariant &value);

    private:
        const QMetaObject *_pMetaObject;
        QList<ParseObject*> _objects;
        QJsonObject _whereObject;
        int _limit, _skip;
        QStringList _keysList, _orderList;
    };
}

#endif // CGPARSE_PARSEQUERY_H
