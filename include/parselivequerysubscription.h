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
#ifndef CGPARSE_PARSELIVEQUERYSUBSCRIPTION_H
#define CGPARSE_PARSELIVEQUERYSUBSCRIPTION_H
#pragma once

#include "parse.h"
#include "parseobject.h"
#include <QObject>
#include <QString>

namespace cg
{
    class CGPARSE_API ParseLiveQuerySubscription : public QObject
    {
        Q_OBJECT
    public:
        ParseLiveQuerySubscription(int id);
        ~ParseLiveQuerySubscription();

        int id() const;
        void unsubscribe();

    signals:
        void subscribed();
        void createEvent(const ParseObject& object);
        void enterEvent(const ParseObject& object);
        void leaveEvent(const ParseObject& object);
        void updateEvent(const ParseObject& object);
        void deleteEvent(const ParseObject& object);

    private:
        int _id;
    };
}

#endif // CGPARSE_PARSELIVEQUERYSUBSCRIPTION_H
