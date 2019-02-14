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
#define CGPARSE_NO_EVENT_LOOP

#include "parsetest.h"
#include "parseclient.h"
#include "parseobject.h"
#include "parseuser.h"
#include "parsequery.h"
#include "parsefile.h"
#include "parserelation.h"
#include "parseuserhelper.h"

#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <asyncfuture.h>

// used to define your own PARSE_APPLICATION_ID, PARSE_CLIENT_API_KEY
#include "parsesecret.h"

#include <QTest>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QDebug>
#include <QScopedPointer>
#include <QFileInfo>
#include <QSequentialIterable>
#include <QScopedPointer>

template <typename T>
inline void await(QFuture<T> future, int timeout = -1) {
    if (future.isFinished()) {
        return;
    }

    QEventLoop loop;
    QFutureWatcher<T> watcher;
    QObject::connect(&watcher, SIGNAL(finished()), &loop, SLOT(quit()));

    watcher.setFuture(future);

    if (timeout > 0) {
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    }

    loop.exec();
}

using namespace cg;

QTEST_MAIN(ParseTest)

//
// TestMovie
//

TestMovie::TestMovie()
    : ParseObject("TestMovie")
{
}

TestMovie::TestMovie(const QString &title)
    : ParseObject("TestMovie")
{
    setTitle(title);
}

TestMoviePtr ParseTest::createMovie(const QString &title)
{
    TestMoviePtr movieObject = QSharedPointer<TestMovie>::create(title);
    _movies.append(movieObject);
    return movieObject;
}

//
// TestCharacter
//

TestCharacter::TestCharacter()
    : cg::ParseObject("TestCharacter")
{
}

TestCharacter::TestCharacter(const QString &name)
    : cg::ParseObject("TestCharacter")
{
    setName(name);
}

TestCharacterPtr ParseTest::createCharacter(const QString &name, const QString &imageFile)
{
    ParseFilePtr pFile;

    if (!imageFile.isEmpty())
    {
        QString imagePath = _testImagesDir.absolutePath() + "/" + imageFile;
        QFileInfo fi(imagePath);
        if (fi.exists())
        {
            pFile = QSharedPointer<ParseFile>::create(imagePath);
            auto saveFuture = pFile->save();
            await(saveFuture);
        }
        else
        {
            qDebug() << "Warning: Test image is missing: " << imageFile;
        }
    }

    TestCharacterPtr character = QSharedPointer<TestCharacter>::create(name);
    if (pFile)
        character->setPicture(pFile);

    _characters.append(character);
    return character;
}

//
// TestQuote
//

TestQuote::TestQuote()
    : cg::ParseObject("TestQuote")
{
}

TestQuote::TestQuote(TestMoviePtr movie, TestCharacterPtr character, int rank, const QString &quote)
    : cg::ParseObject("TestQuote")
{
    setMovie(movie);
    setCharacter(character);
    setRank(rank);
    setQuote(quote);
}

TestQuotePtr ParseTest::createQuote(TestMoviePtr movie, TestCharacterPtr character, int rank, const QString &quote)
{
    TestQuotePtr quoteObject = QSharedPointer<TestQuote>::create(movie, character, rank, quote);
    _quotes.append(quoteObject);
    return quoteObject;
}

//
// ParseTest
//

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
    {
        QFile file("C:\\Temp\\parselog.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QTextStream outStream(&file);
            outStream << localMsg << "\n";
            file.close();
        }
    }
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

void ParseTest::initTestCase()
{
    qInstallMessageHandler(myMessageOutput); // Install the handler
                                             
    // set name & version for user-agent
    QCoreApplication::setApplicationName("ParseTest");
    QCoreApplication::setApplicationVersion("0.1");

    ParseClient::instance()->initialize(PARSE_APPLICATION_ID, PARSE_CLIENT_API_KEY, "api.parse.buddy.com");
    ParseClient::instance()->setLoggingEnabled(true);

    QString appDirPath = QCoreApplication::applicationDirPath();
    _testImagesDir.setPath(appDirPath + "/../../../../test/images");

    deleteTestObjects();
    createTestObjects();
}

