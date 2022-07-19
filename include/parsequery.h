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

#include "parsequeryrequest.h"
#include "parsequeryimpl.h"
#include "parseconvert.h"
#include <QString>
#include <QSharedPointer>
#include <QList>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMetaType>


namespace cg
{
    class ParseReply;

    template <class T>
    class ParseQuery
    {
    public:

        static ParseQuery<T> orQuery(const QList<ParseQuery<T>> &queries)
        {
            ParseQuery<T> query;
            QJsonArray jsonArray;

            for (auto & subQuery : queries)
                jsonArray.append(subQuery._pImpl->whereObject);

            query._pImpl->whereObject.insert("$or", jsonArray);
            return query;
        }

    public:
        ParseQuery(const QString& className = QString())
        {
            if (!className.isEmpty())
                _pImpl = QSharedPointer<ParseQueryImpl>::create(className);
            else
                _pImpl = QSharedPointer<ParseQueryImpl>::create(classNameFromType<T>());
        }

        ParseQuery(const QString &relationClassName, const QString &relationObjectId, const QString &relationKey)
         {
            _pImpl = QSharedPointer<ParseQueryImpl>::create(classNameFromType<T>());

            QJsonObject pointerObject;
            pointerObject.insert(Parse::TypeKey, Parse::PointerValue);
            pointerObject.insert(Parse::ClassNameKey, relationClassName);
            pointerObject.insert(Parse::ObjectIdKey, relationObjectId);

            QJsonObject relatedToObject;
            relatedToObject.insert("object", pointerObject);
            relatedToObject.insert("key", relationKey);

            _pImpl->whereObject.insert("$relatedTo", relatedToObject);
        }

        ParseQuery(const ParseQuery& query)
            : _pImpl(query._pImpl)
        {
        }

        ~ParseQuery() 
        { 
        }

        ParseQuery& operator=(const ParseQuery& query)
        {
            _pImpl = query._pImpl;
            return *this;
        }

        QString className() const { return _pImpl->className; }

        ParseQuery<T>& orderByAscending(const QString &key)
        {
            _pImpl->orderList.clear();
            _pImpl->orderList.append(key);
            return *this;
        }

        ParseQuery<T>& orderByDescending(const QString &key)
        {
            _pImpl->orderList.clear();
            _pImpl->orderList.append("-" + key);
            return *this;
        }

        ParseQuery<T>& addAscendingOrder(const QString &key)
        {
            _pImpl->orderList.append(key);
            return *this;
        }

        ParseQuery<T>& addDescendingOrder(const QString &key)
        {
            _pImpl->orderList.append("-" + key);
            return *this;
        }

        ParseQuery<T>& setLimit(int limit)
        {
            _pImpl->limit = limit;
            return *this;
        }

        ParseQuery<T>& setSkip(int skip)
        {
            _pImpl->skip = skip;
            return *this;
        }

        ParseQuery<T>& selectKeys(const QStringList &keys)
        {
            _pImpl->keysList = keys;
            return *this;
        }

        ParseQuery<T>& include(const QString &key)
        {
            _pImpl->includeList.append(key);
            return *this;
        }

        ParseQuery<T>& whereEqualTo(const QString &key, const QVariant &value)
        {
            _pImpl->whereObject.insert(key, QJsonValue::fromVariant(value));
            return *this;
        }

        ParseQuery<T>& whereEqualTo(const QString &key, const ParseObject& object)
        {
            _pImpl->whereObject.insert(key, QJsonValue::fromVariant(ParseConvert::toVariantMap(object)));
            return *this;
        }

        ParseQuery<T>& whereNotEqualTo(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$ne", value);
            return *this;
        }

        ParseQuery<T>& whereGreaterThan(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$gt", value);
            return *this;
        }

        ParseQuery<T>& whereGreaterThanOrEqualTo(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$gte", value);
            return *this;
        }

        ParseQuery<T>& whereLessThan(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$lt", value);
            return *this;
        }

        ParseQuery<T>& whereLessThanOrEqualTo(const QString &key, const QVariant &value)
        {
            addConstraint(key, "$lte", value);
            return *this;
        }

        ParseQuery<T>& whereExists(const QString &key)
        {
            addConstraint(key, "$exists", true);
            return *this;
        }

        ParseQuery<T>& whereDoesNotExists(const QString &key)
        {
            addConstraint(key, "$exists", false);
            return *this;
        }

