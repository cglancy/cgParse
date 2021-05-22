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
#ifndef CGPARSE_PARSEOBJECTHELPER_H
#define CGPARSE_PARSEOBJECTHELPER_H
#pragma once

#include "parse.h"
#include "parsetypes.h"
#include "parseobjectpointer.h"

#include <QObject>
#include <QWeakPointer>
#include <QMap>
#include <QSet>

class QNetworkReply;

namespace cg
{
    class ParseReply;
    class ParseObject;
    class ParseFile;

    class CGPARSE_API ParseObjectHelper : public QObject
    {
        Q_OBJECT
    public:
        ParseObjectHelper();
        ~ParseObjectHelper();

        ParseReply* createObject(QSharedPointer<ParseObject> pObject);
        ParseReply* fetchObject(QSharedPointer<ParseObject> pObject);
        ParseReply* updateObject(QSharedPointer<ParseObject> pObject);
        ParseReply* deleteObject(QSharedPointer<ParseObject> pObject);

        ParseReply* saveAll(const QList<QSharedPointer<ParseObject>> &objects);
        ParseReply* deleteAll(const QList<QSharedPointer<ParseObject>> &objects);

    private slots:
        void privateCreateObjectFinished();
        void privateFetchObjectFinished();
        void privateUpdateObjectFinished();
        void privateSaveAllFinished();

    private:
        void saveChildrenIfNeeded(QSharedPointer<ParseObject> pObject);
        bool collectDirtyChildren(QSharedPointer<ParseObject> pObject, QList<QSharedPointer<ParseFile>> &files, QList<QSharedPointer<ParseObject>> &objects);
        void collectDirtyChildren(const QVariantMap &map, QList<QSharedPointer<ParseFile>> &files, QList<QSharedPointer<ParseObject>> &objects);
        void collectDirtyChildren(const QVariantList &list, QList<QSharedPointer<ParseFile>> &files, QList<QSharedPointer<ParseObject>> &objects);

    private:
        QWeakPointer<ParseObject> _pObject;
        QMap<QSharedPointer<ParseObject>, QList<QSharedPointer<ParseObject>>> _objectObjectsMap;
        QSet<QSharedPointer<ParseObject>> _objectsBeingSaved;
        QMap<ParseReply*, QList<QSharedPointer<ParseObject>>> _replyObjectListMap;
    };
}

#endif // CGPARSE_PARSEOBJECTHELPER_H
