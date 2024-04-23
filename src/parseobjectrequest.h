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
#ifndef CGPARSE_PARSEOBJECTREQUEST_H
#define CGPARSE_PARSEOBJECTREQUEST_H
#pragma once

#include "parse.h"
#include "parseobjectpointer.h"

#include <QObject>
#include <QMap>
#include <QSet>

class QNetworkReply;
class QNetworkAccessManager;

namespace cg
{
    class ParseReply;
    class ParseFile;
    class ParseObject;

    class CGPARSE_API ParseObjectRequest : public QObject
    {
        Q_OBJECT
    private:
        ParseObjectRequest();
        ~ParseObjectRequest();

    public:
        static ParseObjectRequest* get();

        ParseReply* createObject(const ParseObject &object, QNetworkAccessManager* pNam);
        ParseReply* fetchObject(const ParseObject& object, QNetworkAccessManager* pNam);
        ParseReply* updateObject(const ParseObject& object, QNetworkAccessManager* pNam);
        ParseReply* deleteObject(const ParseObject& object, QNetworkAccessManager* pNam);

        ParseReply* saveAll(const QList<ParseObject> &objects, QNetworkAccessManager* pNam);
        ParseReply* deleteAll(const QList<ParseObject> &objects, QNetworkAccessManager* pNam);

    private slots:
        void privateCreateObjectFinished();
        void privateFetchObjectFinished();
        void privateUpdateObjectFinished();
        void privateSaveAllFinished();

    private:
        void saveChildrenIfNeeded(const ParseObject& object);
        bool collectDirtyChildren(const ParseObject& object, QList<ParseFile> &files, QList<ParseObject> &objects);
        void collectDirtyChildren(const QVariantMap &map, QList<ParseFile> &files, QList<ParseObject> &objects);
        void collectDirtyChildren(const QVariantList &list, QList<ParseFile> &files, QList<ParseObject> &objects);
        static QString classPath(const QString& className);

    private:
        static ParseObjectRequest* _instance;
        QMap<ParseObject, QList<ParseObject>> _objectObjectsMap;
        QSet<ParseObject> _objectsBeingSaved;
        QMap<ParseReply*, QList<ParseObject>> _replyObjectListMap;
        QMap<ParseReply*, ParseObject> _replyObjectMap;
    };
}

#endif // CGPARSE_PARSEOBJECTREQUEST_H
