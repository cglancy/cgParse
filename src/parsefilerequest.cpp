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
#include "parsefilerequest.h"
#include "parserequest.h"
#include "parsereply.h"
#include "parsefileimpl.h"

namespace cg
{
	ParseFileRequest* ParseFileRequest::_instance = nullptr;

	ParseFileRequest::ParseFileRequest()
	{
	}

	ParseFileRequest::~ParseFileRequest()
	{
	}

	ParseFileRequest* ParseFileRequest::get()
	{
		if (!_instance)
			_instance = new ParseFileRequest;

		return _instance;
	}

	ParseReply* ParseFileRequest::saveFile(const ParseFile& file, QNetworkAccessManager* pNam)
	{
		ParseRequest request(ParseRequest::PostHttpMethod, "/files/" + file.name(), file.data(), file.contentType());
		ParseReply* pReply = new ParseReply(request, pNam);
		connect(pReply, &ParseReply::preFinished, this, &ParseFileRequest::saveFileFinished);
		_replyFileMap.insert(pReply, file);
		return pReply;
	}

	void ParseFileRequest::saveFileFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		ParseFile file = _replyFileMap.take(pReply);

		if (!pReply->isError() && pReply->statusCode() == 201)
		{
			QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
			if (doc.isObject())
			{
				QJsonObject obj = doc.object();
				file.setUrl(obj.value("url").toString());
				file.setName(obj.value("name").toString());
			}
		}

		pReply->deleteLater();
	}

	ParseReply* ParseFileRequest::fetchFile(const ParseFile& file, QNetworkAccessManager* pNam)
	{
		ParseRequest request(ParseRequest::GetHttpMethod, file.url());
		ParseReply* pReply = new ParseReply(request, pNam);
		connect(pReply, &ParseReply::preFinished, this, &ParseFileRequest::fetchFileFinished);
		_replyFileMap.insert(pReply, file);
		return pReply;
	}

	void ParseFileRequest::fetchFileFinished()
	{
		ParseReply* pReply = qobject_cast<ParseReply*>(sender());
		if (!pReply)
			return;

		ParseFile file = _replyFileMap.take(pReply);

		if (!pReply->isError() && pReply->statusCode() == 200)
		{
			file._pImpl->data = pReply->data();
		}
	}

	ParseReply* ParseFileRequest::deleteFile(const QString& urlStr, const QString& masterKey, QNetworkAccessManager* pNam)
	{
		QUrl url(urlStr);
		ParseRequest request(ParseRequest::DeleteHttpMethod, "/files/" + url.fileName());
		request.removeHeader("X-Parse-REST-API-Key");
		request.setHeader("X-Parse-Master-Key", masterKey.toUtf8());
		return new ParseReply(request, pNam);
	}
}

