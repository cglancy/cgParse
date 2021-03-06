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

#include "parsetypes.h"
#include "parsequery.h"
#include <QVariant>

namespace cg
{
    class ParseObject;

    template <class T>
    class ParseRelation
    {
    public:
        ParseRelation(const QString &objectClassName, const QString &objectId, const QString &objectKey)
            : _objectClassName(objectClassName), _objectId(objectId), _objectKey(objectKey)
        {
        }

//        QSharedPointer<ParseQuery<T>> query()
//        {
//            return QSharedPointer<ParseQuery<T>>::create(_objectClassName, _objectId, _objectKey);
//        }

        void add(QSharedPointer<T> pObject) { _addList.append(pObject); }
        void remove(QSharedPointer<T> pObject) { _removeList.append(pObject); }

        const QList<QSharedPointer<T>> & addList() const { return _addList; }
        const QList<QSharedPointer<T>> & removeList() const { return _removeList; }

        void setValues(const QVariantMap &map)
        {
            if (map.contains(Parse::OperatorKey) && map.value(Parse::OperatorKey).toString() == Parse::AddRelationValue)
            {
                QVariantList list = map.value("objects").toList();
                for (auto & variant : list)
                {
                    QSharedPointer<ParseObject> pBaseObject = variant.value<QSharedPointer<ParseObject>>();
                    _addList.append(pBaseObject.template staticCast<T>());
                }
            }
            else if (map.contains(Parse::OperatorKey) && map.value(Parse::OperatorKey).toString() == Parse::RemoveRelationValue)
            {
                QVariantList list = map.value("objects").toList();
                for (auto & variant : list)
                {
                    QSharedPointer<ParseObject> pBaseObject = variant.value<QSharedPointer<ParseObject>>();
                    _removeList.append(pBaseObject.template staticCast<T>());
                }
            }
        }

        QVariantMap toMap() const
        {
            QVariantMap map;
            QVariantList list;

            if (_addList.size() > 0)
            {
                for (auto & pObject : _addList)
                {
                    QSharedPointer<ParseObject> pBaseObject = pObject.template staticCast<ParseObject>();
                    list.append(QVariant::fromValue(pBaseObject));
                }

                map.insert(Parse::OperatorKey, Parse::AddRelationValue);
                map.insert("objects", list);
            }
            else if (_removeList.size() > 0)
            {
                for (auto & pObject : _removeList)
                {
                    QSharedPointer<ParseObject> pBaseObject = pObject.template staticCast<ParseObject>();
                    list.append(QVariant::fromValue(pBaseObject));
                }

                map.insert(Parse::OperatorKey, Parse::RemoveRelationValue);
                map.insert("objects", list);
            }

            return map;
        }

    private:
        QString _objectClassName, _objectId, _objectKey;
        QList<QSharedPointer<T>> _addList, _removeList;
    };
}

#endif // CGPARSE_PARSERELATION_H
