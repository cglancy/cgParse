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
#include "parseobjecthelper.h"
#include "parseobject.h"
#include "parseuser.h"
#include "parserequest.h"
#include "parsereply.h"
#include "parsefile.h"
#include "parseconvert.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>

namespace cg
{
    ParseObjectHelper::ParseObjectHelper()
    {
    }

    ParseObjectHelper::~ParseObjectHelper()
    {
    }

    bool ParseObjectHelper::collectDirtyChildren(QSharedPointer<ParseObject> pObject, QList<QSharedPointer<ParseFile>> &files, QList<QSharedPointer<ParseObject>> &objects)
    {
        for (auto & key : pObject->keys())
        {
            if (pObject->isDirty(key))
            {
                QVariant variant = pObject->value(key);
                if (variant.canConvert<QSharedPointer<ParseObject>>())
                    objects.append(variant.value<QSharedPointer<ParseObject>>());
                else if (variant.canConvert<QSharedPointer<ParseFile>>())
                    files.append(variant.value<QSharedPointer<ParseFile>>());
                else if (variant.canConvert<QVariantMap>())
                    collectDirtyChildren(variant.toMap(), files, objects);
                else if (variant.canConvert<QVariantList>())
                    collectDirtyChildren(variant.toList(), files, objects);
            }
        }

        return files.size() > 0 || objects.size() > 0;
    }

    void ParseObjectHelper::collectDirtyChildren(const QVariantMap &map, QList<QSharedPointer<ParseFile>> &files, QList<QSharedPointer<ParseObject>> &objects)
    {
        for (auto & key : map.keys())
        {
            QVariant variant = map.value(key);
            if (variant.canConvert<QSharedPointer<ParseObject>>())
                objects.append(variant.value<QSharedPointer<ParseObject>>());
            else if (variant.canConvert<QSharedPointer<ParseFile>>())
                files.append(variant.value<QSharedPointer<ParseFile>>());
            else if (variant.canConvert<QVariantMap>())
                collectDirtyChildren(variant.toMap(), files, objects);
            else if (variant.canConvert<QVariantList>())
                collectDirtyChildren(variant.toList(), files, objects);
        }
    }

    void ParseObjectHelper::collectDirtyChildren(const QVariantList &list, QList<QSharedPointer<ParseFile>> &files, QList<QSharedPointer<ParseObject>> &objects)
    {
        for (auto & variant : list)
        {
            if (variant.canConvert<QSharedPointer<ParseObject>>())
                objects.append(variant.value<QSharedPointer<ParseObject>>());
            else if (variant.canConvert<QSharedPointer<ParseFile>>())
                files.append(variant.value<QSharedPointer<ParseFile>>());
            else if (variant.canConvert<QVariantMap>())
                collectDirtyChildren(variant.toMap(), files, objects);
            else if (variant.canConvert<QVariantList>())
                collectDirtyChildren(variant.toList(), files, objects);
        }
    }

    void ParseObjectHelper::saveChildrenIfNeeded(QSharedPointer<ParseObject> pObject)
    {
        _objectsBeingSaved.insert(pObject);

        QList<QSharedPointer<ParseFile>> files;
        QList<QSharedPointer<ParseObject>> objects;
        collectDirtyChildren(pObject, files, objects);

        QList<QSharedPointer<ParseFile>> filesToSave;
        QList<QSharedPointer<ParseObject>> objectsToSave;

        // prevent infinite recursion
        for (auto & pDirtyObject : objects)
        {
            if (pDirtyObject && !_objectsBeingSaved.contains(pDirtyObject) && pDirtyObject->className() != "_User")
            {
                _objectsBeingSaved.insert(pDirtyObject);
                objectsToSave.append(pDirtyObject);
            }
        }

        _objectObjectsMap.insert(pObject, objectsToSave);

        for (auto & pFile : filesToSave)
        {
            QEventLoop loop;
            ParseReply *pFileReply = pFile->save();
            connect(pFileReply, &ParseReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
        }

        for (auto & pObject : objectsToSave)
        {
            QEventLoop loop;
            ParseReply *pObjectReply = pObject->save();
            connect(pObjectReply, &ParseReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
        }
    }

    ParseReply* ParseObjectHelper::createObject(QSharedPointer<ParseObject> pObject, QNetworkAccessManager* pNam)
    {
        saveChildrenIfNeeded(pObject);

        _pObject = pObject;
        QJsonObject object = ParseConvert::toJsonObject(pObject->toMap());
        QJsonDocument doc(object);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/classes/" + pObject->className(), content);

        ParseReply *pReply = new ParseReply(request, pNam);
        connect(pReply, &ParseReply::preFinished, this, &ParseObjectHelper::privateCreateObjectFinished);
        return pReply;
    }

    void ParseObjectHelper::privateCreateObjectFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        QSharedPointer<ParseObject> pObject = _pObject.lock();

        if (!pReply->isError() && pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isObject())
            {
                pObject->setValues(ParseConvert::toVariantMap(doc.object()));
                pObject->clearDirtyState();
            }
        }

        QList<QSharedPointer<ParseObject>> savedObjects = _objectObjectsMap.take(pObject);
        for (auto & pSavedObject : savedObjects)
        {
            _objectsBeingSaved.remove(pSavedObject);
        }

        _objectsBeingSaved.remove(pObject);
    }

