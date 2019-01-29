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

// used to define your own PARSE_APPLICATION_ID, PARSE_CLIENT_API_KEY
#include "parsesecret.h"

#include <QTest>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QDebug>
#include <QScopedPointer>

using namespace cg;

QTEST_MAIN(ParseTest)

void ParseTest::initTestCase()
{
    ParseClient::instance()->initialize(PARSE_APPLICATION_ID, PARSE_CLIENT_API_KEY, "api.parse.buddy.com");

    // set name & version for user-agent
    QCoreApplication::setApplicationName("ParseTest");
    QCoreApplication::setApplicationVersion("0.1");

    qRegisterMetaType<ParseObject*>();
    qRegisterMetaType<ParseUser*>();
}

void ParseTest::cleanupTestCase()
{
}

void ParseTest::testParseObject()
{
    ParseObject *gameScore = new ParseObject("GameScore");
    gameScore->setProperty("score", 1337);
    gameScore->setProperty("playerName", "Sean Plott");

    QVERIFY(gameScore->objectId().isEmpty());
    QCOMPARE(gameScore->isNew(), true);
    QCOMPARE(gameScore->isDirty(), true);
    QCOMPARE(gameScore->property("score").toInt(), 1337);

    gameScore->save();
    QSignalSpy saveSpy(gameScore, SIGNAL(saveFinished(int)));
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
    QSignalSpy deleteSpy(gameScore, SIGNAL(deleteFinished(int)));
    QVERIFY(deleteSpy.wait(10000));

    delete gameScore;
}

void ParseTest::testParseObjectSubclass()
{

}

void ParseTest::testParseUser()
{
    ParseClient *client = ParseClient::instance();

    ParseUser *testUser = new ParseUser();
    testUser->setUsername("testUser1");
    testUser->setPassword("pa55w0rd");
    testUser->signUp();
    QSignalSpy signUpSpy(testUser, SIGNAL(signUpFinished(int)));
    QVERIFY(signUpSpy.wait(10000));

    client->login(testUser->username(), testUser->password());
    QSignalSpy loginSpy(client, SIGNAL(loginFinished(ParseUser*, int)));
    QVERIFY(loginSpy.wait(10000));

    ParseUser *currentUser = client->currentUser();
    QVERIFY(currentUser != nullptr);
    QVERIFY(currentUser->hasSameId(testUser));
    QVERIFY(currentUser->isAuthenticated());

    client->logout();
    QSignalSpy logoutSpy(client, SIGNAL(logoutFinished(int)));
    QVERIFY(logoutSpy.wait(10000));

    testUser->deleteUser();
    QSignalSpy deleteSpy(testUser, SIGNAL(deleteUserFinished(int)));
    QVERIFY(deleteSpy.wait(10000));

    delete testUser;
}