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
#include "parserelation.h"
#include "parsequery.h"

namespace cg
{
    ParseRelation::ParseRelation(const QMetaObject *pMetaObject, const QString &className, 
        const QString &objectId, const QString &key, QObject *pParent)
        : QObject(pParent),
        _pMetaObject(pMetaObject),
        _className(className),
        _objectId(objectId),
        _key(key)
    {
    }

    ParseRelation::~ParseRelation()
    {
    }

    ParseQuery * ParseRelation::query()
    {
        return new ParseQuery(_pMetaObject, _className, _objectId, _key, this);
    }

    void ParseRelation::add(ParseObject *pObject)
    {
        _addList.append(pObject);
    }

    void ParseRelation::remove(ParseObject *pObject)
    {
        _removeList.append(pObject);
    }
}