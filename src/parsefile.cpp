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
#include "parsefileimpl.h"
#include "parseclient.h"
#include "parsefilerequest.h"
#include "parserequest.h"
#include "parseobject.h"
#include "parsereply.h"

#include <QFile>
#include <QFileInfo>
#include <QUrl>

namespace cg
{
    ParseFile::ParseFile()
    {
        // construct null object
    }

    ParseFile::ParseFile(const QString &path)
    {
        _pImpl = QSharedPointer<ParseFileImpl>::create();

        QFileInfo fi(path);
        _pImpl->name = fi.fileName();
        QString extension = fi.suffix();
        if (extension == "jpg" || extension == "jpeg")
            _pImpl->contentType = "image/jpeg";
        else if (extension == "png")
            _pImpl->contentType = "image/png";

        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
        {
            _pImpl->data = file.readAll();
            file.close();
        }
    }

    ParseFile::ParseFile(const QString& name, const QString& url)
    {
        _pImpl = QSharedPointer<ParseFileImpl>::create();

        _pImpl->name = name;
        _pImpl->url = url;

        QFileInfo fi(name);
        QString extension = fi.suffix();
        if (extension == "jpg" || extension == "jpeg")
            _pImpl->contentType = "image/jpeg";
        else if (extension == "png")
            _pImpl->contentType = "image/png";
    }

    ParseFile::ParseFile(const QString &name, const QByteArray &data, const QString &contentType)
    {
        _pImpl = QSharedPointer<ParseFileImpl>::create();
        
        _pImpl->name = name;
        _pImpl->data = data;
        _pImpl->contentType = contentType;
    }

    ParseFile::ParseFile(const ParseFile& file)
    {
        _pImpl = file._pImpl;
    }

    ParseFile::~ParseFile()
    {
    }

    ParseFile& ParseFile::operator=(const ParseFile& file)
    {
        _pImpl = file._pImpl;
        return *this;
    }

    ParseReply* ParseFile::deleteFile(const QString &urlStr, const QString &masterKey, QNetworkAccessManager *pNam)
    {
        return ParseFileRequest::get()->deleteFile(urlStr, masterKey, pNam);
    }

    bool ParseFile::isNull() const
    {
        return nullptr == _pImpl;
    }

    bool ParseFile::isDirty() const
    {
        return _pImpl ? _pImpl->url.isEmpty() : false;
    }

    QString ParseFile::name() const
    {
        return _pImpl ? _pImpl->name : QString();
    }

    void ParseFile::setName(const QString &name)
    {
        if (_pImpl)
            _pImpl->name = name;
    }

    QString ParseFile::url() const
    {
        return _pImpl ? _pImpl->url : QString();
    }

    void ParseFile::setUrl(const QString &url)
    {
        if (_pImpl)
            _pImpl->url = url;
    }

    QString ParseFile::contentType() const
    {
        return _pImpl ? _pImpl->contentType : QString("unknown/unknown");
    }

    void ParseFile::setContentType(const QString &contentType)
    {
        if (_pImpl)
            _pImpl->contentType = contentType;
    }

    QByteArray ParseFile::data() const
    {
        return _pImpl ? _pImpl->data : QByteArray();
    }

    QVariantMap ParseFile::toMap() const
    {
        if (!_pImpl)
            return QVariantMap();

        QVariantMap map;
        map.insert(Parse::TypeKey, Parse::FileValue);
        map.insert("name", _pImpl->name);
        if (_pImpl->url.isEmpty())
            map.insert("url", _pImpl->url);
        return map;
    }

    void ParseFile::setValues(const QVariantMap &map)
    {
        if (_pImpl)
        {
            if (map.contains("name"))
                _pImpl->name = map.value("name").toString();

            if (map.contains("url"))
                _pImpl->url = map.value("url").toString();
        }
    }

    ParseReply* ParseFile::save(QNetworkAccessManager *pNam)
    {
        return ParseFileRequest::get()->saveFile(*this, pNam);
    }

    ParseReply* ParseFile::fetch(QNetworkAccessManager* pNam)
    {
        return ParseFileRequest::get()->fetchFile(*this, pNam);
    }
}
