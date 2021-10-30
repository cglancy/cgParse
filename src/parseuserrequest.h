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
#ifndef CGPARSE_PARSEUSERREQUEST_H
#define CGPARSE_PARSEUSERREQUEST_H
#pragma once

#include <QObject>
#include "parsefile.h"
#include "parseuser.h"

class QNetworkAccessManager;

namespace cg
{
	class ParseReply;

	class ParseUserRequest : public QObject
	{
	public:
		static ParseUserRequest* get();

		ParseReply* login(const QString& username, const QString& password, QNetworkAccessManager* pNam = nullptr);
		ParseReply* logout(QNetworkAccessManager* pNam = nullptr);
		ParseReply* requestPasswordReset(const QString& email, QNetworkAccessManager* pNam = nullptr);
		ParseReply* become(const QString& sessionToken, QNetworkAccessManager* pNam = nullptr);
		ParseReply* signUp(const ParseUser& user, QNetworkAccessManager* pNam = nullptr);
		ParseReply* deleteUser(const ParseUser& user, QNetworkAccessManager* pNam = nullptr);

	private slots:
		void loginFinished();
		void logoutFinished();
		void signUpFinished();
		void deleteUserFinished();
		void becomeFinished();

	private:
		ParseUserRequest();
		~ParseUserRequest();

	private:
		static ParseUserRequest* _instance;
		QMap<ParseReply*, ParseUser> _replyUserMap;
	};
}

#endif // CGPARSE_PARSEUSERREQUEST_H
