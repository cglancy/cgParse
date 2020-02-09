/**
* Copyright 2019 Charles Glancy (charles@glancyfamily.net)
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
#include "parse.h"
#include "parsequerymodel.h"
#include "parsereply.h"

#include <QJsonObject>
#include <QUrlQuery>

namespace cg
{
    ParseQueryModel::ParseQueryModel(QObject *parent)
        : ParseQueryModelBase(parent),
        _pHelper(new ParseQueryHelper())
    {
    }

    void ParseQueryModel::find()
    {
        findWithReply();
    }

    ParseReply * ParseQueryModel::findWithReply()
    {
        QUrlQuery urlQuery;

        if (_queryMap.contains("where") && _queryMap.value("where").canConvert<QVariantMap>())
        {
            QJsonObject whereObject = QJsonObject::fromVariantMap(_queryMap.value("where").toMap());
            QJsonDocument doc(whereObject);

            urlQuery.addQueryItem("where", doc.toJson(QJsonDocument::Compact));
        }

        if (_queryMap.contains("order") && _queryMap.value("order").canConvert<QStringList>())
        {
            QStringList orderList = _queryMap.value("order").toStringList();
            urlQuery.addQueryItem("order", orderList.join(','));
        }

        ParseReply *pReply = _pHelper->findObjects(className(), urlQuery);
        connect(pReply, &ParseReply::finished, this, &ParseQueryModel::findFinished);
        return pReply;
    }

    void ParseQueryModel::findFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        QList<ParseObjectPtr> objects = pReply->objects<ParseObject>();
        if (objects.size() > 0)
        {
            if (_objects.size() > 0)
            {
                beginRemoveRows(QModelIndex(), 0, _objects.size() - 1);
                _objects.clear();
                endRemoveRows();
            }

            beginInsertRows(QModelIndex(), 0, objects.size() - 1);
            _objects = objects;
            endInsertRows();
        }

        pReply->deleteLater();
    }
}