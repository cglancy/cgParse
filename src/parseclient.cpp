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
#include "parseclient.h"
#include "parseresult.h"

#include <QMetaType>


namespace cg {
    ParseClient * ParseClient::_pInstance = nullptr;

    ParseClient::ParseClient()
        : _loggingEnabled(false)
    {
        qRegisterMetaType<ParseResult>();
        qRegisterMetaType<ParseUserResult>();
        qRegisterMetaType<ParseObjectsResult>();
        qRegisterMetaType<ParseCountResult>();
        qRegisterMetaType<ParseSessionResult>();
    }

    ParseClient::~ParseClient()
    {
    }

    ParseClient * ParseClient::get()
    {
        if (!_pInstance)
            _pInstance = new ParseClient();

        return _pInstance;
    }

    void ParseClient::initialize(const QByteArray &appId, const QByteArray &clientKey, const QByteArray &apiHost)
    {
        _appId = appId;
        _clientKey = clientKey;
        _apiHost = apiHost;
    }

    QByteArray ParseClient::applicationId() const
    {
        return _appId;
    }

    QByteArray ParseClient::clientKey() const
    {
        return _clientKey;
    }

    QByteArray ParseClient::apiHost() const
    {
        return _apiHost;
    }

    bool ParseClient::isLoggingEnabled() const
    {
        return _loggingEnabled;
    }

    void ParseClient::setLoggingEnabled(bool enabled)
    {
        _loggingEnabled = enabled;
    }
}
