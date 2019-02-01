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
#include "parsetest.h"
#include "parseclient.h"
#include "parseobject.h"
#include "parseuser.h"
#include "parsequery.h"
#include "parsefile.h"

// used to define your own PARSE_APPLICATION_ID, PARSE_CLIENT_API_KEY
#include "parsesecret.h"

#include <QTest>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QDebug>
#include <QScopedPointer>
#include <QFileInfo>
#include <QSequentialIterable>

using namespace cg;

QTEST_MAIN(ParseTest)

void ParseTest::initTestCase()
{
    ParseClient::instance()->initialize(PARSE_APPLICATION_ID, PARSE_CLIENT_API_KEY, "api.parse.buddy.com");

    // set name & version for user-agent
    QCoreApplication::setApplicationName("ParseTest");
    QCoreApplication::setApplicationVersion("0.1");

    QString appDirPath = QCoreApplication::applicationDirPath();
    _testImagesDir.setPath(appDirPath + "/../../../../test/images");

    createTestObjects();
}

TestCharacter::TestCharacter(const QString &name)
    : cg::ParseObject("TestCharacter"),
    _name(name),
    _picture(nullptr)
{
}

TestCharacter * ParseTest::createCharacter(const QString &name, const QString &imageFile)
{
    ParseFile *pFile = nullptr;

    if (!imageFile.isEmpty())
    {
        QString imagePath = _testImagesDir.absolutePath() + "/" + imageFile;
        pFile = new ParseFile(imagePath);
        _files.append(pFile);

        pFile->save();
        QSignalSpy saveSpy(pFile, &ParseFile::saveFinished);
        saveSpy.wait(100000);
    }

    TestCharacter * character = new TestCharacter(name);
    if (pFile)
        character->setPicture(pFile);

    _characters.append(character);
    return character;
}

TestQuote * ParseTest::createQuote(TestCharacter *character, int rank, const QString &quote)
{
    TestQuote * quoteObject = new TestQuote(character, rank, quote);
    _quotes.append(quoteObject);
    return quoteObject;
}

void ParseTest::createTestObjects()
{
    luke = createCharacter("Luke Skywalker", "luke.png");        
    leia = createCharacter("Leia Organa", "leia.jpg");            
    anakin = createCharacter("Anakin Skywalker");
    vader = createCharacter("Darth Vader");
    han = createCharacter("Han Solo");               
    obiwan = createCharacter("Obi-wan Kenobi");      
    yoda = createCharacter("Yoda");                   
    palpatine = createCharacter("Emperor Palpatine"); 
    quigon = createCharacter("Qui-Gon Jinn");         
    nute = createCharacter("Nute Gunray");            
    shmi = createCharacter("Shmi Skywalker");         
    jamillia = createCharacter("Queen Jamillia");    
    jango = createCharacter("Jango Fett");          
    dooku = createCharacter("Count Dooku");          
    padme = createCharacter(QString("Padm%1 Amidala").arg(QChar(0x00E9)));
    rey = createCharacter("Rey");                    
    c3po = createCharacter("C-3PO");                 
    chirrut = createCharacter(QString("Chirrut %1mwe").arg(QChar(0x00CE)));
    cassian = createCharacter("Cassian Andor");       
    k2so = createCharacter("K-2SO"); 

    cg::ParseClient::instance()->createAll(_characters);
    QSignalSpy createSpy(cg::ParseClient::instance(), &ParseClient::createAllFinished);
    QVERIFY(createSpy.wait(10000));

    int rank = 1;
    QList<ParseObject*> quotes;
    createQuote(leia, rank++, "Help me, Obi-Wan Kenobi. You're my only hope.");
    createQuote(vader, rank++, "I find your lack of faith disturbing.");
    createQuote(han, rank++, "It's the ship that made the Kessel run in less than twelve parsecs.");
    createQuote(obiwan, rank++, "The Force will be with you. Always.");
    createQuote(leia, rank++, "Why, you stuck-up, half-witted, scruffy-looking nerf herder!");
    createQuote(han, rank++, "Never tell me the odds!");
    createQuote(yoda, rank++, "Do. Or do not. There is no try.");
    createQuote(vader, rank++, "No. I am your father.");
    createQuote(yoda, rank++, "The Force runs strong in your family.");
    createQuote(luke, rank++, "I am a Jedi, like my father before me.");
    createQuote(palpatine, rank++, "Now, young Skywalker, you will die.");
    createQuote(anakin, rank++, "Just for once, let me look on you with my own eyes.");
    createQuote(quigon, rank++, "There's always a bigger fish.");
    createQuote(nute, rank++, "In time, the suffering of your people will persuade you to see our point of view.");
    createQuote(shmi, rank++, "You can't stop the change, any more than you can stop the suns from setting.");
    createQuote(yoda, rank++, "Fear is the path to the dark side. Fear leads to anger; anger leads to hate; hate leads to suffering.");
    createQuote(obiwan, rank++, "Well, if droids could think, there'd be none of us here, would there?");
    createQuote(jamillia, rank++, "We must keep our faith in the Republic. The day we stop believing democracy can work is the day we lose it.");
    createQuote(jango, rank++, "I'm just a simple man trying to make my way in the universe.");
    createQuote(dooku, rank++, "What if I told you that the Republic was now under the control of a Dark Lord of the Sith?");
    createQuote(palpatine, rank++, "The dark side of the Force is a pathway to many abilities some consider to be unnatural.");
    createQuote(palpatine, rank++, "Power! Unlimited power!");
    createQuote(padme, rank++, "So this is how liberty dies. With thunderous applause.");
    createQuote(obiwan, rank++, "You were my brother, Anakin. I loved you.");
    createQuote(rey, rank++, "The garbage'll do!");
    createQuote(han, rank++, "Chewie, we're home.");
    createQuote(leia, rank++, "You know, no matter how much we fought, I've always hated watching you leave.");
    createQuote(c3po, rank++, "Oh, my dear friend. How I've missed you.");
    createQuote(chirrut, rank++, "I'm one with the Force. The Force is with me.");
    createQuote(cassian, rank++, "Every time I walked away from something I wanted to forget, I told myself it was for a cause that I believed in. A cause that was worth it. Without that, we're lost.");
    createQuote(k2so, rank++, "Jyn, I'll be there for you. Cassian said I had to.");
    createQuote(leia, rank++, "Hope.");

    cg::ParseClient::instance()->createAll(_quotes);
    QVERIFY(createSpy.wait(10000));
}

