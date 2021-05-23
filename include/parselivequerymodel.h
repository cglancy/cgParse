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
#ifndef CGPARSE_PARSELIVEQUERYMODEL_H
#define CGPARSE_PARSELIVEQUERYMODEL_H
#pragma once

#include "parsequerymodel.h"

namespace cg
{
	class ParseLiveQuerySubscription;

    class CGPARSE_API ParseLiveQueryModel : public ParseQueryModel
    {
        Q_OBJECT
    public:
        explicit ParseLiveQueryModel(QObject *parent = nullptr);
		~ParseLiveQueryModel();

	public slots:
		void subscribe();
		void unsubscribe();

	signals:
		void subscribed();

	private slots:
		void createEvent(QSharedPointer<ParseObject> pObject);
		void enterEvent(QSharedPointer<ParseObject> pObject);
		void leaveEvent(QSharedPointer<ParseObject> pObject);
		void updateEvent(QSharedPointer<ParseObject> pObject);
		void deleteEvent(QSharedPointer<ParseObject> pObject);

	private:
		int indexOf(QSharedPointer<ParseObject> pObject) const;

    private:
		ParseLiveQuerySubscription *m_pSubscription;
    };
}

#endif // CGPARSE_PARSELIVEQUERYMODEL_H