    ParseReply* ParseObjectHelper::fetchObject(QSharedPointer<ParseObject> pObject, QNetworkAccessManager* pNam)
    {
        _pObject = pObject;
        ParseRequest request(ParseRequest::GetHttpMethod, "/classes/" + pObject->className() + "/" + pObject->objectId());
        ParseReply *pReply = new ParseReply(request, pNam);
        connect(pReply, &ParseReply::preFinished, this, &ParseObjectHelper::privateFetchObjectFinished);
        return pReply;
    }

    void ParseObjectHelper::privateFetchObjectFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        QSharedPointer<ParseObject> pObject = _pObject.lock();

        if (!pReply->isError() && pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isObject())
            {
                pObject->setValues(ParseConvert::toVariantMap(doc.object()));
                pObject->clearDirtyState();
            }
        }
    }

    ParseReply* ParseObjectHelper::updateObject(QSharedPointer<ParseObject> pObject, QNetworkAccessManager* pNam)
    {
        if (!pObject || pObject->objectId().isEmpty())
        {
            return new ParseReply(ParseError::UnknownError);
        }

        saveChildrenIfNeeded(pObject);

        _pObject = pObject;
        QJsonObject object = ParseConvert::toJsonObject(pObject->toMap());
        QJsonDocument doc(object);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PutHttpMethod, "/classes/" + pObject->className() + "/" + pObject->objectId(), content);
        ParseReply *pReply = new ParseReply(request, pNam);
        connect(pReply, &ParseReply::preFinished, this, &ParseObjectHelper::privateUpdateObjectFinished);
        return pReply;
    }

    void ParseObjectHelper::privateUpdateObjectFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        QSharedPointer<ParseObject> pObject = _pObject.lock();

        if (!pReply->isError() && pObject)
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isObject())
            {
                pObject->setValues(ParseConvert::toVariantMap(doc.object()));
                pObject->clearDirtyState();
            }
        }

        QList<QSharedPointer<ParseObject>> savedObjects = _objectObjectsMap.take(pObject);
        for (auto & pSavedObject : savedObjects)
        {
            _objectsBeingSaved.remove(pSavedObject);
        }

        _objectsBeingSaved.remove(pObject);
    }

    ParseReply* ParseObjectHelper::deleteObject(QSharedPointer<ParseObject> pObject, QNetworkAccessManager* pNam)
    {
        if (!pObject || pObject->objectId().isEmpty())
        {
            return new ParseReply(ParseError::UnknownError);
        }

        _pObject = pObject;
        ParseRequest request(ParseRequest::DeleteHttpMethod, "/classes/" + pObject->className() + "/" + pObject->objectId());
        return new ParseReply(request, pNam);
    }

    ParseReply* ParseObjectHelper::saveAll(const QList<QSharedPointer<ParseObject>>& objects, QNetworkAccessManager* pNam)
    {
        if (objects.size() == 0)
        {
            return new ParseReply(ParseError::UnknownError);
        }

        QString pathStr = "/classes/";
        QJsonArray requestsArray;

        for (auto & pObject : objects)
        {
            saveChildrenIfNeeded(pObject);

            QJsonObject requestObject;

            if (pObject->objectId().isEmpty())
            {
                QString apiPath = pathStr + pObject->className();
                requestObject.insert("method", "POST");
                requestObject.insert("path", apiPath);
            }
            else
            {
                QString apiPath = pathStr + pObject->className() + "/" + pObject->objectId();
                requestObject.insert("method", "PUT");
                requestObject.insert("path", apiPath);
            }

            QJsonObject bodyObject = ParseConvert::toJsonObject(pObject->toMap());
            requestObject.insert("body", bodyObject);
            requestsArray.append(requestObject);
        }

        QJsonObject contentObject;
        contentObject.insert("requests", requestsArray);
        QJsonDocument doc(contentObject);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/batch", content);
        ParseReply *pReply = new ParseReply(request, pNam);
        connect(pReply, &ParseReply::preFinished, this, &ParseObjectHelper::privateSaveAllFinished);
        _replyObjectListMap.insert(pReply, objects);
        return pReply;
    }

    void ParseObjectHelper::privateSaveAllFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        QList<QSharedPointer<ParseObject>> objects = _replyObjectListMap.take(pReply);

        if (!pReply->isError())
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isArray())
            {
                QJsonArray resultsArray = doc.array();
                for (int i = 0; i < resultsArray.size(); i++)
                {
                    QJsonObject arrayObject = resultsArray.at(i).toObject();
                    if (arrayObject.contains("success"))
                    {
                        QJsonObject successObject = arrayObject.value("success").toObject();
                        QSharedPointer<ParseObject> pObject = objects.at(i);
                        pObject->setValues(ParseConvert::toVariantMap(successObject));
                        pObject->clearDirtyState();
                    }
                }
            }
        }
    }

    ParseReply* ParseObjectHelper::deleteAll(const QList<QSharedPointer<ParseObject>>& objects, QNetworkAccessManager* pNam)
    {
        if (objects.size() == 0)
        {
            return new ParseReply(ParseError::UnknownError);
        }

        QString pathStr = "/classes/";
        QJsonArray requestsArray;

        for (auto & pObject : objects)
        {
            QString apiPath = pathStr + pObject->className() + "/" + pObject->objectId();
            QJsonObject requestObject;
            requestObject.insert("method", "DELETE");
            requestObject.insert("path", apiPath);
            requestsArray.append(requestObject);
        }

        QJsonObject contentObject;
        contentObject.insert("requests", requestsArray);
        QJsonDocument doc(contentObject);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/batch", content);
        return new ParseReply(request, pNam);
    }
}
