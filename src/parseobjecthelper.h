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
#include "parsehelperbase.h"
#include "parsetypes.h"
#include "parseresult.h"

#include <QObject>
#include <QWeakPointer>
#include <QMap>

class QNetworkReply;

namespace cg
{
    class CGPARSE_API ParseObjectHelper : public QObject, public ParseHelperBase
    {
        Q_OBJECT
    public:
        ParseObjectHelper();
        ~ParseObjectHelper();

    public slots:
        void createObject(ParseObjectPtr pObject);
        void fetchObject(ParseObjectPtr pObject);
        void updateObject(ParseObjectPtr pObject);
        void deleteObject(ParseObjectPtr pObject);

        void createAll(const QList<ParseObjectPtr> &objects);
        void updateAll(const QList<ParseObjectPtr> &objects);
        void deleteAll(const QList<ParseObjectPtr> &objects);

    private:
        void privateCreateObjectFinished();
        void privateFetchObjectFinished();
        void privateUpdateObjectFinished();
        void privateDeleteObjectFinished();

        void privateCreateAllFinished();
        void privateUpdateAllFinished();
        void privateDeleteAllFinished();

    signals:
        void createObjectFinished(int error);
        void fetchObjectFinished(int error);
        void updateObjectFinished(int error);
        void deleteObjectFinished(int error);

        void createAllFinished(int error);
        void updateAllFinished(int error);
        void deleteAllFinished(int error);

    private:
        QWeakPointer<ParseObject> _pObject;
        QMap<QNetworkReply*, QList<ParseObjectPtr>> _replyObjectListMap;
    };
}

#endif // CGPARSE_PARSEOBJECTHELPER_H
