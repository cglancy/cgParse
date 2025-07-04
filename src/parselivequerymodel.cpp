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
#include "parselivequerymodel.h"
#include "parselivequeryclient.h"
#include "parselivequerysubscription.h"

#include <QJsonObject>
#include <QUrlQuery>

namespace cg
{
    ParseLiveQueryModel::ParseLiveQueryModel(QObject *parent)
        : ParseQueryModel(parent),
		m_pSubscription(nullptr)
    {
    }

	ParseLiveQueryModel::~ParseLiveQueryModel()
	{
		unsubscribe();
	}

	void ParseLiveQueryModel::subscribe()
	{
		if (m_pSubscription)
			return;

		find();

		ParseLiveQueryClient *pClient = ParseLiveQueryClient::get();
		if (!pClient)
			return;

		QJsonObject subscriptionQueryObject;
		subscriptionQueryObject.insert("className", _className);

		if (_queryMap.contains("where") && _queryMap.value("where").typeId() == QMetaType::QVariantMap)
		{
			QJsonObject whereObject = QJsonObject::fromVariantMap(_queryMap.value("where").toMap());
			subscriptionQueryObject.insert("where", whereObject);
		}

		m_pSubscription = pClient->subscribe(subscriptionQueryObject);
		connect(m_pSubscription, &ParseLiveQuerySubscription::subscribed, this, &ParseLiveQueryModel::subscribed);
		connect(m_pSubscription, &ParseLiveQuerySubscription::createEvent, this, &ParseLiveQueryModel::createEvent);
		connect(m_pSubscription, &ParseLiveQuerySubscription::enterEvent, this, &ParseLiveQueryModel::enterEvent);
		connect(m_pSubscription, &ParseLiveQuerySubscription::leaveEvent, this, &ParseLiveQueryModel::leaveEvent);
		connect(m_pSubscription, &ParseLiveQuerySubscription::updateEvent, this, &ParseLiveQueryModel::updateEvent);
		connect(m_pSubscription, &ParseLiveQuerySubscription::deleteEvent, this, &ParseLiveQueryModel::deleteEvent);
	}

	void ParseLiveQueryModel::unsubscribe()
	{
		if (m_pSubscription)
		{
			disconnect(m_pSubscription, &ParseLiveQuerySubscription::subscribed, this, &ParseLiveQueryModel::subscribed);
			disconnect(m_pSubscription, &ParseLiveQuerySubscription::createEvent, this, &ParseLiveQueryModel::createEvent);
			disconnect(m_pSubscription, &ParseLiveQuerySubscription::enterEvent, this, &ParseLiveQueryModel::enterEvent);
			disconnect(m_pSubscription, &ParseLiveQuerySubscription::leaveEvent, this, &ParseLiveQueryModel::leaveEvent);
			disconnect(m_pSubscription, &ParseLiveQuerySubscription::updateEvent, this, &ParseLiveQueryModel::updateEvent);
			disconnect(m_pSubscription, &ParseLiveQuerySubscription::deleteEvent, this, &ParseLiveQueryModel::deleteEvent);

			m_pSubscription->unsubscribe();
			delete m_pSubscription;
			m_pSubscription = nullptr;
		}
	}

	void ParseLiveQueryModel::createEvent(const ParseObject& object)
	{
		beginInsertRows(QModelIndex(), _objects.size(), _objects.size());
		_objects.append(object);
		endInsertRows();
	}

	void ParseLiveQueryModel::enterEvent(const ParseObject& object)
	{
		beginInsertRows(QModelIndex(), _objects.size(), _objects.size());
		_objects.append(object);
		endInsertRows();
	}

	void ParseLiveQueryModel::leaveEvent(const ParseObject& object)
	{
		int index = indexOf(object);
		if (index >= 0 && index < _objects.size())
		{
			beginRemoveRows(QModelIndex(), index, index);
			_objects.removeAt(index);
			endRemoveRows();
		}
	}

	void ParseLiveQueryModel::updateEvent(const ParseObject& object)
	{
		int index = indexOf(object);
		if (index >= 0 && index < _objects.size())
		{
			emit dataChanged(createIndex(index, 0), createIndex(index, _keysList.size()));
		}
	}

	void ParseLiveQueryModel::deleteEvent(const ParseObject& object)
	{
		int index = indexOf(object);
		if (index >= 0 && index < _objects.size())
		{
			beginRemoveRows(QModelIndex(), index, index);
			_objects.removeAt(index);
			endRemoveRows();
		}
	}

	int ParseLiveQueryModel::indexOf(const ParseObject& object) const
	{
		for (int i = 0; i < _objects.size(); i++)
		{
			if (object.hasSameId(_objects.at(i)))
				return i;
		}

		return -1;
	}


 }