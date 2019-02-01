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
#ifndef CGPARSE_PARSEQUERY_H
#define CGPARSE_PARSEQUERY_H
#pragma once

#include "cgparse.h"
#include "parseclient.h"
#include <QObject>
#include <QList>
#include <QUrlQuery>

namespace cg
{
    class ParseObject;

    class CGPARSE_API ParseQuery : public QObject
    {
        Q_OBJECT
    public:
        ParseQuery(const QString &className);
        ~ParseQuery();

        template <class T>
        static ParseQuery * createQuery()
        {
            return new ParseQuery(T::staticMetaObject->className());
        }

        QString className() const { return _className; }
        ParseQuery * orderByAscending(const QByteArray &propertyName);
        ParseQuery * orderByDescending(const QByteArray &propertyName);
        ParseQuery * setLimit(int limit);
        ParseQuery * setSkip(int skip);

        QUrlQuery urlQuery() const;

    public slots:
        void get(const QString &objectId);
        void find();
        void count();

    signals:
        void getFinished(ParseObject *pObject, int errorCode);
        void findFinished(const QList<ParseObject*> &objects, int errorCode);
        void countFinished(int count, int errorCode);

    private:
        friend class ParseClientPrivate;

    private:
        QString _className;
    };
}

#endif // CGPARSE_PARSEQUERY_H
