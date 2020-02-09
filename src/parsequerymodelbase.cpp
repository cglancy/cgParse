/**
* Copyright 2019 Charles Glancy (charles@glancyfamily.net)
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
#include "parse.h"
#include "parsequerymodelbase.h"
#include "parsereply.h"

#include <QJsonObject>
#include <QUrlQuery>

namespace cg
{
    ParseQueryModelBase::ParseQueryModelBase(QObject *parent)
        : QAbstractListModel(parent)
    {
        _keysList << Parse::ObjectIdKey;
        _keysList << Parse::CreatedAtKey;
        _keysList << Parse::UpdatedAtKey;
        updateHash();
    }

	ParseQueryModelBase::~ParseQueryModelBase()
	{
	}

    QString ParseQueryModelBase::className() const
    {
        return _className;
    }

    void ParseQueryModelBase::setClassName(const QString & className)
    {
        beginResetModel();

        _objects.clear();
        _className = className;

        endResetModel();

        emit classNameNotify();
    }

    QStringList ParseQueryModelBase::keys() const
    {
        return _keysList;
    }

    void ParseQueryModelBase::setKeys(const QStringList & keys)
    {
        beginResetModel();

        _objects.clear();
        _keysList = keys;
        updateHash();

        endResetModel();

        emit keysNotify();
    }

    QVariantMap ParseQueryModelBase::query() const
    {
        return _queryMap;
    }

    void ParseQueryModelBase::setQuery(const QVariantMap & queryMap)
    {
        _queryMap = queryMap;
        emit queryNotify();
    }

    void ParseQueryModelBase::updateHash()
    {
        _roleHash.clear();

        for (int i = 0; i < _keysList.size(); i++)
        {
            _roleHash.insert(i + Qt::UserRole, _keysList.at(i).toLocal8Bit());
        }
    }

    int ParseQueryModelBase::rowCount(const QModelIndex &parent) const
    {
        if (parent.isValid())
            return 0;

        return _objects.size();
    }

    QVariant ParseQueryModelBase::data(const QModelIndex &index, const QString &key) const
    {
        if (!index.isValid())
            return QVariant();

        if (index.row() < 0 || index.row() >= _objects.size())
            return QVariant();

        ParseObjectPtr pObject = _objects.at(index.row());
        QVariant variant = pObject->value(key);

        return variant;
    }

    QVariant ParseQueryModelBase::data(const QModelIndex &index, int role) const
    {
        QString key = _roleHash.value(role);
        return data(index, key);
    }

    QHash<int, QByteArray> ParseQueryModelBase::roleNames() const
    {
        return _roleHash;
    }
}