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
#include "parsequery.h"
#include "parseclient.h"
#include "parseobject.h"
#include "parseutil.h"

#include <QJsonObject>
#include <QJsonDocument>

namespace cg
{
    ParseQuery::ParseQuery(const QMetaObject *pMetaObject)
        : QObject(ParseClient::instance()),
        _pMetaObject(pMetaObject),
        _limit(0),
        _skip(0)
    {
    }

    ParseQuery::~ParseQuery()
    {
        clearResults();
    }

    const QMetaObject* ParseQuery::queryMetaObject() const
    {
        return _pMetaObject;
    }

    QString ParseQuery::className() const
    {
        return _pMetaObject->className();
    }

    void ParseQuery::clearResults()
    {
        for (auto & pObject : _objects)
            delete pObject;
        _objects.clear();
    }

    void ParseQuery::setResults(const QList<ParseObject*> &results)
    {
        clearResults();
        _objects = results;
    }

    void ParseQuery::get(const QString &objectId)
    {
        ParseClient::instance()->getObject(this, objectId);
    }

    void ParseQuery::find()
    {
        QStringList parameters;
        ParseClient::instance()->findObjects(this);
    }

    void ParseQuery::count()
    {
        QStringList parameters;
        ParseClient::instance()->countObjects(this);
    }

    QUrlQuery ParseQuery::urlQuery() const
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

        if (_limit > 0)
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

    ParseObject * ParseQuery::firstObject() const
    {
        ParseObject *pObject = nullptr;
        if (!_objects.isEmpty())
            pObject = _objects.first();
        return pObject;
    }

    ParseQuery * ParseQuery::orderByAscending(const QString &key)
    {
        _orderList.append(key);
        return this;
    }

    ParseQuery * ParseQuery::orderByDescending(const QString &key)
    {
        _orderList.append("-" + key);
        return this;
    }

    ParseQuery * ParseQuery::setLimit(int limit)
    {
        _limit = limit;
        return this;
    }

    ParseQuery * ParseQuery::setSkip(int skip)
    {
        _skip = skip;
        return this;
    }

    ParseQuery * ParseQuery::selectKeys(const QStringList & keys)
    {
        _keysList = keys;
        return nullptr;
    }

    void ParseQuery::addConstraint(const QString &key, const QString &constraintKey, const QVariant &value)
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

    ParseQuery * ParseQuery::whereEqualTo(const QString &key, const QVariant &value)
    {
        _whereObject.insert(key, ParseUtil::toJsonValue(value));
        return this;
    }

    ParseQuery * ParseQuery::whereNotEqualTo(const QString &key, const QVariant &value)
    {
        addConstraint(key, "$ne", value);
        return this;
    }

    ParseQuery * ParseQuery::whereGreaterThan(const QString &key, const QVariant &value)
    {
        addConstraint(key, "$gt", value);
        return this;
    }

    ParseQuery * ParseQuery::whereGreaterThanOrEqualTo(const QString &key, const QVariant &value)
    {
        addConstraint(key, "$gte", value);
        return this;
    }

    ParseQuery * ParseQuery::whereLessThan(const QString &key, const QVariant &value)
    {
        addConstraint(key, "$lt", value);
        return this;
    }

    ParseQuery * ParseQuery::whereLessThanOrEqualTo(const QString &key, const QVariant &value)
    {
        addConstraint(key, "$lte", value);
        return this;
    }

    ParseQuery * ParseQuery::whereExists(const QString &key)
    {
        addConstraint(key, "$exists", true);
        return this;
    }

    ParseQuery * ParseQuery::whereDoesNotExists(const QString &key)
    {
        addConstraint(key, "$exists", false);
        return this;
    }

    ParseQuery * ParseQuery::whereFullText(const QString &key, const QString &text)
    {
        QJsonObject termObject, searchObject, textObject;
        termObject.insert("$term", text);
        searchObject.insert("$search", termObject);
        textObject.insert("$text", searchObject);
        _whereObject.insert(key, textObject);
        return this;
    }

    void ParseQuery::clear(const QString &key)
    {
        _whereObject.remove(key);
    }
}

