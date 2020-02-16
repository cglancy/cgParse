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
#ifndef CGPARSE_PARSEQUERYMODEL_H
#define CGPARSE_PARSEQUERYMODEL_H
#pragma once

#include "parseobject.h"
#include "parsequeryhelper.h"

#include <QAbstractListModel>
#include <QScopedPointer>
#include <QVariant>
#include <QHash>

namespace cg
{
    class ParseQueryModel : public QAbstractListModel
    {
        Q_OBJECT
        Q_PROPERTY(QString className READ className WRITE setClassName NOTIFY classNameNotify)
        Q_PROPERTY(QStringList keys READ keys WRITE setKeys NOTIFY keysNotify)
        Q_PROPERTY(QVariantMap query READ query WRITE setQuery NOTIFY queryNotify)

    public:
        explicit ParseQueryModel(QObject *parent = nullptr);
		virtual ~ParseQueryModel();

		Q_INVOKABLE void find();
		ParseReply * findWithReply();

        QString className() const;
        void setClassName(const QString &className);

        QStringList keys() const;
        void setKeys(const QStringList &keysList);

        QVariantMap query() const;
        void setQuery(const QVariantMap &queryMap);

        QVariant data(const QModelIndex &index, const QString &key) const;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

    signals:
        void classNameNotify();
        void keysNotify();
        void queryNotify();

	private slots:
		void findFinished();

	protected:
        void updateHash();

    protected:
        QString _className;
        QStringList _keysList;
        QVariantMap _queryMap;
        QHash<int, QByteArray> _roleHash;
        QList<ParseObjectPtr> _objects;
		QScopedPointer<ParseQueryHelper> _pHelper;
    };
}

#endif // CGPARSE_PARSEQUERYMODEL_H
