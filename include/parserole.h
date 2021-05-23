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
#ifndef CGPARSE_PARSEROLE_H
#define CGPARSE_PARSEROLE_H
#pragma once

#include "parseobject.h"
#include "parsequery.h"
#include "parserelation.h"
#include "parsetypes.h"

namespace cg
{
    class CGPARSE_API ParseRole : public ParseObject
    {
    public:
        static QSharedPointer<ParseRole> create();
        static QSharedPointer<ParseQuery<ParseRole>> query();

    public:
        ParseRole();
        ~ParseRole();

        QString name() const;
        void setName(const QString &name);

        QSharedPointer<ParseRelation<ParseRole>> roles();
        QSharedPointer<ParseRelation<ParseUser>> users();
    };
}

Q_DECLARE_METATYPE(cg::ParseRole);
Q_DECLARE_METATYPE(QSharedPointer<cg::ParseRole>);

#endif // CGPARSE_PARSEROLE_H
