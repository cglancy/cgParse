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
#ifndef CGPARSE_PARSEACL_H
#define CGPARSE_PARSEACL_H
#pragma once

#include "parse.h"
#include <QVariant>

namespace cg
{
    class ParseUser;

    class CGPARSE_API ParseACL
    {
    public:
        static ParseACL defaultACL();
        static void setDefaultACL(const ParseACL &acl, bool withAccessForCurrentUser);

    public:
        ParseACL();
        ParseACL(const QVariantMap& map);
        ParseACL(const ParseUser& user);
        ParseACL(const ParseACL &acl);
        ~ParseACL();

        bool publicReadAccess() const;
        bool publicWriteAccess() const;
        bool readAccess(const ParseUser& user) const;
        bool readAccess(const QString& userId) const;
        bool writeAccess(const ParseUser& pUser) const;
        bool writeAccess(const QString& userId) const;

        void setPublicReadAccess(bool allowed);
        void setPublicWriteAccess(bool allowed);
        void setReadAccess(const ParseUser& user, bool allowed);
        void setReadAccess(const QString& userId, bool allowed);
        void setReadAccess(const QStringList& userIdList, bool allowed);
        void setWriteAccess(const ParseUser& user, bool allowed);
        void setWriteAccess(const QString& userId, bool allowed);
        void setWriteAccess(const QStringList& userIdList, bool allowed);

        QVariantMap toMap() const;

    private:
        QVariantMap _map;
        static ParseACL _defaultACL;
    };
}

#endif // CGPARSE_PARSEACL_H