void ParseTest::createTestObjects()
{
    episode1 = createMovie("Star Wars: Episode I The Phantom Menace");
    episode2 = createMovie("Star Wars: Episode II Attack of the Clones");
    episode3 = createMovie("Star Wars: Episode III Revenge of the Sith");
    episode4 = createMovie("Star Wars: Episode IV A New Hope");
    episode5 = createMovie("Star Wars: Episode V The Empire Strikes Back");
    episode6 = createMovie("Star Wars: Episode VI Return of the Jedi");
    episode7 = createMovie("Star Wars: Episode VII The Force Awakens");
    episode8 = createMovie("Star Wars: Episode VIII The Last Jedi");
    rogue1 = createMovie("Rogue One: A Star Wars Story");

    auto future = cg::ParseObject::createAll(_movies);
    await(future);

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

    auto future2 = cg::ParseObject::createAll(_characters);
    await(future2);

    int rank = 1;
    QList<ParseObject*> quotes;
    createQuote(episode4, leia, rank++, "Help me, Obi-Wan Kenobi. You're my only hope.");
    createQuote(episode4, vader, rank++, "I find your lack of faith disturbing.");
    createQuote(episode4, han, rank++, "It's the ship that made the Kessel run in less than twelve parsecs.");
    createQuote(episode4, obiwan, rank++, "The Force will be with you. Always.");
    createQuote(episode4, leia, rank++, "Why, you stuck-up, half-witted, scruffy-looking nerf herder!");
    createQuote(episode4, han, rank++, "Never tell me the odds!");
    createQuote(episode5, yoda, rank++, "Do. Or do not. There is no try.");
    createQuote(episode5, vader, rank++, "No. I am your father.");
    createQuote(episode6, yoda, rank++, "The Force runs strong in your family.");
    createQuote(episode6, luke, rank++, "I am a Jedi, like my father before me.");
    createQuote(episode6, palpatine, rank++, "Now, young Skywalker, you will die.");
    createQuote(episode6, anakin, rank++, "Just for once, let me look on you with my own eyes.");
    createQuote(episode1, quigon, rank++, "There's always a bigger fish.");
    createQuote(episode1, nute, rank++, "In time, the suffering of your people will persuade you to see our point of view.");
    createQuote(episode1, shmi, rank++, "You can't stop the change, any more than you can stop the suns from setting.");
    createQuote(episode1, yoda, rank++, "Fear is the path to the dark side. Fear leads to anger; anger leads to hate; hate leads to suffering.");
    createQuote(episode1, obiwan, rank++, "Well, if droids could think, there'd be none of us here, would there?");
    createQuote(episode1, jamillia, rank++, "We must keep our faith in the Republic. The day we stop believing democracy can work is the day we lose it.");
    createQuote(episode2, jango, rank++, "I'm just a simple man trying to make my way in the universe.");
    createQuote(episode2, dooku, rank++, "What if I told you that the Republic was now under the control of a Dark Lord of the Sith?");
    createQuote(episode3, palpatine, rank++, "The dark side of the Force is a pathway to many abilities some consider to be unnatural.");
    createQuote(episode3, palpatine, rank++, "Power! Unlimited power!");
    createQuote(episode3, padme, rank++, "So this is how liberty dies. With thunderous applause.");
    createQuote(episode3, obiwan, rank++, "You were my brother, Anakin. I loved you.");
    createQuote(episode7, rey, rank++, "The garbage'll do!");
    createQuote(episode8, han, rank++, "Chewie, we're home.");
    createQuote(episode8, leia, rank++, "You know, no matter how much we fought, I've always hated watching you leave.");
    createQuote(episode8, c3po, rank++, "Oh, my dear friend. How I've missed you.");
    createQuote(rogue1, chirrut, rank++, "I'm one with the Force. The Force is with me.");
    createQuote(rogue1, cassian, rank++, "Every time I walked away from something I wanted to forget, I told myself it was for a cause that I believed in. A cause that was worth it. Without that, we're lost.");
    createQuote(rogue1, k2so, rank++, "Jyn, I'll be there for you. Cassian said I had to.");
    createQuote(rogue1, leia, rank++, "Hope.");

    auto future3 = cg::ParseObject::createAll(_quotes);
    await(future3);
}

