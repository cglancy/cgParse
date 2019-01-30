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

        template <class T>
        static ParseQuery * createQuery()
        {
            return new ParseQuery(T::staticMetaObject->className());
        }

        template <class T>
        T * first() const
        {
            T * pObject = nullptr;
            if (_list.size() > 0)
                pObject = qobject_cast<T*>(_list.first());

            return pObject;
        }

        template <class T>
        const QList<T*> & results() const
        {
            QList<T*> objects;

            for (auto & pObject : _list)
                objects.append(qobject_cast<T*>(pObject));

            return objects;
        }

        void clearResults();
        ParseObject * firstObject() const;
        const QList<ParseObject*> & resultObjects() const;

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
        void getFinished(int errorCode);
        void findFinished(int count, int errorCode);
        void countFinished(int count, int errorCode);

    private:
        friend class ParseClientPrivate;
        void setResults(const QList<ParseObject*> &objects);

    private:
        QString _className;
        QList<ParseObject*> _list;
    };
}

#endif // CGPARSE_PARSEQUERY_H