void ParseTest::deleteTestObjects()
{
    //for (auto *pFile : _files)
    //{
    //    cg::ParseClient::instance()->deleteFile(pFile->url(), PARSE_MASTER_KEY);
    //    QSignalSpy fileSpy(cg::ParseClient::instance(), &ParseClient::deleteFileFinished);
    //    QVERIFY(fileSpy.wait(10000));
    //}
    //_files.clear();

    cg::ParseClient::instance()->deleteAll(_quotes);
    QSignalSpy deleteSpy(cg::ParseClient::instance(), &ParseClient::deleteAllFinished);
    QVERIFY(deleteSpy.wait(10000));
    for (auto & pQuote : _quotes)
        delete pQuote;
    _quotes.clear();

    cg::ParseClient::instance()->deleteAll(_characters);
    QVERIFY(deleteSpy.wait(10000));

    for (auto & pCharacter : _characters)
        delete pCharacter;
    _characters.clear();
}

void ParseTest::cleanupTestCase()
{
    deleteTestObjects();
}

void ParseTest::testObject()
{
    ParseObject *gameScore = new ParseObject("TestGameScore");
    gameScore->setProperty("score", 1337);
    gameScore->setProperty("playerName", "Sean Plott");

    QVERIFY(gameScore->objectId().isEmpty());
    QCOMPARE(gameScore->isNew(), true);
    QCOMPARE(gameScore->isDirty(), true);
    QCOMPARE(gameScore->property("score").toInt(), 1337);

    gameScore->save();
    QSignalSpy saveSpy(gameScore, &ParseObject::saveFinished);
    QVERIFY(saveSpy.wait(10000));

    QVERIFY(!gameScore->objectId().isEmpty());
    QCOMPARE(gameScore->isNew(), false);
    QCOMPARE(gameScore->isDirty(), false);
    QString objectId = gameScore->objectId();

    gameScore->setProperty("score", 1338);
    QCOMPARE(gameScore->isDirty(), true);
    QCOMPARE(gameScore->isDirty("score"), true);

    gameScore->save();
    QVERIFY(saveSpy.wait(10000));

    QCOMPARE(gameScore->property("score").toInt(), 1338);

    gameScore->deleteObject();
    QSignalSpy deleteSpy(gameScore, &ParseObject::deleteFinished);
    QVERIFY(deleteSpy.wait(10000));

    delete gameScore;
}

