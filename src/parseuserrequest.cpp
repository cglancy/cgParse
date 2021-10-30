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
#include "parseuserrequest.h"
#include "parserequest.h"
#include "parsereply.h"
#include "parsefileimpl.h"

namespace cg
{
	ParseUserRequest* ParseUserRequest::_instance = nullptr;

	ParseUserRequest::ParseUserRequest()
	{
	}

	ParseUserRequest::~ParseUserRequest()
	{
	}

	ParseUserRequest* ParseUserRequest::get()
	{
		if (!_instance)
			_instance = new ParseUserRequest;

		return _instance;
	}

	ParseReply* ParseUserRequest::login(const QString& username, const QString& password, QNetworkAccessManager* pNam)
	{
		ParseRequest request(ParseRequest::GetHttpMethod, "/login");
		request.setHeader("X-Parse-Revocable-Session", "1");

		QUrlQuery query;
		query.addQueryItem("username", username);
		query.addQueryItem("password", password);
		request.setUrlQuery(query);

		ParseReply* pParseReply = new ParseReply(request, pNam);
		connect(pParseReply, &ParseReply::preFinished, this, &ParseUserRequest::loginFinished);
		return pParseReply;
	}

	void ParseUserRequest::loginFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		if (!pReply->isError() && pReply->statusCode() == 200)
		{
			ParseUser::_currentUser = pReply->user();
		}
	}

	ParseReply* ParseUserRequest::logout(QNetworkAccessManager* pNam)
	{
		QString sessionToken;
		ParseUser user = ParseUser::currentUser();
		if (!user.isNull())
			sessionToken = user.sessionToken();

		ParseRequest request(ParseRequest::PostHttpMethod, "/logout");
		request.setHeader("X-Parse-Session-Token", sessionToken.toUtf8());

		ParseReply* pParseReply = new ParseReply(request, pNam);
		connect(pParseReply, &ParseReply::preFinished, this, &ParseUserRequest::logoutFinished);
		return pParseReply;
	}

	void ParseUserRequest::logoutFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		if (!pReply->isError())
		{
			ParseUser::_currentUser = ParseUser();
		}
	}

	ParseReply* ParseUserRequest::requestPasswordReset(const QString& email, QNetworkAccessManager* pNam)
	{
		QByteArray content;
		QJsonObject jsonObject;
		jsonObject.insert("email", email);
		QJsonDocument doc(jsonObject);
		content = doc.toJson(QJsonDocument::Compact);

		ParseRequest request(ParseRequest::PostHttpMethod, "/requestPasswordReset", content);
		return new ParseReply(request, pNam);
	}

	ParseReply* ParseUserRequest::become(const QString& sessionToken, QNetworkAccessManager* pNam)
	{
		ParseRequest request(ParseRequest::GetHttpMethod, "/users/me");
		request.setHeader("X-Parse-Session-Token", sessionToken.toUtf8());

		ParseReply* pParseReply = new ParseReply(request, pNam);
		QObject::connect(pParseReply, &ParseReply::preFinished, this, &ParseUserRequest::becomeFinished);
		return pParseReply;
	}

	void ParseUserRequest::becomeFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		if (!pReply->isError())
		{
			ParseUser::_currentUser = pReply->user();
		}
	}

	ParseReply* ParseUserRequest::signUp(const ParseUser& user, QNetworkAccessManager* pNam)
	{
		QJsonObject object = ParseConvert::toJsonObject(user.toMap());
		QJsonDocument doc(object);
		QByteArray content = doc.toJson(QJsonDocument::Compact);

		ParseRequest request(ParseRequest::PostHttpMethod, "/users", content);
		request.setHeader("X-Parse-Revocable-Session", "1");

		ParseReply* pParseReply = new ParseReply(request, pNam);
		connect(pParseReply, &ParseReply::preFinished, this, &ParseUserRequest::signUpFinished);
		_replyUserMap.insert(pParseReply, user);
		return pParseReply;
	}

	void ParseUserRequest::signUpFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		ParseUser user = _replyUserMap.take(pReply);

		if (!pReply->isError() && pReply->statusCode() == 201 && !user.isNull())
		{
			QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
			if (doc.isObject())
			{
				user.setValues(ParseConvert::toVariantMap(doc.object()));
				user.clearDirtyState();

				ParseUser::_currentUser = user;
			}
		}
	}

	ParseReply* ParseUserRequest::deleteUser(const ParseUser& user, QNetworkAccessManager* pNam)
	{
		ParseRequest request(ParseRequest::DeleteHttpMethod, "/users/" + user.objectId());
		request.setHeader("X-Parse-Session-Token", user.sessionToken().toUtf8());

		ParseReply* pParseReply = new ParseReply(request, pNam);
		connect(pParseReply, &ParseReply::preFinished, this, &ParseUserRequest::deleteUserFinished);
		_replyUserMap.insert(pParseReply, user);
		return pParseReply;
	}

	void ParseUserRequest::deleteUserFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		ParseUser user = _replyUserMap.take(pReply);

		if (!pReply->isError() && !user.isNull())
		{
			if (user.hasSameId(ParseUser::currentUser()))
				ParseUser::_currentUser = ParseUser();
		}
	}
}

