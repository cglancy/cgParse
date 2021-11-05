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
#include "parserelation.h"
#include "parsesession.h"
#include "parsedatetime.h"
#include "parsepolygon.h"
#include "parsereply.h"
#include "parselivequeryclient.h"
#include "parselivequerysubscription.h"
#include "parsequerymodel.h"
#include "parselivequerymodel.h"
#include "parsegraphql.h"

#include <QTimer>
#include <QFile>
#include <QTextStream>

// used to define your own PARSE_APPLICATION_ID, PARSE_CLIENT_API_KEY
#include "parsesecret.h"

#include <QTest>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QDebug>
#include <QScopedPointer>
#include <QFileInfo>
#include <QSequentialIterable>

#define SPY_WAIT 10000

using namespace cg;

QTEST_MAIN(ParseTest)

//
// TestMovie
//

TestMovie::TestMovie()
    : ParseObject("TestMovie")
{
}

TestMovie::TestMovie(const TestMovie &movie)
    : ParseObject(movie)
{
}

TestMovie::TestMovie(const ParseObject& object)
{
    if (object.className() == "TestMovie")
        ParseObject::assign(object);
    else
        nullify();
}

TestMovie::TestMovie(const QString &title)
    : ParseObject("TestMovie")
{
    setTitle(title);
}

void TestMovie::assign(const ParseObject& object)
{
    if (object.className() == "TestMovie")
        ParseObject::assign(object);
    else
        nullify();
}

TestMovie& TestMovie::operator=(const TestMovie& other)
{
    ParseObject::operator=(other);
    return *this;
}

TestMovie ParseTest::createMovie(const QString &title)
{
    TestMovie movieObject = TestMovie(title);
    _movies.append(movieObject);
    return movieObject;
}

//
// TestCharacter
//

TestCharacter::TestCharacter()
    : ParseObject("TestCharacter")
{
}

TestCharacter::TestCharacter(const TestCharacter &character)
    : ParseObject(character)
{
}

TestCharacter::TestCharacter(const ParseObject& object)
{
    if (object.className() == "TestCharacter")
        ParseObject::assign(object);
    else
        nullify();
}

TestCharacter::TestCharacter(const QString &name)
    : cg::ParseObject("TestCharacter")
{
    setName(name);
}

void TestCharacter::assign(const ParseObject& object)
{
    if (object.className() == "TestCharacter")
        ParseObject::assign(object);
    else
        nullify();
}

TestCharacter& TestCharacter::operator=(const TestCharacter& other)
{
    ParseObject::operator=(other);
    return *this;
}

TestCharacter ParseTest::createCharacter(const QString &name, const QString &imageFile)
{
    ParseFile file;

    if (!imageFile.isEmpty())
    {
        QString imagePath = _testImagesDir.absolutePath() + "/" + imageFile;
        QFileInfo fi(imagePath);
        if (fi.exists())
        {
            file = ParseFile(imagePath);
            //ParseReply *pReply = file.save();
            //QSignalSpy saveSpy(pReply, &ParseReply::finished);
            //saveSpy.wait(SPY_WAIT);
            //pReply->deleteLater();
        }
        else
        {
            qDebug() << "Warning: Test image is missing: " << imageFile;
        }
    }

    TestCharacter character = TestCharacter(name);
    if (!file.isNull())
        character.setPicture(file);

    _characters.append(character);
    return character;
}

//
// TestQuote
//

TestQuote::TestQuote()
    : ParseObject("TestQuote")
{
}

TestQuote::TestQuote(const TestQuote &quote)
    : ParseObject(quote)
{
}

TestQuote::TestQuote(const ParseObject& object)
{
    if (object.className() == "TestQuote")
        ParseObject::assign(object);
    else
        nullify();
}

TestQuote::TestQuote(const TestMovie& movie, const TestCharacter& character, int rank, const QString &quote)
    : cg::ParseObject("TestQuote")
{
    setMovie(movie);
    setCharacter(character);
    setRank(rank);
    setQuote(quote);
}

void TestQuote::assign(const ParseObject& object)
{
    if (object.className() == "TestQuote")
        ParseObject::assign(object);
    else
        nullify();
}

TestQuote& TestQuote::operator=(const TestQuote& other)
{
    ParseObject::operator=(other);
    return *this;
}

TestQuote ParseTest::createQuote(const TestMovie& movie, const TestCharacter& character, int rank, const QString &quote)
{
    TestQuote quoteObject = TestQuote(movie, character, rank, quote);
    _quotes.append(quoteObject);
    return quoteObject;
}

