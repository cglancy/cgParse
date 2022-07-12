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
#include "parsegraphql.h"
#include "parseclient.h"
#include "parsereply.h"
#include "parserequest.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

namespace cg
{
    ParseReply* ParseGraphQL::query(const QString& queryStr, const QString& operationStr, const QVariantMap& variables)
    {
        return new ParseReply(ParseGraphQL(queryStr, operationStr, variables));
    }

    ParseGraphQL::ParseGraphQL(const QString& queryStr, const QString& operationStr, const QVariantMap& variableMap)
    {
        setApiRoute("graphql");
        setHttpMethod(PostHttpMethod);
        setContentType(JsonContentType);

        setHeader("User-Agent", ParseRequest::userAgent());
        setHeader("X-Parse-Application-Id", ParseClient::get()->applicationId());
        setHeader("X-Parse-Master-Key", ParseClient::get()->masterKey());
        setHeader("X-Parse-Client-Key", ParseClient::get()->clientKey());

        QJsonObject jsonObject;
        jsonObject["query"] = queryStr;

        if (!operationStr.isEmpty())
            jsonObject["operationName"] = operationStr;

        if (!variableMap.isEmpty())
        {
            QJsonObject variableObject = QJsonObject::fromVariantMap(variableMap);
            jsonObject["variables"] = variableObject;
        }

        QJsonDocument doc;
        doc.setObject(jsonObject);

        setContent(doc.toJson());
    }
}
