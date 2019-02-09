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

#include <QFile>
#include <QFileInfo>

namespace cg
{
    ParseFile::ParseFile(ParseObject *pParent)
    {
        pParent;
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

    ParseReply* ParseFile::save()
    {
        return ParseClient::instance()->saveFile(this);
    }
}