QT += network testlib websockets

TARGET = cgParseTest
CONFIG += testcase warn_on debug_and_release debug_and_release_target

TEMPLATE = app

SOURCES += parsetest.cpp

HEADERS += parsetest.h \
	parsesecret.h

INCLUDEPATH += ../include ../src

CONFIG(debug, debug|release) {
    LIBS += -L../src/debug -lcgParse
}
else {
    LIBS += -L../src/release -lcgParse
}