void ParseTest::cleanupTestCase()
{
    deleteTestObjects();
}

void ParseTest::deleteTestObjects()
{
    auto pQuoteQuery = ParseQuery<TestQuote>::create();
    auto findFuture1 = pQuoteQuery->find();
    await(findFuture1);

    QList<TestQuotePtr> quotes = pQuoteQuery->results();
    if (quotes.size() > 0)
    {
        QList<ParseObjectPtr> objects;
        for (auto & pQuote : quotes)
            objects.append(pQuote);

        auto deleteFuture1 = ParseObject::deleteAll(objects);
        await(deleteFuture1);
    }

    auto pCharacterQuery = ParseQuery<TestCharacter>::create();
    auto findFuture2 = pCharacterQuery->find();
    await(findFuture2);

    QList<TestCharacterPtr> characters = pCharacterQuery->results();
    for (auto & pCharacter : characters)
    {
        ParseFilePtr pImageFile = pCharacter->picture();
        if (pImageFile)
        {
            auto deleteFuture2 = ParseFile::deleteFile(pImageFile->url(), PARSE_MASTER_KEY);
            await(deleteFuture2);
        }
    }

    if (characters.size() > 0)
    {
        QList<ParseObjectPtr> objects;
        for (auto & pCharacter : characters)
            objects.append(pCharacter);

        auto deleteFuture3 = ParseObject::deleteAll(objects);
        await(deleteFuture3);
    }

    auto pMovieQuery = QSharedPointer<ParseQuery<TestMovie>>::create();
    auto findFuture3 = pMovieQuery->find();
    await(findFuture3);

    QList<TestMoviePtr> movies = pMovieQuery->results();
    if (movies.size() > 0)
    {
        QList<ParseObjectPtr> objects;
        for (auto & pMovie : movies)
            objects.append(pMovie);

        auto deleteFuture4 = ParseObject::deleteAll(objects);
        await(deleteFuture4);
    }
}

QFuture<void> timeoutFunction(QTimer *timer)
{
    return AsyncFuture::observe(timer, &QTimer::timeout).future();
}

void ParseTest::testAsyncFuture()
{
    QTimer *timer = new QTimer();
    timer->setInterval(500);
    timer->setSingleShot(true);
    timer->start();
    QFuture<void> future = timeoutFunction(timer);
    await(future);
    QVERIFY(future.isFinished());
}

void ParseTest::testUserLogin()
{
#if 0
    ParseUserPtr testUser = ();
    testUser->setUsername("TestLogin");
    testUser->setPassword("Parse123");
    testUser->setEmail(PARSE_TEST_EMAIL);
    QVERIFY(testUser->sessionToken().isEmpty());

    auto signupFuture = testUser->signUp();
    await(signupFuture);
#endif

    QFuture<ParseUserReply> loginFuture = ParseUser::login("TestLogin", "Parse123");
    await(loginFuture);

    ParseUserReply reply = loginFuture.result();
    ParseUserPtr pUser = reply.user();
    QVERIFY(pUser != nullptr);
    QVERIFY(pUser->isAuthenticated());
    QVERIFY(pUser == ParseUser::currentUser());

    QFuture<int> logoutFuture = ParseUser::logout();
    await(logoutFuture);

    QVERIFY(logoutFuture.result() == 200);
    QVERIFY(nullptr == ParseUser::currentUser());
}

void ParseTest::testResetPassword()
{
#if 0
    auto future = ParseUser::requestPasswordReset(PARSE_TEST_EMAIL);
    await(future);
    QVERIFY(future.isFinished());
#endif
}

