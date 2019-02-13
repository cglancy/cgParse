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
#include "parsefile.h"
#include "parseclient.h"
#include "parserequest.h"
#include "parsefilehelper.h"
#include <asyncfuture.h>

#include <QFile>
#include <QFileInfo>

namespace cg
{
    ParseFile::ParseFile()
        : _pHelper(new ParseFileHelper(sharedFromThis()))
    {
    }

    ParseFile::ParseFile(const QString &path)
        : _contentType("unknown/unknown")
    {
        QFileInfo fi(path);
        _name = fi.fileName();
        QString extension = fi.suffix();
        if (extension == "jpg")
            _contentType = "image/jpeg";
        else if (extension == "png")
            _contentType = "image/png";

        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
        {
            _data = file.readAll();
            file.close();
        }
    }

    ParseFile::ParseFile(const QString &name, const QByteArray &data, const QString &contentType)
        : _name(name),
        _data(data),
        _contentType(contentType)
    {
    }

    ParseFile::~ParseFile()
    {
    }

    QFuture<int> ParseFile::deleteFile(const QString &url, const QString &masterKey)
    {
        QScopedPointer<ParseFileHelper> pHelper(new ParseFileHelper(nullptr));
        pHelper->deleteFile(url, masterKey);
        return AsyncFuture::observe(pHelper.data(), &ParseFileHelper::deleteFileFinished).future();
    }

    bool ParseFile::isDirty() const
    {
        return _url.isEmpty();
    }

    QString ParseFile::name() const
    {
        return _name;
    }

    void ParseFile::setName(const QString &name)
    {
        _name = name;
    }

    QString ParseFile::url() const
    {
        return _url;
    }

    void ParseFile::setUrl(const QString &url)
    {
        _url = url;
    }

    QString ParseFile::contentType() const
    {
        return _contentType;
    }

    void ParseFile::setContentType(const QString &contentType)
    {
        _contentType = contentType;
    }

    QByteArray ParseFile::data() const
    {
        return _data;
    }

    QJsonObject ParseFile::toJsonObject() const
    {
        QJsonObject jsonObject;
        jsonObject.insert("__type", "File");
        jsonObject.insert("name", _name);
        if (_url.isEmpty())
            jsonObject.insert("url", _url);
        return jsonObject;
    }

    void ParseFile::setValues(const QJsonObject &jsonObject)
    {
        if (jsonObject.contains("name"))
            _name = jsonObject.value("name").toString();

        if (jsonObject.contains("url"))
            _url = jsonObject.value("url").toString();
    }

    QFuture<ParseFileReply> ParseFile::save()
    {
        _pHelper->saveFile(sharedFromThis());
        return AsyncFuture::observe(_pHelper.data(), &ParseFileHelper::saveFileFinished).future();
    }
}