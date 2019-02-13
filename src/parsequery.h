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
#include <QEnableSharedFromThis>
#include <QString>
#include <QSharedPointer>
#include <QList>
#include <QUrlQuery>
#include <QJsonObject>
#include <typeinfo>

#define CLASSNAME_FROM_TYPE(T) QString(typeid(T).name()).mid(6)

namespace cg
{
    template <class T>
    class CGPARSE_API ParseQuery : public QEnableSharedFromThis<ParseQuery<T>>
    {
    public:
        ParseQuery()
            : _limit(-1), _skip(0), _count(0)
        {
            _className = CLASSNAME_FROM_TYPE(T);
        }

        ParseQuery(const QString &relationClassName, const QString &relationObjectId, const QString &relationKey)
            : _limit(-1), _skip(0), _count(0)
        {
            _className = CLASSNAME_FROM_TYPE(T);
        }

        template <class T>
        static QSharedPointer<ParseQuery<T>> createQuery()
        {
            return QSharedPointer<ParseQuery<T>>::create();
        }

        ~ParseQuery() { clearResults(); }
        void clearResults() { _results.clear(); }

        QString className() const { return _className; }

        QSharedPointer<ParseQuery<T>> orderByAscending(const QString &key)
        {
            _orderList.clear();
            _orderList.append(key);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> orderByDescending(const QString &key)
        {
            _orderList.clear();
            _orderList.append("-" + key);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> addAscendingOrder(const QString &key)
        {
            _orderList.append(key);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> addDescendingOrder(const QString &key)
        {
            _orderList.append("-" + key);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> setLimit(int limit)
        {
            _limit = limit;
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> setSkip(int skip)
        {
            _skip = skip;
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> selectKeys(const QStringList &keys)
        {
            _keysList = keys;
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> whereEqualTo(const QString &key, const QVariant &value)
        {
            _whereObject.insert(key, ParseUtil::toJsonValue(value));
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> whereNotEqualTo(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$ne", value);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> whereGreaterThan(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$gt", value);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> whereGreaterThanOrEqualTo(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$gte", value);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> whereLessThan(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$lt", value);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> whereLessThanOrEqualTo(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$lte", value);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> whereExists(const QString &key)
        {
            addConstraint(key, "$exists", true);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> whereDoesNotExists(const QString &key)
        {
            addConstraint(key, "$exists", false);
            return sharedFromThis();
        }

        QSharedPointer<ParseQuery<T>> whereFullText(const QString &key, const QString &text)
        {
            QJsonObject termObject, searchObject, textObject;
            termObject.insert("$term", text);
            searchObject.insert("$search", termObject);
            textObject.insert("$text", searchObject);
            _whereObject.insert(key, textObject);
            return sharedFromThis();
        }

        void clear(const QString &key)
        {
            _whereObject.remove(key);
        }

        QUrlQuery urlQuery() const
        {
            QUrlQuery urlQuery;

            if (!_whereObject.isEmpty())
            {
                QJsonDocument doc(_whereObject);
                urlQuery.addQueryItem("where", doc.toJson(QJsonDocument::Compact));
            }

            if (!_orderList.isEmpty())
            {
                urlQuery.addQueryItem("order", _orderList.join(','));
            }

            if (_count > 0)
            {
                urlQuery.addQueryItem("count", QString::number(_count));
            }

            if (_limit >= 0)
            {
                urlQuery.addQueryItem("limit", QString::number(_limit));
            }

            if (_skip > 0)
            {
                urlQuery.addQueryItem("skip", QString::number(_skip));
            }

            if (!_keysList.isEmpty())
            {
                urlQuery.addQueryItem("keys", _keysList.join(','));
            }

            return urlQuery;
        }

        QSharedPointer<T> firstObject() const
        {
            if (_results.size() > 0)
                return _results.first();

            return QSharedPointer<T>();
        }

        const QList<QSharedPointer<T>> & results() const { return _results; }

    private:
        void setResults(const QList<QSharedPointer<T>> &results) { _results = results; }
        void addConstraint(const QString &key, const QString &constraintKey, const QVariant &value)
        {
            if (_whereObject.contains(key))
            {
                if (_whereObject.value(key).isObject())
                {
                    QJsonObject constraintObject = _whereObject.value(key).toObject();
                    constraintObject.insert(constraintKey, ParseUtil::toJsonValue(value));
                    _whereObject.insert(key, constraintObject);
                }
            }
            else
            {
                QJsonObject constraintObject;
                constraintObject.insert(constraintKey, ParseUtil::toJsonValue(value));
                _whereObject.insert(key, constraintObject);
            }
        }

    private:
        QString _className;
        QJsonObject _whereObject;
        int _limit, _skip, _count;
        QStringList _keysList, _orderList, _includeList;
        QList<QSharedPointer<T>> _results;
    };
}

#endif // CGPARSE_PARSEQUERY_H