//
// TestNamespace
//
ns1::ns2::TestNamespace::TestNamespace()
{
}

//
// ParseTest
//

static QString logFileName;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
    {
        if (!logFileName.isEmpty())
        {
            QFile file(logFileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            {
                QTextStream outStream(&file);
                outStream << localMsg << "\n";
                file.close();
            }
        }
        else
        {
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
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

    ParseClient::get()->initialize(PARSE_APPLICATION_ID, PARSE_CLIENT_API_KEY, PARSE_MASTER_KEY, PARSE_SERVER_URL);

    QByteArray testDir = qgetenv("CGPARSE_TEST_DIR");
    QVERIFY(!testDir.isEmpty());
    _testImagesDir.setPath(testDir + "/images");
    QVERIFY(_testImagesDir.exists());

    logFileName = qgetenv("CGPARSE_TEST_LOG");
    if (!logFileName.isEmpty())
    {
        ParseClient::get()->setLoggingEnabled(true);
        QFile::remove(logFileName);
    }

    deleteTestObjects();
    createTestObjects();

	ParseLiveQueryClient *pClient = ParseLiveQueryClient::get();

	pClient->open(PARSE_APPLICATION_ID, PARSE_LIVEQUERY_URL, PARSE_CLIENT_API_KEY);
	QSignalSpy openedSpy(pClient, &ParseLiveQueryClient::opened);
	QVERIFY(openedSpy.wait(SPY_WAIT));
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

    ParseReply* pSave1Reply = cg::ParseObject::saveAll(_movies);
    QSignalSpy save1Spy(pSave1Reply, &ParseReply::finished);
    QVERIFY(save1Spy.wait(SPY_WAIT));
    pSave1Reply->deleteLater();

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

    ParseReply *pSave2Reply = cg::ParseObject::saveAll(_characters);
    QSignalSpy save2Spy(pSave2Reply, &ParseReply::finished);
    QVERIFY(save2Spy.wait(SPY_WAIT));
    pSave2Reply->deleteLater();

    int rank = 1;
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

    ParseReply *pSave3Reply = cg::ParseObject::saveAll(_quotes);
    QSignalSpy save3Spy(pSave3Reply, &ParseReply::finished);
    QVERIFY(save3Spy.wait(SPY_WAIT));
    pSave3Reply->deleteLater();
}

void ParseTest::cleanupTestCase()
{
    deleteTestObjects();

	ParseLiveQueryClient *pClient = ParseLiveQueryClient::get();
	pClient->close();
}

void ParseTest::deleteTestObjects()
{
    auto quoteQuery = ParseQuery<TestQuote>();
    ParseReply *pFind1Reply = quoteQuery.find();
    QSignalSpy find1Spy(pFind1Reply, &ParseReply::finished);
    QVERIFY(find1Spy.wait(SPY_WAIT));
    pFind1Reply->deleteLater();

    QList<TestQuote> quotes = pFind1Reply->objects<TestQuote>();
    if (quotes.size() > 0)
    {
        QList<ParseObject> objects;
        for (auto & quote : quotes)
            objects.append(quote);

        ParseReply *pDelete1Reply = ParseObject::deleteAll(objects);
        QSignalSpy delete1Spy(pDelete1Reply, &ParseReply::finished);
        QVERIFY(delete1Spy.wait(SPY_WAIT));
        pDelete1Reply->deleteLater();
    }

    auto characterQuery = ParseQuery<TestCharacter>();
    ParseReply *pFind2Reply = characterQuery.find();
    QSignalSpy find2Spy(pFind2Reply, &ParseReply::finished);
    QVERIFY(find2Spy.wait(SPY_WAIT));
    pFind2Reply->deleteLater();

    QList<TestCharacter> characters = pFind2Reply->objects<TestCharacter>();
    for (auto & character : characters)
    {
        ParseFile imageFile = character.picture();

        if (!imageFile.isNull())
        {
            ParseReply* pDeleteReply = ParseFile::deleteFile(imageFile.url(), PARSE_MASTER_KEY);
            QSignalSpy deleteSpy(pDeleteReply, &ParseReply::finished);
            QVERIFY(deleteSpy.wait(SPY_WAIT));
            pDeleteReply->deleteLater();
        }

    }

    if (characters.size() > 0)
    {
        QList<ParseObject> objects;
        for (auto & character : characters)
            objects.append(character);

        ParseReply *pDelete3Reply = ParseObject::deleteAll(objects);
        QSignalSpy delete3Spy(pDelete3Reply, &ParseReply::finished);
        QVERIFY(delete3Spy.wait(SPY_WAIT));
        pDelete3Reply->deleteLater();
    }

    auto movieQuery = ParseQuery<TestMovie>();
    ParseReply *pFind3Reply = movieQuery.find();
    QSignalSpy find3Spy(pFind3Reply, &ParseReply::finished);
    QVERIFY(find3Spy.wait(SPY_WAIT));
    pFind3Reply->deleteLater();

    QList<TestMovie> movies = pFind3Reply->objects<TestMovie>();
    if (movies.size() > 0)
    {
        QList<ParseObject> objects;
        for (auto & movie : movies)
            objects.append(movie);

        ParseReply *pDelete4Reply = ParseObject::deleteAll(objects);
        QSignalSpy delete4Spy(pDelete4Reply, &ParseReply::finished);
        QVERIFY(delete4Spy.wait(SPY_WAIT));
        pDelete4Reply->deleteLater();
    }

    auto gameScoreQuery = ParseQuery<ParseObject>("TestGameScore");
    ParseReply* pFind4Reply = gameScoreQuery.find();
    QSignalSpy find4Spy(pFind4Reply, &ParseReply::finished);
    QVERIFY(find4Spy.wait(SPY_WAIT));
    pFind4Reply->deleteLater();

    QList<ParseObject> scores = pFind4Reply->objects<ParseObject>();
    if (scores.size() > 0)
    {
        ParseReply* pDelete5Reply = ParseObject::deleteAll(scores);
        QSignalSpy delete5Spy(pDelete5Reply, &ParseReply::finished);
        QVERIFY(delete5Spy.wait(SPY_WAIT));
        pDelete5Reply->deleteLater();
    }
}

void ParseTest::testVariant()
{
    ParseObject object = ParseObject("TestObject");
    object.setValue("name", "testObject1");

    QVariant variant = QVariant::fromValue(object);
    ParseObject object2 = variant.value<ParseObject>();
    QCOMPARE(object2.value("name").toString(), QString("testObject1"));

    ParseUser user = ParseUser::create();
    user.setUsername("testUser");

    QVariant userVariant = QVariant::fromValue(user);
    ParseUser user2 = userVariant.value<ParseUser>();
    QCOMPARE(user2.username(), QString("testUser"));

    object.setUser("user", user);
    ParseUser user3 = object.user("user");
    QCOMPARE(user3.username(), QString("testUser"));
}

void ParseTest::testDateTime()
{
    ParseDateTime dt;
    QVERIFY(dt.isNull());

    // never forget
    QDateTime qdt(QDate(2001, 9, 11), QTime(8, 46));
    ParseDateTime dt2(qdt);
    QDateTime qdt2 = dt2.toDateTime();
    QCOMPARE(qdt, qdt2);
}

void ParseTest::testGeoPoint()
{
    ParseGeoPoint seattle(47.6062, -122.3321);
    ParseGeoPoint portland(45.5122, -122.6587);

    // a long way on a bicycle
    double distance = seattle.distanceInMilesTo(portland);
    QVERIFY(std::abs(distance - 145.52) < 1.e-2);
}

void ParseTest::testPolygon()
{
    ParseGeoPoint sanJuan(18.4655, -66.1057);
    ParseGeoPoint bermuda(32.3078, -64.7505);
    ParseGeoPoint miami(25.7617, -80.1918);
    ParseGeoPoint bermudaTriangle(25.0000, -71.0000);

    ParsePolygon polygon;
    polygon << sanJuan;
    polygon << bermuda;
    polygon << miami;

    QVERIFY(polygon.containsPoint(bermudaTriangle));
}

void ParseTest::testUserLogin()
{
#if 0
    ParseUser testUser = ParseUser::create();
    testUser.setUsername("TestLogin");
    testUser.setPassword("Parse123");
    testUser.setEmail(PARSE_TEST_EMAIL);
    QVERIFY(testUser.sessionToken().isEmpty());

    ParseReply* pSignUpReply = testUser.signUp();
    QSignalSpy signUpSpy(pSignUpReply, &ParseReply::finished);
    QVERIFY(signUpSpy.wait(SPY_WAIT));

    QVERIFY(!pSignUpReply->isError());
    pSignUpReply->deleteLater();
#else
    ParseReply *pLoginReply = ParseUser::login("TestLogin", "Parse123");
    QSignalSpy loginSpy(pLoginReply, &ParseReply::finished);
    QVERIFY(loginSpy.wait(SPY_WAIT));

    QVERIFY(!pLoginReply->isError());
    pLoginReply->deleteLater();
#endif
    ParseUser currentUser = ParseUser::currentUser();
    QVERIFY(!currentUser.isNull());
    QVERIFY(currentUser.isAuthenticated());

    ParseReply *pLogoutReply = ParseUser::logout();
    QSignalSpy logoutSpy(pLogoutReply, &ParseReply::finished);
    QVERIFY(logoutSpy.wait(SPY_WAIT));

    QVERIFY(!pLogoutReply->isError());
    pLogoutReply->deleteLater();

    QVERIFY(ParseUser::currentUser().isNull());
}

void ParseTest::testUserResetPassword()
{
#if 0
    auto future = ParseUser::requestPasswordReset(PARSE_TEST_EMAIL);
    await(future);
    QVERIFY(future.isFinished());
#endif
}

void ParseTest::testUserSignUp()
{
#if 0
    QSKIP("not expected to work now that email confirmation is required");

    ParseUser testUser = ParseUser();
    testUser.setUsername("testUser1");
    testUser.setPassword("pa55w0rd");
    QVERIFY(testUser.sessionToken().isEmpty());
    QVERIFY(testUser.isAuthenticated() == false);

    ParseReply *pSignUpReply = testUser.signUp();
    QSignalSpy signUpSpy(pSignUpReply, &ParseReply::finished);
    QVERIFY(signUpSpy.wait(SPY_WAIT));
    pSignUpReply->deleteLater();

    QVERIFY(!testUser.sessionToken().isEmpty());
    QString sessionToken = testUser.sessionToken();

    ParseReply *pDeleteReply = testUser.deleteUser();
    QSignalSpy deleteSpy(pDeleteReply, &ParseReply::finished);
    QVERIFY(deleteSpy.wait(SPY_WAIT));
    pDeleteReply->deleteLater();

    ParseReply *pSessionReply = ParseSession::deleteSession(sessionToken);
    QSignalSpy sessionSpy(pSessionReply, &ParseReply::finished);
    QVERIFY(sessionSpy.wait(SPY_WAIT));
    pSessionReply->deleteLater();
#endif
}

void ParseTest::testObject()
{
    QDateTime dateTime(QDate(2019, 2, 24), QTime(10, 30));
    ParseDateTime pdt(dateTime);
    ParseGeoPoint portland(45.5122, -122.6587);

    ParseObject gameScore = ParseObject("TestGameScore");
    gameScore.setValue("score", 1337);
    gameScore.setValue("playerName", "Sean Plott");
    gameScore.setDateTime("date", dateTime);
    gameScore.setGeoPoint("location", portland);

    QVERIFY(gameScore.objectId().isEmpty());
    QCOMPARE(gameScore.isDirty(), true);
    QCOMPARE(gameScore.value("score").toInt(), 1337);

    ParseReply *pSave1Reply = gameScore.save();
    QSignalSpy save1Spy(pSave1Reply, &ParseReply::finished);
    QVERIFY(save1Spy.wait(SPY_WAIT));
    pSave1Reply->deleteLater();

    QVERIFY(!gameScore.objectId().isEmpty());
    QCOMPARE(gameScore.isDirty(), false);
    QString objectId = gameScore.objectId();

    QCOMPARE(gameScore.value("score").toInt(), 1337);
    QCOMPARE(gameScore.value("playerName").toString(), QString("Sean Plott"));
    QCOMPARE(gameScore.dateTime("date"), dateTime);

    ParseGeoPoint location = gameScore.geoPoint("location");
    QVERIFY(portland.distanceInMilesTo(location) < 1.e-6);

    gameScore.setValue("score", 1338);
    QCOMPARE(gameScore.isDirty(), true);
    QCOMPARE(gameScore.isDirty("score"), true);

    ParseReply *pSave2Reply = gameScore.save();
    QSignalSpy save2Spy(pSave2Reply, &ParseReply::finished);
    QVERIFY(save2Spy.wait(SPY_WAIT));
    pSave2Reply->deleteLater();

    QCOMPARE(gameScore.value("score").toInt(), 1338);

    ParseReply *pDeleteReply = gameScore.deleteObject();
    QSignalSpy deleteSpy(pDeleteReply, &ParseReply::finished);
    QVERIFY(deleteSpy.wait(SPY_WAIT));
    pDeleteReply->deleteLater();
}

void ParseTest::testObjectRevert()
{
    ParseObject gameScore = ParseObject("TestGameScore");
    gameScore.setValue("score", 1337);
    gameScore.setValue("playerName", "Sean Plott");

    QVERIFY(gameScore.isDirty("score"));
    QVERIFY(gameScore.isDirty("playerName"));
    QVERIFY(gameScore.isDirty());
    gameScore.revert();
    QVERIFY(!gameScore.isDirty("score"));
    QVERIFY(!gameScore.isDirty("playerName"));
    QVERIFY(!gameScore.isDirty());
}

void ParseTest::testObjectArray()
{
    ParseObject gameScore = ParseObject("TestGameScore");
    gameScore.setValue("score", 1337);
    gameScore.setValue("playerName", "Sean Plott");

    QList<int> previousScores;
    previousScores << 124;
    previousScores << 1330;
    previousScores << 1;
    gameScore.addAll("previousScores", previousScores);

    ParseReply *pSaveReply = gameScore.save();
    QSignalSpy saveSpy(pSaveReply, &ParseReply::finished);
    QVERIFY(saveSpy.wait(SPY_WAIT));
    pSaveReply->deleteLater();

    ParseReply *pFetchReply = gameScore.fetch();
    QSignalSpy fetchSpy(pFetchReply, &ParseReply::finished);
    QVERIFY(fetchSpy.wait(SPY_WAIT));
    pFetchReply->deleteLater();

    QList<int> scores = gameScore.list<int>("previousScores");
    QCOMPARE(scores.size(), 3);
    QVERIFY(scores.contains(1));
    QVERIFY(scores.contains(1330));
    QVERIFY(scores.contains(124));

    ParseReply *pDeleteReply = gameScore.deleteObject();
    QSignalSpy deleteSpy(pDeleteReply, &ParseReply::finished);
    QVERIFY(deleteSpy.wait(SPY_WAIT));
    pDeleteReply->deleteLater();
}

void ParseTest::testObjectRelation()
{
    auto charactersInEpisode4 = episode4.relation<TestCharacter>("characters");
    charactersInEpisode4.add(vader);
    charactersInEpisode4.add(c3po);
    charactersInEpisode4.add(leia);
    charactersInEpisode4.add(luke);
    charactersInEpisode4.add(obiwan);
    charactersInEpisode4.add(han);
    episode4.setRelation("characters", charactersInEpisode4);
    QCOMPARE(charactersInEpisode4.addList().size(), 6);

    auto charactersToTest = episode4.relation<TestCharacter>("characters");
    QCOMPARE(charactersToTest.addList().size(), 6);

    ParseReply *pSaveReply = episode4.save();
    QSignalSpy saveSpy(pSaveReply, &ParseReply::finished);
    QVERIFY(saveSpy.wait(SPY_WAIT));
    pSaveReply->deleteLater();

    auto charactersQuery = charactersInEpisode4.query();
    ParseReply *pFindReply = charactersQuery.find();
    QSignalSpy findSpy(pFindReply, &ParseReply::finished);
    QVERIFY(findSpy.wait(SPY_WAIT));
    pFindReply->deleteLater();

    QList<TestCharacter> characters = charactersQuery.results();
    QCOMPARE(characters.size(), 6);
}

void ParseTest::testObjectPointerHash()
{
    QHash<ParseObjectPointer, TestMovie> hash;
    hash.insert(episode1.toPointer(), episode1);
    hash.insert(episode2.toPointer(), episode2);
    hash.insert(episode3.toPointer(), episode3);
    hash.insert(episode4.toPointer(), episode4);
    hash.insert(episode5.toPointer(), episode5);
    hash.insert(episode6.toPointer(), episode6);

    TestMovie movie = hash.value(episode4.toPointer());
    QVERIFY(episode4.hasSameId(movie));
}

void ParseTest::testObjectReferenceSave()
{
    TestMovie soloMovie = createMovie("Solo: A Star Wars Story");
    TestCharacter qira = createCharacter("Qi'ra");
    TestQuote quote = createQuote(soloMovie, qira, 33, "You look good. A little rough around the edges, but good.");

    ParseReply *pSaveReply = quote.save();
    QSignalSpy saveSpy(pSaveReply, &ParseReply::finished);
    QVERIFY(saveSpy.wait(SPY_WAIT));

    QVERIFY(!quote.objectId().isEmpty());
    QVERIFY(!qira.objectId().isEmpty());
    QVERIFY(!soloMovie.objectId().isEmpty());

    ParseReply *pDelete1Reply = quote.deleteObject();
    QSignalSpy delete1Spy(pDelete1Reply, &ParseReply::finished);
    QVERIFY(delete1Spy.wait(SPY_WAIT));

    ParseReply *pDelete2Reply = qira.deleteObject();
    QSignalSpy delete2Spy(pDelete2Reply, &ParseReply::finished);
    QVERIFY(delete2Spy.wait(SPY_WAIT));

    ParseReply *pDelete3Reply = soloMovie.deleteObject();
    QSignalSpy delete3Spy(pDelete3Reply, &ParseReply::finished);
    QVERIFY(delete3Spy.wait(SPY_WAIT));
}


void ParseTest::testObjectSetObject()
{
    episode4.setObject("villian", vader);
    TestCharacter villian = episode4.object<TestCharacter>("villian");
    QVERIFY(vader == villian);
}

void ParseTest::testQueryNamespace()
{
    auto query = ParseQuery<ns1::ns2::TestNamespace>();
    QCOMPARE(query.className(), QString("TestNamespace"));
}

void ParseTest::testQueryGet()
{
    TestQuote quote = TestQuote(episode1, luke, 1, "I have a very bad feeling about this.");
    ParseReply *pSaveReply = quote.save();
    QSignalSpy saveSpy(pSaveReply, &ParseReply::finished);
    QVERIFY(saveSpy.wait(SPY_WAIT));
    pSaveReply->deleteLater();

    QString id = quote.objectId();

    auto query = ParseQuery<TestQuote>();
    ParseReply *pGetReply = query.get(id);
    QSignalSpy getSpy(pGetReply, &ParseReply::finished);
    QVERIFY(getSpy.wait(SPY_WAIT));
    pGetReply->deleteLater();

    TestQuote gotQuote = pGetReply->first<TestQuote>();
    QVERIFY(!gotQuote.isNull());
    QVERIFY(gotQuote.objectId() == id);

    ParseReply *pDeleteReply = quote.deleteObject();
    QSignalSpy deleteSpy(pDeleteReply, &ParseReply::finished);
    QVERIFY(deleteSpy.wait(SPY_WAIT));
    pDeleteReply->deleteLater();
}

void ParseTest::testQueryFindAll()
{
    auto query = ParseQuery<TestCharacter>();
    ParseReply *pFindReply = query.find();
    QSignalSpy findSpy(pFindReply, &ParseReply::finished);
    QVERIFY(findSpy.wait(SPY_WAIT));
    pFindReply->deleteLater();

    QList<TestCharacter> objects = pFindReply->objects<TestCharacter>();
    QCOMPARE(objects.size(), 20);
}

void ParseTest::testQueryCount()
{
    auto pQuery = ParseQuery<TestQuote>();
    ParseReply *pCountReply = pQuery.count();
    QSignalSpy countSpy(pCountReply, &ParseReply::finished);
    QVERIFY(countSpy.wait(SPY_WAIT));
    pCountReply->deleteLater();

    QCOMPARE(pCountReply->count(), 32);
}

void ParseTest::testQueryOrder()
{
    auto pAscendingQuery = ParseQuery<TestQuote>();
    pAscendingQuery.orderByAscending("rank");
    ParseReply *pFindReply = pAscendingQuery.find();
    QSignalSpy findSpy(pFindReply, &ParseReply::finished);
    QVERIFY(findSpy.wait(SPY_WAIT));
    pFindReply->deleteLater();

    QList<TestQuote> quotes = pFindReply->objects<TestQuote>();
    QCOMPARE(quotes.size(), 32);

    int prevRank = 0;
    for (auto & quote : quotes)
    {
        QVERIFY(quote.rank() > prevRank);
        prevRank = quote.rank();
    }

    auto pDescendingQuery = ParseQuery<TestQuote>();
    pDescendingQuery.orderByDescending("rank");
    ParseReply *pFind2Reply = pDescendingQuery.find();
    QSignalSpy find2Spy(pFind2Reply, &ParseReply::finished);
    QVERIFY(find2Spy.wait(SPY_WAIT));
    pFind2Reply->deleteLater();

    prevRank++;
    quotes = pFind2Reply->objects<TestQuote>();
    QCOMPARE(quotes.size(), 32);

    for (auto & quote : quotes)
    {
        QVERIFY(quote.rank() < prevRank);
        prevRank = quote.rank();
    }
}

void ParseTest::testQueryComparison()
{
    {
        auto pEqualToQuery = ParseQuery<TestCharacter>();
        pEqualToQuery.whereEqualTo("name", "Yoda");
        ParseReply *pFindReply = pEqualToQuery.find();
        QSignalSpy findSpy(pFindReply, &ParseReply::finished);
        QVERIFY(findSpy.wait(SPY_WAIT));
        pFindReply->deleteLater();

        TestCharacter foundCharacter = pFindReply->first<TestCharacter>();
        QVERIFY(yoda.hasSameId(foundCharacter));
    }
}

void ParseTest::testQueryFullText()
{
    auto pFullTextQuery = ParseQuery<TestQuote>();
    pFullTextQuery.whereFullText("quote", "force");
    ParseReply *pFindReply = pFullTextQuery.find();
    QSignalSpy findSpy(pFindReply, &ParseReply::finished);
    QVERIFY(findSpy.wait(SPY_WAIT));
    pFindReply->deleteLater();

    QList<TestQuote> quotes = pFullTextQuery.results();
    QCOMPARE(quotes.size(), 4);

    QStringList objectIds;
    objectIds << obiwan.objectId();
    objectIds << yoda.objectId();
    objectIds << palpatine.objectId();
    objectIds << chirrut.objectId();

    for (auto & quote : quotes)
    {
        TestCharacter character = quote.character();
        QVERIFY(!character.isNull());
        QVERIFY(objectIds.contains(character.objectId()));
    }
}

void ParseTest::testQueryOr()
{
    QList<ParseQuery<TestQuote>> list;

    auto pAnakinQuery = ParseQuery<TestQuote>();
    pAnakinQuery.whereEqualTo("character", anakin);
    list.append(pAnakinQuery);

    auto pVaderQuery = ParseQuery<TestQuote>();
    pVaderQuery.whereEqualTo("character", vader);
    list.append(pVaderQuery);

    auto pQuery = ParseQuery<TestQuote>::orQuery(list);
    ParseReply *pFindReply = pQuery.find();
    QSignalSpy findSpy(pFindReply, &ParseReply::finished);
    QVERIFY(findSpy.wait(SPY_WAIT));
    pFindReply->deleteLater();

    QList<TestQuote> quotes = pQuery.results();
    QCOMPARE(quotes.size(), 3);
}

void ParseTest::testQueryModel()
{
    QStringList keys;
    keys << Parse::ObjectIdKey;
    keys << "rank";
    keys << "quote";

    QStringList orderList;
    orderList << "rank";
    QVariantMap queryMap;
    queryMap.insert("order", orderList);

    QVariantMap movieTitle{ {"title", "Star Wars: Episode IV A New Hope"} };
    QVariantMap movieWhere{ {"where", movieTitle}, { "className", "TestMovie" } };
    QVariantMap innerQuery{ {"$inQuery", movieWhere} };
    QVariantMap movieQuery{ {"movie", innerQuery} };
    queryMap["where"] = movieQuery;

    QScopedPointer<ParseQueryModel> pQueryModel(new ParseQueryModel);
    pQueryModel->setClassName("TestQuote");
    pQueryModel->setKeys(keys);
    pQueryModel->setQuery(queryMap);
    ParseReply *pFindReply = pQueryModel->findWithReply();
    QSignalSpy findSpy(pFindReply, &ParseReply::finished);
    QVERIFY(findSpy.wait(SPY_WAIT));

    int count = pQueryModel->rowCount();
    QCOMPARE(count, 6);

    QHash<int, QByteArray> names = pQueryModel->roleNames();
    QCOMPARE(names.size(), 3);

    QModelIndex index = pQueryModel->index(0);
    int rank = pQueryModel->data(index, Qt::UserRole + 1).toInt();
    QString quote = pQueryModel->data(index, Qt::UserRole + 2).toString();

    QCOMPARE(rank, 1);
    QCOMPARE(quote, QString("Help me, Obi-Wan Kenobi. You're my only hope."));

    pFindReply->deleteLater();
}

void ParseTest::testFile()
{
    QString imagePath = _testImagesDir.absolutePath() + "/star_wars.jpeg";

    ParseFile file = ParseFile(imagePath);
    QVERIFY(file.url().isEmpty());
    QByteArray data1 = file.data();

    ParseReply *pSaveReply = file.save();
    QSignalSpy saveSpy(pSaveReply, &ParseReply::finished);
    QVERIFY(saveSpy.wait(SPY_WAIT));
    pSaveReply->deleteLater();

    QVERIFY(!file.url().isEmpty());

    ParseReply* pFetchReply = file.fetch();
    QSignalSpy fetchSpy(pFetchReply, &ParseReply::finished);
    QVERIFY(fetchSpy.wait(SPY_WAIT));
    QByteArray data2 = pFetchReply->data();
    pFetchReply->deleteLater();

    QCOMPARE(data2, data1);

    ParseReply* pDeleteReply = ParseFile::deleteFile(file.url(), PARSE_MASTER_KEY);
    QSignalSpy deleteSpy(pDeleteReply, &ParseReply::finished);
    QVERIFY(deleteSpy.wait(SPY_WAIT));
    pDeleteReply->deleteLater();
}

void ParseTest::testLiveQueryClient()
{
#if 0
    QJsonObject whereObject;
    whereObject.insert("playerName", "Charles");

    QJsonObject queryObject;
    queryObject.insert("className", "TestGameScore");
    queryObject.insert("where", whereObject);

    ParseLiveQueryClient *pClient = ParseLiveQueryClient::get();
    ParseLiveQuerySubscription *pSubscription = pClient->subscribe(queryObject);
    QSignalSpy subscribedSpy(pClient, &ParseLiveQueryClient::subscribed);
    QVERIFY(subscribedSpy.wait(SPY_WAIT));

    ParseObject gameScore = ParseObject::create("TestGameScore");
    gameScore.setValue("score", 42);
    gameScore.setValue("playerName", "Charles");
    gameScore.save();

    QSignalSpy createEventSpy(pSubscription, &ParseLiveQuerySubscription::createEvent);
    QVERIFY(createEventSpy.wait(SPY_WAIT));

    gameScore.setValue("score", 43);
    gameScore.save();

    QSignalSpy updateEventSpy(pSubscription, &ParseLiveQuerySubscription::updateEvent);
    QVERIFY(updateEventSpy.wait(SPY_WAIT));

    gameScore.deleteObject();

    QSignalSpy deleteEventSpy(pSubscription, &ParseLiveQuerySubscription::deleteEvent);
    QVERIFY(deleteEventSpy.wait(SPY_WAIT));

    pSubscription->unsubscribe();
    QSignalSpy unsubscribedEventSpy(pClient, &ParseLiveQueryClient::unsubscribed);
    QVERIFY(unsubscribedEventSpy.wait(SPY_WAIT));

    pSubscription->deleteLater();
#endif
}

void ParseTest::testLiveQueryModel()
{
#if 0
	QVariantMap whereMap;
	whereMap.insert("playerName", "Charles");

	QVariantMap queryMap;
	queryMap.insert("where", whereMap);

	QScopedPointer<ParseLiveQueryModel> pQueryModel(new ParseLiveQueryModel);
	pQueryModel->setClassName("TestGameScore");
	pQueryModel->setQuery(queryMap);
	pQueryModel->subscribe();

	QSignalSpy subscribedSpy(pQueryModel.data(), &ParseLiveQueryModel::subscribed);
	QVERIFY(subscribedSpy.wait(SPY_WAIT));

	ParseObject gameScore = ParseObject::create("TestGameScore");
	gameScore.setValue("score", 42);
	gameScore.setValue("playerName", "Charles");
	gameScore.save();

	QSignalSpy createEventSpy(pQueryModel.data(), &ParseLiveQueryModel::rowsInserted);
	QVERIFY(createEventSpy.wait(SPY_WAIT));

	gameScore.setValue("score", 43);
	gameScore.save();

	QSignalSpy updateEventSpy(pQueryModel.data(), &ParseLiveQueryModel::dataChanged);
	QVERIFY(updateEventSpy.wait(SPY_WAIT));

	gameScore.deleteObject();

	QSignalSpy deleteEventSpy(pQueryModel.data(), &ParseLiveQueryModel::rowsRemoved);
	QVERIFY(deleteEventSpy.wait(SPY_WAIT));
#endif
}

void ParseTest::testGraphQL()
{
    ParseGraphQL query("query FindTestMovie { testMovies { count } }");
    ParseReply* reply = new ParseReply(query);

    QSignalSpy querySpy(reply, &ParseReply::finished);
    QVERIFY(querySpy.wait(SPY_WAIT));

    QVariantMap result = reply->graphQLResult();
    QVariantMap dataMap = result.value("data").toMap();
    QVariantMap moviesMap = dataMap.value("testMovies").toMap();
    int count = moviesMap.value("count").toInt();

    QCOMPARE(count, 9);
}

