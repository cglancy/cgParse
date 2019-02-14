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

#include "parseobject.h"
#include "parsefile.h"
#include <QObject>
#include <QDir>

class TestMovie;
typedef QSharedPointer<TestMovie> TestMoviePtr;

class TestCharacter;
typedef QSharedPointer<TestCharacter> TestCharacterPtr;

class TestQuote;
typedef QSharedPointer<TestQuote> TestQuotePtr;

//
// TestMovie
//
class TestMovie : public cg::ParseObject
{
public:
    TestMovie();
    TestMovie(const QString &title);
    ~TestMovie() {}

    QString title() const { return value("title").toString(); }

private:
    void setTitle(const QString &title) { setValue("title", title); }
};

//
// TestCharacter
//
class TestCharacter : public cg::ParseObject
{
public:
    TestCharacter();
    TestCharacter(const QString &name);
    ~TestCharacter() {}

    QString name() const { return value("name").toString(); }
    cg::ParseFilePtr picture() { return file("picture"); }
    void setPicture(cg::ParseFilePtr pFile) { setFile("picture", pFile); }

private:
    void setName(const QString &name) { setValue("name", name); }
};

//
// TestQuote
//
class TestQuote : public cg::ParseObject
{
public:
    TestQuote();
    TestQuote(TestMoviePtr movie, TestCharacterPtr character, int rank, const QString &quote);
    ~TestQuote() {}

    int rank() const { return value("rank").toInt(); }
    QString quote() const { return value("quote").toString(); }
    TestMoviePtr movie() const { return object<TestMovie>("movie"); }
    TestCharacterPtr character() const { return object<TestCharacter>("character"); }

private:
    void setRank(int rank) { setValue("rank", rank); }
    void setQuote(const QString &quote) { setValue("quote", quote); }
    void setMovie(TestMoviePtr pMovie) { setObject<TestMovie>("movie", pMovie); }
    void setCharacter(TestCharacterPtr pCharacter) { setObject<TestCharacter>("character", pCharacter); }
};

//
// ParseTest
//
class ParseTest : public QObject
{
    Q_OBJECT

    void createTestObjects();
    TestMoviePtr createMovie(const QString &title);
    TestCharacterPtr createCharacter(const QString &name, const QString &imageFile = QString());
    TestQuotePtr createQuote(TestMoviePtr movie, TestCharacterPtr character, int rank, const QString &quote);
    void deleteTestObjects();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testAsyncFuture();
    void testUserLogin();
    void testUserSignUp();
    void testResetPassword();

    void testObject();
    void testObjectRevert();
    void testObjectArray();
    void testObjectRelation();

    void testGetQuery();
    void testFindAllQuery();
    void testCountQuery();
    void testOrderQuery();
    void testComparisonQuery();
    void testFullTextQuery();

private:
    TestMoviePtr episode1, episode2, episode3, episode4, episode5, episode6, episode7, episode8, rogue1;
    TestCharacterPtr leia, han, obiwan, yoda, luke, palpatine, anakin, vader, quigon, nute, 
        shmi, jamillia, jango, dooku, padme, rey, c3po, chirrut, cassian, k2so;
    QList<cg::ParseObjectPtr> _movies, _characters, _quotes;
    QDir _testImagesDir;
};

#endif // CGPARSE_PARSETEST_H
