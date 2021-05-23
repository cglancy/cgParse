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
#include "parseobject.h"
#include "parsereply.h"

#include <QFile>
#include <QFileInfo>
#include <QUrl>

namespace cg
{
    QSharedPointer<ParseFile> ParseFile::create()
    {
        return QSharedPointer<ParseFile>::create();
    }

    QSharedPointer<ParseFile> ParseFile::create(const QString &localPath)
    {
        return QSharedPointer<ParseFile>::create(localPath);
    }

    QSharedPointer<ParseFile> ParseFile::create(const QString &name, const QByteArray &data, const QString &contentType)
    {
        return QSharedPointer<ParseFile>::create(name, data, contentType);
    }

    QSharedPointer<ParseQuery<ParseFile>> ParseFile::query()
    {
        return QSharedPointer<ParseQuery<ParseFile>>::create();
    }

    ParseFile::ParseFile()
        : _pHelper(new ParseFileHelper())
    {
    }

    ParseFile::ParseFile(const ParseFile& file)
    {
        _name = file._name;
        _url = file._url;
        _contentType = file._contentType;
        _data = file._data;
    }

    ParseFile::ParseFile(const QString &path)
        : _contentType("unknown/unknown"),
        _pHelper(new ParseFileHelper())
    {
        QFileInfo fi(path);
        _name = fi.fileName();
        QString extension = fi.suffix();
        if (extension == "jpg" || extension == "jpeg")
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
        _contentType(contentType),
        _data(data),
        _pHelper(new ParseFileHelper())
    {
    }

    ParseFile::~ParseFile()
    {
    }

    ParseReply* ParseFile::deleteFile(const QString &urlStr, const QString &masterKey)
    {
        QUrl url(urlStr);
        ParseRequest request(ParseRequest::DeleteHttpMethod, "/files/" + url.fileName());
        request.removeHeader("X-Parse-REST-API-Key");
        request.setHeader("X-Parse-Master-Key", masterKey.toUtf8());
        return new ParseReply(request);
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

    QVariantMap ParseFile::toMap() const
    {
        QVariantMap map;
        map.insert(Parse::TypeKey, Parse::FileValue);
        map.insert("name", _name);
        if (_url.isEmpty())
            map.insert("url", _url);
        return map;
    }

    void ParseFile::setValues(const QVariantMap &map)
    {
        if (map.contains("name"))
            _name = map.value("name").toString();

        if (map.contains("url"))
            _url = map.value("url").toString();
    }

    ParseReply* ParseFile::save()
    {
        _pHelper->_pFile = sharedFromThis();
        ParseRequest request(ParseRequest::PostHttpMethod, "/files/" + name(), data(), contentType());
        ParseReply *pReply = new ParseReply(request);
        QObject::connect(pReply, &ParseReply::preFinished, _pHelper.data(), &ParseFileHelper::saveFileFinished);
        return pReply;
    }

    ParseReply* ParseFile::fetch()
    {
        _pHelper->_pFile = sharedFromThis();
        ParseRequest request(ParseRequest::GetHttpMethod, url());
        ParseReply* pReply = new ParseReply(request);
        QObject::connect(pReply, &ParseReply::preFinished, _pHelper.data(), &ParseFileHelper::fetchFileFinished);
        return pReply;
    }
}
