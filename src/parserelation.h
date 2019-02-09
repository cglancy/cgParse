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
#ifndef CGPARSE_PARSERELATION_H
#define CGPARSE_PARSERELATION_H
#pragma once

#include "cgparse.h"
#include <QObject>

namespace cg
{
    class ParseObject;
    class ParseQuery;

    class CGPARSE_API ParseRelation : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(ParseRelation)

    public:
        ParseRelation(const QMetaObject *pMetaObject, const QString &className, const QString &objectId, 
            const QString &key, QObject *pParent = nullptr);
        ~ParseRelation();

        ParseQuery * query();
        void add(ParseObject *pObject);
        void remove(ParseObject *pObject);

        const QList<ParseObject*> & addList() const { return _addList; } 
        const QList<ParseObject*> & removeList() const { return _removeList; }

    private:
        const QMetaObject *_pMetaObject;
        QString _className, _objectId, _key;
        QList<ParseObject*> _addList, _removeList;
    };

    Q_DECLARE_METATYPE(ParseRelation*);
}

#endif // CGPARSE_PARSERELATION_H
