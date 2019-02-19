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
#ifndef CGPARSE_PARSEFILE_H
#define CGPARSE_PARSEFILE_H
#pragma once

#include "parse.h"
#include "parseresult.h"
#include "parsequery.h"
#include <QEnableSharedFromThis>
#include <QString>
#include <QByteArray>
#include <QMetaType>
#include <QJsonObject>
#include <QVariant>
#include <QFuture>
#include <QScopedPointer>
#include <QSharedPointer>

class QFile;

namespace cg
{
    class ParseFileHelper;

    class CGPARSE_API ParseFile : public QEnableSharedFromThis<ParseFile>
    {
    public:
        static ParseFilePtr create();
        static ParseFilePtr create(const QString &localPath);
        static ParseFilePtr create(const QString &name, const QByteArray &data, const QString &contentType);

        static QFuture<int> deleteFile(const QString &url, const QString &masterKey);

        static QSharedPointer<ParseQuery<ParseFile>> query();

        static bool isFile(const QVariant &variant);
        static bool isFile(const QJsonValue &jsonValue);

    public:
        ParseFile();
        ParseFile(const QString &localPath);
        ParseFile(const QString &name, const QByteArray &data, const QString &contentType);
        ~ParseFile();

        bool isDirty() const;

        QString name() const;
        void setName(const QString &name);

        QString url() const;
        void setUrl(const QString &url);

        QString contentType() const;
        void setContentType(const QString &contentType);

        QByteArray data() const;
        QJsonObject toJsonObject() const;
        QVariantMap toMap() const;
        void setValues(const QJsonObject &jsonObject);
        void setValues(const QVariantMap &map);

        QFuture<int> save();

    private:
        static ParseFileHelper * staticHelper();

    private:
        QString _name, _url, _contentType;
        QByteArray _data;
        QScopedPointer<ParseFileHelper> _pHelper;
        static ParseFileHelper *_pStaticHelper;
    };
}

#endif // CGPARSE_PARSEFILE_H
