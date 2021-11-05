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

//
// TestMovie
//
class TestMovie : public cg::ParseObject
{
public:
    TestMovie();
    TestMovie(const TestMovie &movie);
    TestMovie(const ParseObject& object);
    TestMovie(const QString &title);
    ~TestMovie() {}

    void assign(const ParseObject& object) override;
    TestMovie& operator=(const TestMovie& other);

    QString title() const { return value("title").toString(); }

private:
    void setTitle(const QString &title) { setValue("title", title); }
};

Q_DECLARE_METATYPE(TestMovie);

//
// TestCharacter
//
class TestCharacter : public cg::ParseObject
{
public:
    TestCharacter();
    TestCharacter(const QString &name);
    TestCharacter(const TestCharacter &character);
    TestCharacter(const ParseObject& object);
    ~TestCharacter() {}

    void assign(const ParseObject& object) override;
    TestCharacter& operator=(const TestCharacter& other);

    QString name() const { return value("name").toString(); }
    cg::ParseFile picture() { return file("picture"); }
    void setPicture(cg::ParseFile pFile) { setFile("picture", pFile); }

private:
    void setName(const QString &name) { setValue("name", name); }
};

Q_DECLARE_METATYPE(TestCharacter);

//
// TestQuote
//
class TestQuote : public cg::ParseObject
{
public:
    TestQuote();
    TestQuote(const TestMovie& movie, const TestCharacter& character, int rank, const QString &quote);
    TestQuote(const TestQuote &quote);
    TestQuote(const ParseObject& object);
    ~TestQuote() {}

    void assign(const ParseObject& object) override;
    TestQuote& operator=(const TestQuote& other);

    int rank() const { return value("rank").toInt(); }
    QString quote() const { return value("quote").toString(); }
    TestMovie movie() const { return object<TestMovie>("movie"); }
    TestCharacter character() const { return object<TestCharacter>("character"); }

private:
    void setRank(int rank) { setValue("rank", rank); }
    void setQuote(const QString &quote) { setValue("quote", quote); }
    void setMovie(const TestMovie& movie) { setObject("movie", movie); }
    void setCharacter(const TestCharacter& character) { setObject("character", character); }
};

Q_DECLARE_METATYPE(TestQuote);

//
// TestNamespace
//
namespace ns1
{
    namespace ns2
    {
        class TestNamespace : public cg::ParseObject
        {
        public:
            TestNamespace();
        };
    }
}

Q_DECLARE_METATYPE(ns1::ns2::TestNamespace);

//
// ParseTest
//
class ParseTest : public QObject
{
    Q_OBJECT

    void createTestObjects();
    TestMovie createMovie(const QString &title);
    TestCharacter createCharacter(const QString &name, const QString &imageFile = QString());
    TestQuote createQuote(const TestMovie& movie, const TestCharacter& character, int rank, const QString &quote);
    void deleteTestObjects();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testVariant();
    void testDateTime();
    void testGeoPoint();
    void testPolygon();

    void testUserLogin();
    void testUserSignUp();
    void testUserResetPassword();

    void testObject();
    void testObjectRevert();
    void testObjectArray();
    void testObjectRelation();
    void testObjectPointerHash();
    void testObjectReferenceSave();
    void testObjectSetObject();

    void testQueryNamespace();
    void testQueryGet();
    void testQueryFindAll();
    void testQueryCount();
    void testQueryOrder();
    void testQueryComparison();
    void testQueryFullText();
    void testQueryOr();
    void testQueryModel();

    void testFile();

    void testLiveQueryClient();
	void testLiveQueryModel();

    void testGraphQL();

private:
    TestMovie episode1, episode2, episode3, episode4, episode5, episode6, episode7, episode8, rogue1;
    TestCharacter leia, han, obiwan, yoda, luke, palpatine, anakin, vader, quigon, nute, 
        shmi, jamillia, jango, dooku, padme, rey, c3po, chirrut, cassian, k2so;
    QList<cg::ParseObject> _movies, _characters, _quotes;
    QDir _testImagesDir;
};

#endif // CGPARSE_PARSETEST_H
