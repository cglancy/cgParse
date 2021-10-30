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
#include "parserole.h"
#include "parseuser.h"

namespace cg
{
    ParseRole ParseRole::create()
    {
        return ParseObject(Parse::RoleClassNameKey);
    }

    ParseQuery<ParseRole> ParseRole::query()
    {
        return ParseQuery<ParseRole>();
    }

    ParseRole::ParseRole()
    {
        // constructs null object
    }

    ParseRole::ParseRole(const ParseRole& role)
        : ParseObject(role)
    {
    }

    ParseRole::ParseRole(const ParseObject& object)
    {
        if (object.className() == Parse::RoleClassNameKey)
            ParseObject::assign(object);
        else
            nullify();
    }

    ParseRole::~ParseRole()
    {
    }

    void ParseRole::assign(const ParseObject& object)
    {
        if (object.className() == Parse::RoleClassNameKey)
            ParseObject::assign(object);
        else
            nullify();
    }

    ParseRole& ParseRole::operator=(const ParseRole& role)
    {
        ParseObject::operator=(role);
        return *this;
    }

    QString ParseRole::name() const
    {
        return value("name").toString();
    }

    void ParseRole::setName(const QString &name)
    {
        setValue("name", name);
    }

    ParseRelation<ParseRole> ParseRole::roles()
    {
        return ParseRelation<ParseRole>(className(), objectId(), "roles");
    }

    ParseRelation<ParseUser> ParseRole::users()
    {
        return ParseRelation<ParseUser>(className(), objectId(), "users");
    }

}
