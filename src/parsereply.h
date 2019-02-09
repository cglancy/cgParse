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
#ifndef CGPARSE_PARSEREPLY_H
#define CGPARSE_PARSEREPLY_H
#pragma once

#include <QObject>
#include <QByteArray>

class QNetworkReply;

namespace cg
{
	class ParseReply : public QObject
	{
		Q_OBJECT
	public:
		ParseReply(QNetworkReply *pReply);
		virtual ~ParseReply();

        QByteArray data() const { return _data; }
        int statusCode() const { return _statusCode; }
        int errorCode() const { return _errorCode; }
        QString errorMessage() const { return _errorMessage; }

    signals:
        void finished();

    private slots:
        void requestFinished();

    private:
        static int statusCode(QNetworkReply *pReply);
        static int errorCode(QNetworkReply *pReply);
        static bool isError(int statusCode);

    private:
        QNetworkReply *_pReply;
        QByteArray _data;
        int _errorCode, _statusCode;
        QString _errorMessage;
    };
}

#endif // CGPARSE_PARSEREPLY_H