        ParseQuery<T>& whereFullText(const QString &key, const QString &text)
        {
            QJsonObject termObject, searchObject, textObject;
            termObject.insert("$term", text);
            searchObject.insert("$search", termObject);
            textObject.insert("$text", searchObject);
            _pImpl->whereObject.insert(key, textObject);
            return *this;
        }

        QJsonObject whereObject() const
        {
            return _pImpl->whereObject;
        }

        template <class T2>
        ParseQuery<T>& whereInQuery(const QString& key, const ParseQuery<T2>& q2)
        {
            QJsonObject queryObject;
            queryObject.insert("where", q2.whereObject());
            queryObject.insert("className", q2.className());

            QJsonObject keyObject;
            keyObject.insert("$inQuery", queryObject);

            _pImpl->whereObject.insert(key, keyObject);

            return *this;
        }

        template <class T2>
        ParseQuery<T>& whereNotInQuery(const QString& key, const ParseQuery<T2>& q2)
        {
            QJsonObject queryObject;
            queryObject.insert("where", q2.whereObject());
            queryObject.insert("className", q2.className());

            QJsonObject keyObject;
            keyObject.insert("$notInQuery", queryObject);

            _pImpl->whereObject.insert(key, keyObject);

            return *this;
        }

        void clear(const QString &key)
        {
            _pImpl->whereObject.remove(key);
        }

        QUrlQuery urlQuery() const
        {
            QUrlQuery urlQuery;

            if (!_pImpl->whereObject.isEmpty())
            {
                QJsonDocument doc(_pImpl->whereObject);
                urlQuery.addQueryItem("where", doc.toJson(QJsonDocument::Compact));
            }

            if (!_pImpl->orderList.isEmpty())
                urlQuery.addQueryItem("order", _pImpl->orderList.join(','));

            if (_pImpl->count > 0)
                urlQuery.addQueryItem("count", QString::number(_pImpl->count));

            if (_pImpl->limit >= 0)
                urlQuery.addQueryItem("limit", QString::number(_pImpl->limit));

            if (_pImpl->skip > 0)
                urlQuery.addQueryItem("skip", QString::number(_pImpl->skip));

            if (!_pImpl->keysList.isEmpty())
                urlQuery.addQueryItem("keys", _pImpl->keysList.join(','));

            if (!_pImpl->includeList.isEmpty())
                urlQuery.addQueryItem("include", _pImpl->includeList.join(','));

            return urlQuery;
        }

        ParseReply* count(QNetworkAccessManager* pNam = nullptr)
        {
            int origCount = _pImpl->count, origLimit = _pImpl->limit;
            _pImpl->count = 1;
            _pImpl->limit = 0;
            ParseReply * pReply = ParseQueryRequest::get()->countObjects(_pImpl, urlQuery(), pNam);
            _pImpl->count = origCount;
            _pImpl->limit = origLimit;
            return pReply;
        }

        int countResult() const
        {
            return _pImpl->countResult;
        }

        ParseReply* get(const QString &objectId, QNetworkAccessManager* pNam = nullptr)
        {
            return ParseQueryRequest::get()->getObject(_pImpl, objectId, pNam);
        }

        ParseReply* find(QNetworkAccessManager* pNam = nullptr)
        {
            return ParseQueryRequest::get()->findObjects(_pImpl, urlQuery(), pNam);
        }

        T first()
        {
            if (_pImpl->results.size() > 0)
                return _pImpl->results.first();

            return T();
        }

        QList<T> results()
        {
            QList<T> list;

            for (auto const& object : _pImpl->results)
                list.append(object);

            return list;
        }

    private:
        void addConstraint(const QString &key, const QString &constraintKey, const QVariant &value)
        {
            if (_pImpl->whereObject.contains(key))
            {
                if (_pImpl->whereObject.value(key).isObject())
                {
                    QJsonObject constraintObject = _pImpl->whereObject.value(key).toObject();
                    constraintObject.insert(constraintKey, QJsonValue::fromVariant(value));
                    _pImpl->whereObject.insert(key, constraintObject);
                }
            }
            else
            {
                QJsonObject constraintObject;
                constraintObject.insert(constraintKey, QJsonValue::fromVariant(value));
                _pImpl->whereObject.insert(key, constraintObject);
            }
        }

    private:
        QSharedPointer<ParseQueryImpl> _pImpl;
    };
}

#endif // CGPARSE_PARSEQUERY_H
