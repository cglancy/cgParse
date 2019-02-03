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
#include "parseutil.h"
#include "parseobject.h"
#include "parsefile.h"

#include <QJsonObject>

namespace cg
{
    QVariant ParseUtil::toVariant(const QJsonValue &jsonValue, ParseObject *pParent)
    {
        QVariant variant;

        if (jsonValue.isObject())
        {
            QJsonObject object = jsonValue.toObject();
            QString typeStr = object.value("__type").toString();
            if (typeStr == "Pointer")
            {
                QString className = object.value("className").toString();
                QString objectId = object.value(ParseObject::ObjectIdKey).toString();

                ParseObject *pObject = ParseObject::createWithoutData(className, objectId);
                if (pObject && pParent)
                    pObject->setParent(pParent);
                variant = QVariant::fromValue<ParseObject*>(pObject);
            }
            else if (typeStr == "File")
            {
                ParseFile *pFile = new ParseFile(pParent);
                pFile->setName(object.value("name").toString());
                pFile->setUrl(object.value("url").toString());
                variant = QVariant::fromValue<ParseFile*>(pFile);
            }
            else if (typeStr == "Date")
            {
                QString isoStr = object.value("iso").toString();
                QDateTime dateTime = QDateTime::fromString(isoStr, Qt::ISODateWithMs);
                variant = dateTime;
            }
        }
        else
        {
            variant = jsonValue.toVariant();
        }

        return variant;
    }

    QVariantMap ParseUtil::toVariantMap(const QJsonObject &jsonObject, ParseObject *pParent)
    {
        QVariantMap map;

        for (auto & key : jsonObject.keys())
        {
            QVariant variant = toVariant(jsonObject.value(key), pParent);
            map.insert(key, variant);
        }

        return map;
    }

    QJsonValue ParseUtil::toJsonValue(const QVariant &variant)
    {
        QJsonValue jsonValue;

        if (variant.canConvert<ParseObject*>())
        {
            ParseObject *pObject = qvariant_cast<ParseObject*>(variant);
            if (pObject)
            {
                QJsonObject jsonObject;
                jsonObject.insert("__type", "Pointer");
                jsonObject.insert("className", pObject->className());
                jsonObject.insert(ParseObject::ObjectIdKey, pObject->objectId());
                jsonValue = jsonObject;
            }
        }
        else if (variant.canConvert<ParseFile*>())
        {
            ParseFile *pFile = qvariant_cast<ParseFile*>(variant);
            if (pFile)
            {
                QJsonObject jsonObject;
                jsonObject.insert("__type", "File");
                jsonObject.insert("name", pFile->name());
                jsonObject.insert("url", pFile->url());
                jsonValue = jsonObject;
            }
        }
        else if (variant.type() == QVariant::DateTime)
        {
            QJsonObject jsonObject;
            jsonObject.insert("__type", "Date");
            jsonObject.insert("iso", variant.toDateTime().toString(Qt::ISODateWithMs));
            jsonValue = jsonObject;
        }
        else
        {
            jsonValue = QJsonValue::fromVariant(variant);
        }

        return jsonValue;
    }
}