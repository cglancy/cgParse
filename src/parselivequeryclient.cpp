/**
* Copyright 2019 Charles Glancy (charles@glancyfamily.net)
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
#include "parselivequeryclient.h"
#include "parselivequerysubscription.h"
#include "parseconvert.h"

#include <QJsonDocument>
#include <QDebug>

namespace cg {
    ParseLiveQueryClient * ParseLiveQueryClient::_pInstance = nullptr;

    ParseLiveQueryClient::ParseLiveQueryClient()
        : _nextId(1)
    {
        connect(&_webSocket, &QWebSocket::connected, this, &ParseLiveQueryClient::connected);
        connect(&_webSocket, &QWebSocket::disconnected, this, &ParseLiveQueryClient::disconnected);
        connect(&_webSocket, &QWebSocket::textMessageReceived, this, &ParseLiveQueryClient::textMessageReceived);
    }

    ParseLiveQueryClient::~ParseLiveQueryClient()
    {
    }

    ParseLiveQueryClient * ParseLiveQueryClient::get()
    {
        if (!_pInstance)
            _pInstance = new ParseLiveQueryClient();

        return _pInstance;
    }

    void ParseLiveQueryClient::open(const QString &appId, const QString &serverUrl, const QString &restApiKey, const QString &sessionToken)
    {
        _appId = appId;
        _restApiKey = restApiKey;
        _serverUrl = serverUrl;
        _sessionToken = sessionToken;

        if (_webSocket.state() == QAbstractSocket::UnconnectedState)
        {
            QString urlStr = "ws://" + _serverUrl;
            _webSocket.open(QUrl(urlStr));
        }
    }

    void ParseLiveQueryClient::close()
    {
        if (_webSocket.state() != QAbstractSocket::UnconnectedState ||
            _webSocket.state() != QAbstractSocket::ClosingState)
        {
            _webSocket.close();
        }
    }

    void ParseLiveQueryClient::sendJsonObject(const QJsonObject &jsonObject)
    {
        if (_webSocket.state() != QAbstractSocket::ConnectedState)
        {
            qDebug() << "Error: WebSocket is not connected.";
            return;
        }

        QJsonDocument doc(jsonObject);
        QByteArray byteArray = doc.toJson(QJsonDocument::Compact);

        qDebug() << "ParseLiveQueryClient::sendJsonObject() " << byteArray;

        _webSocket.sendTextMessage(byteArray);
    }

    ParseLiveQuerySubscription* ParseLiveQueryClient::subscribe(const QJsonObject & queryObject, const QString & sessionToken)
    {
        if (_webSocket.state() != QAbstractSocket::ConnectedState)
            return nullptr;

        int id = _nextId++;
        ParseLiveQuerySubscription *pSubscription = new ParseLiveQuerySubscription(id);
        _subscriptionMap.insert(id, pSubscription);

        QJsonObject jsonObject;
        jsonObject.insert("op", "subscribe");
        jsonObject.insert("requestId", id);
        jsonObject.insert("query", queryObject);

        if (!sessionToken.isEmpty())
            jsonObject.insert("sessionToken", sessionToken);

        sendJsonObject(jsonObject);

        return pSubscription;
    }

    void ParseLiveQueryClient::unsubscribe(int id)
    {
        _subscriptionMap.remove(id);

        QJsonObject jsonObject;
        jsonObject.insert("op", "unsubscribe");
        jsonObject.insert("requestId", id);
        sendJsonObject(jsonObject);
    }

    void ParseLiveQueryClient::connected()
    {
        QJsonObject jsonObject;

        jsonObject.insert("op", "connect");
        jsonObject.insert("applicationId", _appId);
        jsonObject.insert("restAPIKey", _restApiKey);
        if (!_sessionToken.isEmpty())
            jsonObject.insert("sessionToken", _sessionToken);

        sendJsonObject(jsonObject);
    }

    void ParseLiveQueryClient::disconnected()
    {
        emit closed();
    }

    void ParseLiveQueryClient::textMessageReceived(const QString & message)
    {
        qDebug() << "ParseLiveQueryClient::textMessageReceived() " << message;

        QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
        if (doc.isObject())
        {
            QJsonObject messageObject = doc.object();
            QString operation = messageObject.value("op").toString();

            if (operation == "error")
            {
                int code = messageObject.value("code").toInt();
                QString message = messageObject.value("error").toString();
                bool reconnect = messageObject.value("reconnect").toBool();
                emit error(code, message, reconnect);
            }
            else if (operation == "connected")
            {
                emit opened();
            }
            else if (operation == "subscribed")
            {
                int id = messageObject.value("requestId").toInt();
                ParseLiveQuerySubscription *pSubscription = _subscriptionMap.value(id);
                if (pSubscription)
                    emit pSubscription->subscribed();
                emit subscribed(id);
            }
            else if (operation == "unsubscribed")
            {
                int id = messageObject.value("requestId").toInt();
                emit unsubscribed(id);
            }
            else if (operation == "create")
            {
                int id = messageObject.value("requestId").toInt();
                ParseLiveQuerySubscription *pSubscription = _subscriptionMap.value(id);

                if (pSubscription)
                {
                    QJsonObject jsonObject = messageObject.value("object").toObject();
                    QSharedPointer<ParseObject> pObject = createObject(jsonObject);
                    emit pSubscription->createEvent(pObject);
                }
            }
            else if (operation == "enter")
            {
                int id = messageObject.value("requestId").toInt();
                ParseLiveQuerySubscription *pSubscription = _subscriptionMap.value(id);

                if (pSubscription)
                {
                    QJsonObject jsonObject = messageObject.value("object").toObject();
                    QSharedPointer<ParseObject> pObject = createObject(jsonObject);
                    emit pSubscription->enterEvent(pObject);
                }
            }
            else if (operation == "leave")
            {
                int id = messageObject.value("requestId").toInt();
                ParseLiveQuerySubscription *pSubscription = _subscriptionMap.value(id);

                if (pSubscription)
                {
                    QJsonObject jsonObject = messageObject.value("object").toObject();
                    QSharedPointer<ParseObject> pObject = createObject(jsonObject);
                    emit pSubscription->leaveEvent(pObject);
                }
            }
            else if (operation == "update")
            {
                int id = messageObject.value("requestId").toInt();
                ParseLiveQuerySubscription *pSubscription = _subscriptionMap.value(id);

                if (pSubscription)
                {
                    QJsonObject jsonObject = messageObject.value("object").toObject();
                    QSharedPointer<ParseObject> pObject = createObject(jsonObject);
                    emit pSubscription->updateEvent(pObject);
                }
            }
            else if (operation == "delete")
            {
                int id = messageObject.value("requestId").toInt();
                ParseLiveQuerySubscription *pSubscription = _subscriptionMap.value(id);

                if (pSubscription)
                {
                    QJsonObject jsonObject = messageObject.value("object").toObject();
                    QSharedPointer<ParseObject> pObject = createObject(jsonObject);
                    emit pSubscription->deleteEvent(pObject);
                }
            }
        }
    }

    QSharedPointer<ParseObject> ParseLiveQueryClient::createObject(const QJsonObject &jsonObject)
    {
        QString className = jsonObject.value(Parse::ClassNameKey).toString();
        QString objectId = jsonObject.value(Parse::ObjectIdKey).toString();
        QVariantMap map = ParseConvert::toVariantMap(jsonObject);

        QSharedPointer<ParseObject> pObject = ParseObject::createWithoutData(className, objectId);
        pObject->setValues(map);

        return pObject;
    }
}
