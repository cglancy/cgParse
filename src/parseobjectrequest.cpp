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
#include "parseobjectrequest.h"
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
    ParseObjectRequest* ParseObjectRequest::_instance = nullptr;

    ParseObjectRequest::ParseObjectRequest()
    {
    }

    ParseObjectRequest::~ParseObjectRequest()
    {
    }

    ParseObjectRequest* ParseObjectRequest::get()
    {
        if (!_instance)
            _instance = new ParseObjectRequest();

        return _instance;
    }

    bool ParseObjectRequest::collectDirtyChildren(const ParseObject& object, QList<ParseFile> &files, QList<ParseObject> &objects)
    {
        for (auto & key : object.keys())
        {
            if (object.isDirty(key))
            {
                QVariant variant = object.value(key);
                if (variant.canConvert<ParseObject>())
                    objects.append(variant.value<ParseObject>());
                else if (variant.canConvert<ParseFile>())
                    files.append(variant.value<ParseFile>());
                else if (variant.canConvert<QVariantMap>())
                    collectDirtyChildren(variant.toMap(), files, objects);
                else if (variant.canConvert<QVariantList>())
                    collectDirtyChildren(variant.toList(), files, objects);
            }
        }

        return files.size() > 0 || objects.size() > 0;
    }

    void ParseObjectRequest::collectDirtyChildren(const QVariantMap &map, QList<ParseFile> &files, QList<ParseObject> &objects)
    {
        for (auto & key : map.keys())
        {
            QVariant variant = map.value(key);
            if (variant.canConvert<ParseObject>())
                objects.append(variant.value<ParseObject>());
            else if (variant.canConvert<ParseFile>())
                files.append(variant.value<ParseFile>());
            else if (variant.canConvert<QVariantMap>())
                collectDirtyChildren(variant.toMap(), files, objects);
            else if (variant.canConvert<QVariantList>())
                collectDirtyChildren(variant.toList(), files, objects);
        }
    }

    void ParseObjectRequest::collectDirtyChildren(const QVariantList &list, QList<ParseFile> &files, QList<ParseObject> &objects)
    {
        for (auto & variant : list)
        {
            if (variant.canConvert<ParseObject>())
                objects.append(variant.value<ParseObject>());
            else if (variant.canConvert<ParseFile>())
                files.append(variant.value<ParseFile>());
            else if (variant.canConvert<QVariantMap>())
                collectDirtyChildren(variant.toMap(), files, objects);
            else if (variant.canConvert<QVariantList>())
                collectDirtyChildren(variant.toList(), files, objects);
        }
    }

    void ParseObjectRequest::saveChildrenIfNeeded(const ParseObject& object)
    {
        _objectsBeingSaved.insert(object);

        QList<ParseFile> files;
        QList<ParseObject> objects;
        collectDirtyChildren(object, files, objects);

        QList<ParseFile> filesToSave;
        QList<ParseObject> objectsToSave;

        // prevent infinite recursion
        for (auto & dirtyObject : objects)
        {
            if (!_objectsBeingSaved.contains(dirtyObject) && dirtyObject.className() != "_User")
            {
                _objectsBeingSaved.insert(dirtyObject);
                objectsToSave.append(dirtyObject);
            }
        }

        _objectObjectsMap.insert(object, objectsToSave);

        for (auto & file : filesToSave)
        {
            QEventLoop loop;
            ParseReply *pFileReply = file.save();
            connect(pFileReply, &ParseReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
        }

        for (auto & object : objectsToSave)
        {
            QEventLoop loop;
            ParseReply *pObjectReply = object.save();
            connect(pObjectReply, &ParseReply::finished, &loop, &QEventLoop::quit);
            loop.exec();
        }
    }

    ParseReply* ParseObjectRequest::createObject(const ParseObject& object, QNetworkAccessManager* pNam)
    {
        saveChildrenIfNeeded(object);

        QJsonObject jsonObject = ParseConvert::toJsonObject(object.toMap());
        QJsonDocument doc(jsonObject);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/classes/" + object.className(), content);

        ParseReply *pReply = new ParseReply(request, object.className(), pNam);
        connect(pReply, &ParseReply::preFinished, this, &ParseObjectRequest::privateCreateObjectFinished);
        _replyObjectMap.insert(pReply, object);
        return pReply;
    }

    void ParseObjectRequest::privateCreateObjectFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        ParseObject object = _replyObjectMap.take(pReply);

        if (!pReply->isError() && !object.isNull())
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isObject())
            {
                object.setValues(ParseConvert::toVariantMap(doc.object()));
                object.clearDirtyState();
            }
        }

        QList<ParseObject> savedObjects = _objectObjectsMap.take(object);
        for (auto & savedObject : savedObjects)
        {
            _objectsBeingSaved.remove(savedObject);
        }

        _objectsBeingSaved.remove(object);
    }

    ParseReply* ParseObjectRequest::fetchObject(const ParseObject& object, QNetworkAccessManager* pNam)
    {
        ParseRequest request(ParseRequest::GetHttpMethod, "/classes/" + object.className() + "/" + object.objectId());
        ParseReply *pReply = new ParseReply(request, object.className(), pNam);
        connect(pReply, &ParseReply::preFinished, this, &ParseObjectRequest::privateFetchObjectFinished);
        _replyObjectMap.insert(pReply, object);
        return pReply;
    }

    void ParseObjectRequest::privateFetchObjectFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        ParseObject object = _replyObjectMap.take(pReply);

        if (!pReply->isError() && !object.isNull())
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isObject())
            {
                object.setValues(ParseConvert::toVariantMap(doc.object()));
                object.clearDirtyState();
            }
        }
    }

    ParseReply* ParseObjectRequest::updateObject(const ParseObject& object, QNetworkAccessManager* pNam)
    {
        if (object.isNull() || object.objectId().isEmpty())
        {
            return new ParseReply(ParseError::UnknownError);
        }

        saveChildrenIfNeeded(object);

        QJsonObject jsonObject = ParseConvert::toJsonObject(object.toMap());
        QJsonDocument doc(jsonObject);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PutHttpMethod, "/classes/" + object.className() + "/" + object.objectId(), content);
        ParseReply *pReply = new ParseReply(request, object.className(), pNam);
        connect(pReply, &ParseReply::preFinished, this, &ParseObjectRequest::privateUpdateObjectFinished);
        _replyObjectMap.insert(pReply, object);
        return pReply;
    }

    void ParseObjectRequest::privateUpdateObjectFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        ParseObject object = _replyObjectMap.take(pReply);

        if (!pReply->isError() && !object.isNull())
        {
            QJsonDocument doc = QJsonDocument::fromJson(pReply->data());
            if (doc.isObject())
            {
                object.setValues(ParseConvert::toVariantMap(doc.object()));
                object.clearDirtyState();
            }
        }

        QList<ParseObject> savedObjects = _objectObjectsMap.take(object);
        for (auto & savedObject : savedObjects)
        {
            _objectsBeingSaved.remove(savedObject);
        }

        _objectsBeingSaved.remove(object);
    }

    ParseReply* ParseObjectRequest::deleteObject(const ParseObject& object, QNetworkAccessManager* pNam)
    {
        if (object.isNull() || object.objectId().isEmpty())
        {
            return new ParseReply(ParseError::UnknownError);
        }

        ParseRequest request(ParseRequest::DeleteHttpMethod, "/classes/" + object.className() + "/" + object.objectId());
        return new ParseReply(request, object.className(), pNam);
    }

    ParseReply* ParseObjectRequest::saveAll(const QList<ParseObject>& objects, QNetworkAccessManager* pNam)
    {
        if (objects.size() == 0)
        {
            return new ParseReply(ParseError::UnknownError);
        }

        QString pathStr = "/classes/";
        QJsonArray requestsArray;

        for (auto & object : objects)
        {
            saveChildrenIfNeeded(object);

            QJsonObject requestObject;

            if (object.objectId().isEmpty())
            {
                QString apiPath = pathStr + object.className();
                requestObject.insert("method", "POST");
                requestObject.insert("path", apiPath);
            }
            else
            {
                QString apiPath = pathStr + object.className() + "/" + object.objectId();
                requestObject.insert("method", "PUT");
                requestObject.insert("path", apiPath);
            }

            QJsonObject bodyObject = ParseConvert::toJsonObject(object.toMap());
            requestObject.insert("body", bodyObject);
            requestsArray.append(requestObject);
        }

        QJsonObject contentObject;
        contentObject.insert("requests", requestsArray);
        QJsonDocument doc(contentObject);
        QByteArray content = doc.toJson(QJsonDocument::Compact);

        ParseRequest request(ParseRequest::PostHttpMethod, "/batch", content);
        ParseReply *pReply = new ParseReply(request, pNam);
        connect(pReply, &ParseReply::preFinished, this, &ParseObjectRequest::privateSaveAllFinished);
        _replyObjectListMap.insert(pReply, objects);
        return pReply;
    }

    void ParseObjectRequest::privateSaveAllFinished()
    {
        ParseReply *pReply = qobject_cast<ParseReply*>(sender());
        if (!pReply)
            return;

        QList<ParseObject> objects = _replyObjectListMap.take(pReply);

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
                        ParseObject object = objects.at(i);
                        object.setValues(ParseConvert::toVariantMap(successObject));
                        object.clearDirtyState();
                    }
                }
            }
        }
    }

    ParseReply* ParseObjectRequest::deleteAll(const QList<ParseObject>& objects, QNetworkAccessManager* pNam)
    {
        if (objects.size() == 0)
        {
            return new ParseReply(ParseError::UnknownError);
        }

        QString pathStr = "/classes/";
        QJsonArray requestsArray;

        for (auto & object : objects)
        {
            QString apiPath = pathStr + object.className() + "/" +object.objectId();
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