void ParseTest::testUserLogin()
{
    ParseClient *client = ParseClient::instance();

#if 0
    ParseUser *testUser = new ParseUser();
    testUser->setUsername("TestLogin");
    testUser->setPassword("Parse123");
    testUser->setEmail("charles.glancy@gmail.com");
    QVERIFY(testUser->sessionToken().isEmpty());

    testUser->signUp();
    QSignalSpy signUpSpy(testUser, &ParseUser::signUpFinished);
    QVERIFY(signUpSpy.wait(10000));
#endif

    client->login("TestLogin", "Parse123");
    QSignalSpy loginSpy(client, &ParseClient::loginFinished);
    QVERIFY(loginSpy.wait(10000));

    ParseUser *currentUser = client->currentUser();
    QVERIFY(currentUser != nullptr);
    QVERIFY(currentUser->isAuthenticated());

    client->logout();
    QSignalSpy logoutSpy(client, &ParseClient::logoutFinished);
    QVERIFY(logoutSpy.wait(10000));

    currentUser = client->currentUser();
    QVERIFY(currentUser == nullptr);
}

void ParseTest::testUserSignUp()
{
    ParseUser *testUser = new ParseUser();
    testUser->setUsername("testUser1");
    testUser->setPassword("pa55w0rd");
    QVERIFY(testUser->sessionToken().isEmpty());

    testUser->signUp();
    QSignalSpy signUpSpy(testUser, &ParseUser::signUpFinished);
    QVERIFY(signUpSpy.wait(10000));

    QVERIFY(!testUser->sessionToken().isEmpty());

    testUser->deleteUser();
    QSignalSpy deleteSpy(testUser, &ParseUser::deleteUserFinished);
    QVERIFY(deleteSpy.wait(10000));

    delete testUser;
}

void ParseTest::testGetQuery()
{
    ParseObject *gameScore = new ParseObject("TestGameScore");
    gameScore->setProperty("score", 1337);
    gameScore->setProperty("playerName", "Sean Plott");
    gameScore->save();
    QSignalSpy saveSpy(gameScore, &ParseObject::saveFinished);
    QVERIFY(saveSpy.wait(10000));
    QString id = gameScore->objectId();

    ParseQuery *pQuery = new ParseQuery("TestGameScore");
    pQuery->get(id);
    QSignalSpy getSpy(pQuery, &ParseQuery::getFinished);
    QVERIFY(getSpy.wait(10000));

    QList<QVariant> arguments = getSpy.takeFirst();
    ParseObject *gotObject = qvariant_cast<ParseObject*>(arguments.at(0));
    QVERIFY(gotObject != nullptr);
    QVERIFY(gotObject->objectId() == id);

    gameScore->deleteObject();
    QSignalSpy deleteSpy(gameScore, &ParseObject::deleteFinished);
    QVERIFY(deleteSpy.wait(10000));

    delete gameScore;
}

int ParseTest::getListCount(QSignalSpy &spy, int argIndex)
{
    int count = 0;

    QList<QVariant> arguments = spy.takeFirst();
    QVariant variant = arguments.at(argIndex);
    if (variant.canConvert<QVariantList>())
    {
        QSequentialIterable iterable = variant.value<QSequentialIterable>();
        count = iterable.size();
    }

    return count;
}

void ParseTest::testFindAllQuery()
{
    ParseQuery *pQuery = new ParseQuery("TestCharacter");
    pQuery->find();
    QSignalSpy findSpy(pQuery, &ParseQuery::findFinished);
    QVERIFY(findSpy.wait(10000));

    QCOMPARE(getListCount(findSpy, 0), 20);

    delete pQuery;
}

void ParseTest::testCountQuery()
{
    ParseQuery *pQuery = new ParseQuery("TestQuote");
    pQuery->count();
    QSignalSpy countSpy(pQuery, &ParseQuery::countFinished);
    QVERIFY(countSpy.wait(10000));

    QList<QVariant> arguments = countSpy.takeFirst();
    QCOMPARE(arguments.at(0).toInt(), 32);

    delete pQuery;
}
