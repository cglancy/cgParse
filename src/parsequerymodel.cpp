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
#include "parsequerymodel.h"
#include "parsereply.h"

#include <QJsonObject>
#include <QUrlQuery>

namespace cg
{
    ParseQueryModel::ParseQueryModel(QObject *parent)
        : QAbstractListModel(parent),
		_pHelper(new ParseQueryHelper())
    {
        _keysList << Parse::ObjectIdKey;
        _keysList << Parse::CreatedAtKey;
        _keysList << Parse::UpdatedAtKey;
        updateHash();
    }

	ParseQueryModel::~ParseQueryModel()
	{
	}

    QString ParseQueryModel::className() const
    {
        return _className;
    }

    void ParseQueryModel::setClassName(const QString & className)
    {
        beginResetModel();

        _objects.clear();
        _className = className;

        endResetModel();

        emit classNameNotify();
    }

    QStringList ParseQueryModel::keys() const
    {
        return _keysList;
    }

    void ParseQueryModel::setKeys(const QStringList & keys)
    {
        beginResetModel();

        _objects.clear();
        _keysList = keys;
        updateHash();

        endResetModel();

        emit keysNotify();
    }

    QVariantMap ParseQueryModel::query() const
    {
        return _queryMap;
    }

    void ParseQueryModel::setQuery(const QVariantMap & queryMap)
    {
        _queryMap = queryMap;
        emit queryNotify();
    }

    void ParseQueryModel::updateHash()
    {
        _roleHash.clear();

        for (int i = 0; i < _keysList.size(); i++)
        {
            _roleHash.insert(i + Qt::UserRole, _keysList.at(i).toLocal8Bit());
        }
    }

    int ParseQueryModel::rowCount(const QModelIndex &parent) const
    {
        if (parent.isValid())
            return 0;

        return _objects.size();
    }

    QVariant ParseQueryModel::data(const QModelIndex &index, const QString &key) const
    {
        if (!index.isValid())
            return QVariant();

        if (index.row() < 0 || index.row() >= _objects.size())
            return QVariant();

        QSharedPointer<ParseObject> pObject = _objects.at(index.row());
        QVariant variant = pObject->value(key);

        return variant;
    }

    QVariant ParseQueryModel::data(const QModelIndex &index, int role) const
    {
        QString key = _roleHash.value(role);
        return data(index, key);
    }

    QHash<int, QByteArray> ParseQueryModel::roleNames() const
    {
        return _roleHash;
    }

	void ParseQueryModel::find()
	{
		findWithReply();
	}

	ParseReply * ParseQueryModel::findWithReply(QNetworkAccessManager* pNam)
	{
		QUrlQuery urlQuery;

		if (_queryMap.contains("where") && _queryMap.value("where").canConvert<QVariantMap>())
		{
			QJsonObject whereObject = QJsonObject::fromVariantMap(_queryMap.value("where").toMap());
			QJsonDocument doc(whereObject);

			urlQuery.addQueryItem("where", doc.toJson(QJsonDocument::Compact));
		}

		if (_queryMap.contains("order") && _queryMap.value("order").canConvert<QStringList>())
		{
			QStringList orderList = _queryMap.value("order").toStringList();
			urlQuery.addQueryItem("order", orderList.join(','));
		}

		if (_queryMap.contains("count") && _queryMap.value("count").canConvert<int>())
		{
			int count = _queryMap.value("count").toInt();
			urlQuery.addQueryItem("count", QString::number(count));
		}

		if (_queryMap.contains("limit") && _queryMap.value("limit").canConvert<int>())
		{
			int limit = _queryMap.value("limit").toInt();
			urlQuery.addQueryItem("limit", QString::number(limit));
		}

		if (_queryMap.contains("skip") && _queryMap.value("skip").canConvert<int>())
		{
			int skip = _queryMap.value("skip").toInt();
			urlQuery.addQueryItem("skip", QString::number(skip));
		}

		if (_queryMap.contains("keys") && _queryMap.value("keys").canConvert<QStringList>())
		{
			QStringList keysList = _queryMap.value("keys").toStringList();
			urlQuery.addQueryItem("keys", keysList.join(','));
		}

		if (_queryMap.contains("include") && _queryMap.value("include").canConvert<QStringList>())
		{
			QStringList includeList = _queryMap.value("include").toStringList();
			urlQuery.addQueryItem("include", includeList.join(','));
		}

		ParseReply *pReply = _pHelper->findObjects(className(), urlQuery, pNam);
		connect(pReply, &ParseReply::finished, this, &ParseQueryModel::findFinished);
		return pReply;
	}

	void ParseQueryModel::findFinished()
	{
		ParseReply *pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

        QList<QSharedPointer<ParseObject>> objects = pReply->objects<ParseObject>();
		if (objects.size() > 0)
		{
			if (_objects.size() > 0)
			{
				beginRemoveRows(QModelIndex(), 0, _objects.size() - 1);
				_objects.clear();
				endRemoveRows();
			}

			beginInsertRows(QModelIndex(), 0, objects.size() - 1);
			_objects = objects;
			endInsertRows();
		}

		pReply->deleteLater();
	}
}
