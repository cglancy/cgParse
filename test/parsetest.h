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
#ifndef CGPARSE_PARSETEST_H
#define CGPARSE_PARSETEST_H
#pragma once

#include <QObject>
#include "parseobject.h"

class TestCharacter : public cg::ParseObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName)
public:
    TestCharacter(const QString &name) 
        : cg::ParseObject("TestCharacter"),
        _name(name)
    {}

    QString name() const { return _name; }

private:
    void setName(const QString &name) { _name = name; }
    QString _name;
};

Q_DECLARE_METATYPE(TestCharacter*);

class TestQuote : public cg::ParseObject
{
    Q_OBJECT
    Q_PROPERTY(int rank READ rank WRITE setRank)
    Q_PROPERTY(QString quote READ quote WRITE setQuote)
    Q_PROPERTY(TestCharacter* character READ character WRITE setCharacter)
public:
    TestQuote(TestCharacter *character, int rank, const QString &quote)
        : cg::ParseObject("TestQuote"),
        _character(character),
        _rank(rank),
        _quote(quote)
    {}

    int rank() const { return _rank; }
    QString quote() const { return _quote; }
    TestCharacter* character() const { return _character; }

private:
    void setRank(int rank) { _rank = rank; }
    void setQuote(const QString &quote) { _quote = quote; }
    void setCharacter(TestCharacter *pCharacter) { _character = pCharacter; }

private:
    int _rank;
    QString _quote;
    TestCharacter *_character;
};

class ParseTest : public QObject
{
    Q_OBJECT
    void createTestObjects();
    TestCharacter * createCharacter(const QString &name);
    TestQuote * createQuote(TestCharacter *character, int rank, const QString &quote);
    void deleteTestObjects();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testObject();
    void testObjectSubclass();
    void testUser();
    void testGetQuery();

private:
    TestCharacter *leia, *han, *obiwan, *yoda, *luke, *palpatine, *anakin, *vader, *quigon, *nute, 
        *shmi, *jamillia, *jango, *dooku, *padme, *rey, *c3po, *chirrut, *cassian, *k2so;
    QList<cg::ParseObject*> _characters, _quotes;
};

#endif // CGPARSE_PARSETEST_H