void ParseTest::testUserSignUp()
{
    ParseUserPtr testUser = QSharedPointer<ParseUser>::create();
    testUser->setUsername("testUser1");
    testUser->setPassword("pa55w0rd");
    QVERIFY(testUser->sessionToken().isEmpty());
    QVERIFY(testUser->isAuthenticated() == false);

    QFuture<ParseUserReply> signUpFuture = testUser->signUp();
    await(signUpFuture);

    QVERIFY(!testUser->sessionToken().isEmpty());
    QString sessionToken = testUser->sessionToken();

    QFuture<int> deleteFuture = testUser->deleteUser();
    await(deleteFuture);

    QScopedPointer<ParseUserHelper> pUserHelper(new ParseUserHelper());
    pUserHelper->deleteSession(sessionToken);
    QSignalSpy sessionSpy(pUserHelper.data(), &ParseUserHelper::deleteSessionFinished);
    QVERIFY(sessionSpy.wait(10000));
}

void ParseTest::testObject()
{
    ParseObjectPtr gameScore = ParseObject::create("TestGameScore");
    gameScore->setValue("score", 1337);
    gameScore->setValue("playerName", "Sean Plott");

    QVERIFY(gameScore->objectId().isEmpty());
    QCOMPARE(gameScore->isDirty(), true);
    QCOMPARE(gameScore->value("score").toInt(), 1337);

    QFuture<int> save1Future = gameScore->save();
    await(save1Future);

    QVERIFY(!gameScore->objectId().isEmpty());
    QCOMPARE(gameScore->isDirty(), false);
    QString objectId = gameScore->objectId();

    gameScore->setValue("score", 1338);
    QCOMPARE(gameScore->isDirty(), true);
    QCOMPARE(gameScore->isDirty("score"), true);

    QFuture<int> save2Future = gameScore->save();
    await(save2Future);

    QCOMPARE(gameScore->value("score").toInt(), 1338);

    QFuture<int> deleteFuture = gameScore->deleteObject();
    await(deleteFuture);
}

void ParseTest::testObjectRevert()
{
    ParseObjectPtr gameScore = ParseObject::create("TestGameScore");
    gameScore->setValue("score", 1337);
    gameScore->setValue("playerName", "Sean Plott");

    QVERIFY(gameScore->isDirty("score"));
    QVERIFY(gameScore->isDirty("playerName"));
    QVERIFY(gameScore->isDirty());
    gameScore->revert();
    QVERIFY(!gameScore->isDirty("score"));
    QVERIFY(!gameScore->isDirty("playerName"));
    QVERIFY(!gameScore->isDirty());
}

void ParseTest::testObjectArray()
{
    ParseObjectPtr gameScore = ParseObject::create("TestGameScore");
    gameScore->setValue("score", 1337);
    gameScore->setValue("playerName", "Sean Plott");

    QVariantList previousScores;
    previousScores << 124;
    previousScores << 1330;
    previousScores << 1;
    gameScore->addAll("previousScores", previousScores);

    QFuture<int> save1Future = gameScore->save();
    await(save1Future);

    QFuture<int> fetchFuture = gameScore->fetch();
    await(fetchFuture);

    QVariantList scores = gameScore->value("previousScores").toList();
    QCOMPARE(scores.size(), 3);
    QVERIFY(scores.contains(1));
    QVERIFY(scores.contains(1330));
    QVERIFY(scores.contains(124));

    QFuture<int> deleteFuture = gameScore->deleteObject();
    await(deleteFuture);
}

void ParseTest::testObjectRelation()
{
    auto charactersInEpisode4 = episode4->relation<TestCharacter>("characters");
    charactersInEpisode4->add(vader);
    charactersInEpisode4->add(c3po);
    charactersInEpisode4->add(leia);
    charactersInEpisode4->add(luke);
    charactersInEpisode4->add(obiwan);
    charactersInEpisode4->add(han);
    episode4->setRelation("characters", charactersInEpisode4);

    auto saveFuture = episode4->save();
    await(saveFuture);

    auto charactersQuery = charactersInEpisode4->query();
    auto findFuture = charactersQuery->find();
    await(findFuture);

    QList<TestCharacterPtr> characters = charactersQuery->results();
    QCOMPARE(characters.size(), 6);
}

