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
#include "parseacl.h"
#include "parseuser.h"

namespace cg
{
    ParseACL ParseACL::_defaultACL;

    ParseACL ParseACL::defaultACL()
    {
        return _defaultACL;
    }

    void ParseACL::setDefaultACL(const ParseACL &acl, bool withAccessForCurrentUser)
    {
        _defaultACL = acl;

        if (withAccessForCurrentUser)
        {
            ParseUser currentUser = ParseUser::currentUser();
            if (!currentUser.isNull())
            {
                _defaultACL.setReadAccess(currentUser, true);
                _defaultACL.setWriteAccess(currentUser, true);
            }
        }
    }

    ParseACL::ParseACL()
    {
    }

    ParseACL::ParseACL(const QVariantMap& map)
        : _map(map)
    {
    }

    ParseACL::ParseACL(const ParseUser& user)
    {
        if (!user.isNull())
        {
            setReadAccess(user, true);
            setWriteAccess(user, true);
        }
    }

    ParseACL::ParseACL(const ParseACL &acl)
    {
        _map = acl._map;
    }

    ParseACL::~ParseACL()
    {
    }

    bool ParseACL::publicReadAccess() const
    {
        bool readAllowed = false;

        if (_map.contains(Parse::PublicAccessKey) && _map.value(Parse::PublicAccessKey).canConvert<QVariantMap>())
        {
            QVariantMap publicMap = _map.value(Parse::PublicAccessKey).toMap();
            readAllowed = publicMap.value(Parse::ReadKey, false).toBool();
        }

        return readAllowed;
    }

    bool ParseACL::publicWriteAccess() const
    {
        bool writeAllowed = false;

        if (_map.contains(Parse::PublicAccessKey) && _map.value(Parse::PublicAccessKey).canConvert<QVariantMap>())
        {
            QVariantMap publicMap = _map.value(Parse::PublicAccessKey).toMap();
            writeAllowed = publicMap.value(Parse::WriteKey, false).toBool();
        }

        return writeAllowed;
    }

    bool ParseACL::readAccess(const ParseUser& user) const
    {
        if (!user.isNull())
            return readAccess(user.objectId());

        return false;
    }

    bool ParseACL::readAccess(const QString & userId) const
    {
        bool readAllowed = false;

        if (_map.contains(userId) && _map.value(userId).canConvert<QVariantMap>())
        {
            QVariantMap userMap = _map.value(userId).toMap();
            readAllowed = userMap.value(Parse::ReadKey, false).toBool();
        }

        return readAllowed;
    }

    bool ParseACL::writeAccess(const ParseUser& user) const
    {
        if (!user.isNull())
            return writeAccess(user.objectId());

        return false;
    }

    bool ParseACL::writeAccess(const QString & userId) const
    {
        bool writeAllowed = false;

        if (_map.contains(userId) && _map.value(userId).canConvert<QVariantMap>())
        {
            QVariantMap userMap = _map.value(userId).toMap();
            writeAllowed = userMap.value(Parse::WriteKey, false).toBool();
        }

        return writeAllowed;
    }

    void ParseACL::setPublicReadAccess(bool allowed)
    {
        if (_map.contains(Parse::PublicAccessKey) && _map.value(Parse::PublicAccessKey).canConvert<QVariantMap>())
        {
            QVariantMap publicMap = _map.value(Parse::PublicAccessKey).toMap();
            publicMap.insert(Parse::ReadKey, allowed);
            _map.insert(Parse::PublicAccessKey, publicMap);
        }
        else
        {
            QVariantMap publicMap;
            publicMap.insert(Parse::ReadKey, allowed);
            _map.insert(Parse::PublicAccessKey, publicMap);
        }
    }

    void ParseACL::setPublicWriteAccess(bool allowed)
    {
        if (_map.contains(Parse::PublicAccessKey) && _map.value(Parse::PublicAccessKey).canConvert<QVariantMap>())
        {
            QVariantMap publicMap = _map.value(Parse::PublicAccessKey).toMap();
            publicMap.insert(Parse::WriteKey, allowed);
            _map.insert(Parse::PublicAccessKey, publicMap);
        }
        else
        {
            QVariantMap publicMap;
            publicMap.insert(Parse::WriteKey, allowed);
            _map.insert(Parse::PublicAccessKey, publicMap);
        }
    }

    void ParseACL::setReadAccess(const ParseUser& user, bool allowed)
    {
        if (!user.isNull())
            setReadAccess(user.objectId(), allowed);
    }

    void ParseACL::setReadAccess(const QString & userId, bool allowed)
    {
        if (_map.contains(userId) && _map.value(userId).canConvert<QVariantMap>())
        {
            QVariantMap userMap = _map.value(userId).toMap();
            userMap.insert(Parse::ReadKey, allowed);
            _map.insert(userId, userMap);
        }
        else
        {
            QVariantMap userMap;
            userMap.insert(Parse::ReadKey, allowed);
            _map.insert(userId, userMap);
        }
    }

    void ParseACL::setWriteAccess(const ParseUser& user, bool allowed)
    {
        if (!user.isNull())
            setWriteAccess(user.objectId(), allowed);
    }

    void ParseACL::setWriteAccess(const QString & userId, bool allowed)
    {
        if (_map.contains(userId) && _map.value(userId).canConvert<QVariantMap>())
        {
            QVariantMap userMap = _map.value(userId).toMap();
            userMap.insert(Parse::WriteKey, allowed);
            _map.insert(userId, userMap);
        }
        else
        {
            QVariantMap userMap;
            userMap.insert(Parse::WriteKey, allowed);
            _map.insert(userId, userMap);
        }
    }

    QVariantMap ParseACL::toMap() const
    {
        return _map;
    }

}
