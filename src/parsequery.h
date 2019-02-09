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
#include <QSharedPointer>
#include <QList>
#include <QUrlQuery>
#include <QJsonObject>
#include <typeinfo>

namespace cg
{
    template <class T>
    class CGPARSE_API ParseQuery
    {
    public:
        ParseQuery()
        {
            _className = typeid(T).name();
        }

        ParseQuery(const QString &relationClassName, const QString &relationObjectId,
            const QString &relationKey)
        {
            _className = typeid(T).name();
        }

        template <class T>
        static QSharedPointer<ParseQuery<T>> createQuery()
        {
            return QSharedPointer<ParseQuery<T>>::create();
        }

        QString className() const;

        ParseQuery * orderByAscending(const QString &key);
        ParseQuery * orderByDescending(const QString &key);
        ParseQuery * addAscendingOrder(const QString &key);
        ParseQuery * addDescendingOrder(const QString &key);
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
        QSharedPointer<T> firstObject() const;
        const QList<QSharedPointer<T>> & results() const { return _results; }

    private:
        friend ParseClientPrivate;

        void setResults(const QList<QSharedPointer<T>> &results) { _results = results; }
        void addConstraint(const QString &key, const QString &constraintKey, const QVariant &value);

    private:
        const QString _className;
        QList<QSharedPointer<T>> _results;
        QJsonObject _whereObject;
        int _limit, _skip, _count;
        QStringList _keysList, _orderList;
    };
}

#endif // CGPARSE_PARSEQUERY_H