void ParseTest::testGetQuery()
{
    auto pQuote = QSharedPointer<TestQuote>::create(episode1, luke, 1, "I have a very bad feeling about this.");
    auto saveFuture1 = pQuote->save();
    await(saveFuture1);

    QString id = pQuote->objectId();

    auto pQuery = QSharedPointer<ParseQuery<TestQuote>>::create();
    auto getFuture = pQuery->get(id);
    await(getFuture);

    TestQuotePtr gotQuote = pQuery->first();
    QVERIFY(gotQuote != nullptr);
    QVERIFY(gotQuote->objectId() == id);

    auto deleteFuture = pQuote->deleteObject();
    await(deleteFuture);
}

void ParseTest::testFindAllQuery()
{
    auto pQuery = QSharedPointer<ParseQuery<TestCharacter>>::create();
    auto findFuture = pQuery->find();
    await(findFuture);

    ParseJsonArrayReply jsonArrayReply = findFuture.result();
    QCOMPARE(jsonArrayReply.jsonArray().size(), 20);
}

void ParseTest::testCountQuery()
{
    auto pQuery = QSharedPointer<ParseQuery<TestQuote>>::create();
    auto countFuture = pQuery->count();
    await(countFuture);

    ParseCountReply countReply = countFuture.result();
    QCOMPARE(countReply.count(), 32);
}

void ParseTest::testOrderQuery()
{
    auto pAscendingQuery = ParseQuery<TestQuote>::create();
    pAscendingQuery->orderByAscending("rank");
    auto findFuture = pAscendingQuery->find();
    await(findFuture);

    QList<TestQuotePtr> quotes = pAscendingQuery->results();
    QCOMPARE(quotes.size(), 32);

    int prevRank = 0;
    for (auto & pQuote : quotes)
    {
        QVERIFY(pQuote->rank() > prevRank);
        prevRank = pQuote->rank();
    }

    auto pDescendingQuery = ParseQuery<TestQuote>::create();
    pDescendingQuery->orderByDescending("rank");
    auto findFuture2 = pDescendingQuery->find();
    await(findFuture2);

    prevRank++;
    quotes = pDescendingQuery->results();
    QCOMPARE(quotes.size(), 32);

    for (auto & pQuote : quotes)
    {
        QVERIFY(pQuote->rank() < prevRank);
        prevRank = pQuote->rank();
    }
}

void ParseTest::testComparisonQuery()
{
    {
        auto pEqualToQuery = ParseQuery<TestCharacter>::create();
        pEqualToQuery->whereEqualTo("name", "Yoda");
        auto findFuture = pEqualToQuery->find();
        await(findFuture);

        TestCharacterPtr pFoundCharacter = pEqualToQuery->first();
        QVERIFY(yoda->hasSameId(pFoundCharacter));
    }
}

void ParseTest::testFullTextQuery()
{
    auto pFullTextQuery = ParseQuery<TestQuote>::create();
    pFullTextQuery->whereFullText("quote", "force");
    auto findFuture = pFullTextQuery->find();
    await(findFuture);

    QList<TestQuotePtr> quotes = pFullTextQuery->results();
    QCOMPARE(quotes.size(), 4);

    QStringList objectIds;
    objectIds << obiwan->objectId();
    objectIds << yoda->objectId();
    objectIds << palpatine->objectId();
    objectIds << chirrut->objectId();

    for (auto & pQuote : quotes)
    {
        TestCharacterPtr pCharacter = pQuote->character();
        QVERIFY(pCharacter);
        if (pCharacter)
            QVERIFY(objectIds.contains(pCharacter->objectId()));
    }
}