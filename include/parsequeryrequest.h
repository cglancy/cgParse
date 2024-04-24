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
#ifndef CGPARSE_PARSEQUERYREQUEST_H
#define CGPARSE_PARSEQUERYREQUEST_H
#pragma once

#include <QObject>
#include <QSharedPointer>
#include "parse.h"
#include "parsequeryimpl.h"

class QNetworkAccessManager;
class QUrlQuery;

namespace cg
{
	class ParseReply;

	class CGPARSE_API ParseQueryRequest : public QObject
	{
	public:
		static ParseQueryRequest* get();

		// ParseQuery
		ParseReply* getObject(QSharedPointer<ParseQueryImpl> pQueryImpl, const QString& objectId, QNetworkAccessManager* pNam);
		ParseReply* findObjects(QSharedPointer<ParseQueryImpl> pQueryImpl, const QUrlQuery& urlQuery, QNetworkAccessManager* pNam);
		ParseReply* countObjects(QSharedPointer<ParseQueryImpl> pQueryImpl, const QUrlQuery& urlQuery, QNetworkAccessManager* pNam);

	private slots:
		void getObjectFinished();
		void findObjectsFinished();
		void countObjectsFinished();

	private:
		ParseQueryRequest();
		~ParseQueryRequest();

		void setResults(QSharedPointer<ParseQueryImpl>, const QJsonArray& jsonArray);
		static QString classPath(const QString& className);

	private:
		static ParseQueryRequest* _instance;
		QMap<ParseReply*, QSharedPointer<ParseQueryImpl>> _replyMap;
	};
}

#endif // CGPARSE_PARSEQUERYREQUEST_H
