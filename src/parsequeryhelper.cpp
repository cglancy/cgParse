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
#include "parsequeryhelper.h"
#include "parserequest.h"
#include "parsereply.h"

namespace cg
{
	ParseQueryHelper* ParseQueryHelper::_instance = nullptr;

	ParseQueryHelper::ParseQueryHelper()
	{
	}

	ParseQueryHelper::~ParseQueryHelper()
	{
	}

	ParseQueryHelper* ParseQueryHelper::get()
	{
		if (!_instance)
			_instance = new ParseQueryHelper;

		return _instance;
	}

	ParseReply* ParseQueryHelper::getObject(QSharedPointer<ParseQueryImpl> pQueryImpl, const QString& objectId, QNetworkAccessManager* pNam)
	{
		if (!pQueryImpl || pQueryImpl->className.isEmpty() || objectId.isEmpty())
		{
			return new ParseReply(ParseError::UnknownError);
		}

		QString queryStr = QString("where={\"objectId\":\"%1\"}").arg(objectId);
		QUrlQuery urlQuery;
		urlQuery.setQuery(queryStr);

		ParseRequest request(ParseRequest::GetHttpMethod, "/classes/" + pQueryImpl->className);
		request.setUrlQuery(urlQuery);

		ParseReply* pReply = new ParseReply(request, pNam);
		connect(pReply, &ParseReply::finished, this, &ParseQueryHelper::getObjectFinished);
		_replyMap.insert(pReply, pQueryImpl);
		return pReply;
	}

	void ParseQueryHelper::getObjectFinished()
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
				pImpl->jsonArray = obj.value("results").toArray();
			}
		}
	}

	ParseReply* ParseQueryHelper::findObjects(QSharedPointer<ParseQueryImpl> pQueryImpl, const QUrlQuery& urlQuery, QNetworkAccessManager* pNam)
	{
		if (!pQueryImpl || pQueryImpl->className.isEmpty())
		{
			return new ParseReply(ParseError::UnknownError);
		}

		ParseRequest request(ParseRequest::GetHttpMethod, "/classes/" + pQueryImpl->className);
		request.setUrlQuery(urlQuery);

		ParseReply* pReply = new ParseReply(request, pNam);
		connect(pReply, &ParseReply::finished, this, &ParseQueryHelper::findObjectsFinished);
		_replyMap.insert(pReply, pQueryImpl);
		return pReply;
	}

	void ParseQueryHelper::findObjectsFinished()
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
				pImpl->jsonArray = obj.value("results").toArray();
			}
		}
	}

	ParseReply* ParseQueryHelper::countObjects(const QString& className, const QUrlQuery& urlQuery, QNetworkAccessManager* pNam)
	{
		if (className.isEmpty())
		{
			return new ParseReply(ParseError::UnknownError);
		}

		ParseRequest request(ParseRequest::GetHttpMethod, "/classes/" + className);
		request.setUrlQuery(urlQuery);

		return new ParseReply(request, pNam);
	}

}

