QT += network testlib

TARGET = cgParseTest
CONFIG += testcase warn_on debug_and_release debug_and_release_target

TEMPLATE = app

SOURCES += parsetest.cpp

HEADERS += parsetest.h \
	parsesecret.h

INCLUDEPATH += ../include ../src

CONFIG(debug, debug|release) {
    LIBS += -L../src/debug -lcgParse
    PRE_TARGETDEPS += ../src/debug/cgParse.lib
}
else {
    LIBS += -L../src/release -lcgParse
    PRE_TARGETDEPS += ../src/release/cgParse.lib
}
