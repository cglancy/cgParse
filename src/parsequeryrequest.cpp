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
#include "parsequeryrequest.h"
#include "parserequest.h"
#include "parsereply.h"

namespace cg
{
	ParseQueryRequest* ParseQueryRequest::_instance = nullptr;

	ParseQueryRequest::ParseQueryRequest()
	{
	}

	ParseQueryRequest::~ParseQueryRequest()
	{
	}

	ParseQueryRequest* ParseQueryRequest::get()
	{
		if (!_instance)
			_instance = new ParseQueryRequest;

		return _instance;
	}

	ParseReply* ParseQueryRequest::getObject(QSharedPointer<ParseQueryImpl> pQueryImpl, const QString& objectId, QNetworkAccessManager* pNam)
	{
		if (!pQueryImpl || pQueryImpl->className.isEmpty() || objectId.isEmpty())
		{
			return new ParseReply(ParseError::UnknownError);
		}

		pQueryImpl->results.clear();

		QString queryStr = QString("where={\"objectId\":\"%1\"}").arg(objectId);
		QUrlQuery urlQuery;
		urlQuery.setQuery(queryStr);

		ParseRequest request(ParseRequest::GetHttpMethod, "/classes/" + pQueryImpl->className);
		request.setUrlQuery(urlQuery);

		ParseReply* pReply = new ParseReply(request, pQueryImpl->className, pNam);
		connect(pReply, &ParseReply::preFinished, this, &ParseQueryRequest::getObjectFinished);
		_replyMap.insert(pReply, pQueryImpl);
		return pReply;
	}

	void ParseQueryRequest::getObjectFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		auto pImpl = _replyMap.take(pReply);

		if (!pReply->isError())
		{
			QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
			if (doc.isObject())
			{
				QJsonObject obj = doc.object();
				QJsonArray jsonArray = obj.value("results").toArray();
				setResults(pImpl, jsonArray);
			}
		}
	}

	ParseReply* ParseQueryRequest::findObjects(QSharedPointer<ParseQueryImpl> pQueryImpl, const QUrlQuery& urlQuery, QNetworkAccessManager* pNam)
	{
		if (!pQueryImpl || pQueryImpl->className.isEmpty())
		{
			return new ParseReply(ParseError::UnknownError);
		}

		pQueryImpl->results.clear();

		ParseRequest request(ParseRequest::GetHttpMethod, "/classes/" + pQueryImpl->className);
		request.setUrlQuery(urlQuery);

		ParseReply* pReply = new ParseReply(request, pQueryImpl->className, pNam);
		connect(pReply, &ParseReply::preFinished, this, &ParseQueryRequest::findObjectsFinished);
		_replyMap.insert(pReply, pQueryImpl);
		return pReply;
	}

	void ParseQueryRequest::findObjectsFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		auto pImpl = _replyMap.take(pReply);

		if (!pReply->isError())
		{
			QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
			if (doc.isObject())
			{
				QJsonObject obj = doc.object();
				QJsonArray jsonArray = obj.value("results").toArray();
				setResults(pImpl, jsonArray);
			}
		}
	}

	ParseReply* ParseQueryRequest::countObjects(QSharedPointer<ParseQueryImpl> pQueryImpl, const QUrlQuery& urlQuery, QNetworkAccessManager* pNam)
	{
		if (!pQueryImpl || pQueryImpl->className.isEmpty())
		{
			return new ParseReply(ParseError::UnknownError);
		}

		pQueryImpl->results.clear();
		pQueryImpl->countResult = 0;

		ParseRequest request(ParseRequest::GetHttpMethod, "/classes/" + pQueryImpl->className);
		request.setUrlQuery(urlQuery);

		ParseReply* pReply = new ParseReply(request, pQueryImpl->className, pNam);
		connect(pReply, &ParseReply::preFinished, this, &ParseQueryRequest::countObjectsFinished);
		_replyMap.insert(pReply, pQueryImpl);
		return pReply;
	}

	void ParseQueryRequest::countObjectsFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		auto pQueryImpl = _replyMap.take(pReply);

		if (pQueryImpl)
			pQueryImpl->countResult = pReply->count();
	}

	void ParseQueryRequest::setResults(QSharedPointer<ParseQueryImpl> pImpl, const QJsonArray& jsonArray)
	{
		if (!pImpl)
			return;

		pImpl->results.clear();

		for (auto jsonValue : jsonArray)
		{
			if (jsonValue.isObject())
			{
				QJsonObject jsonObject = jsonValue.toObject();
				QString objectId = jsonObject.value(Parse::ObjectIdKey).toString();

				if (!objectId.isEmpty())
				{
					ParseObject object = ParseObject::create(pImpl->className);
					object.setValues(ParseConvert::toVariantMap(jsonObject));
					object.clearDirtyState();
					pImpl->results.append(object);
				}
			}
		}
	}

}

