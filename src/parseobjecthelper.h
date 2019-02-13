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

#include "cgparse.h"
#include "parsetypes.h"
#include "parsereply.h"

#include <QObject>

namespace cg
{
    class CGPARSE_API ParseObjectHelper : public QObject
    {
        Q_OBJECT
    public:
        ParseObjectHelper(ParseObjectPtr pObject);
        ~ParseObjectHelper();

    public slots:
        void createObject(ParseObjectPtr pObject);
        void fetchObject(ParseObjectPtr pObject);
        void updateObject(ParseObjectPtr pObject);
        void deleteObject(ParseObjectPtr pObject);

    private:
        void privateCreateObjectFinished();
        void privateFetchObjectFinished();
        void privateUpdateObjectFinished();
        void privateDeleteObjectFinished();

    signals:
        void createObjectFinished(int status);
        void fetchObjectFinished(int status);
        void updateObjectFinished(int status);
        void deleteObjectFinished(int status);

    private:
        QWeakPointer<ParseObject> _pObject;
    };
}

#endif // CGPARSE_PARSEOBJECTHELPER_H
