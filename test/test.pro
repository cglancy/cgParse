QT += network testlib

TARGET = cgParseTest
CONFIG += testcase 

TEMPLATE = app

SOURCES += parsetest.cpp

HEADERS += parsetest.h \
	parsesecret.h

INCLUDEPATH += ../include ../src

CONFIG(debug, debug|release) {
    LIBS += -L../src/debug -lcgParse0
    PRE_TARGETDEPS += ../src/debug/cgParse0.dll
}
else {
    LIBS += -L../src/release -lcgParse0
    PRE_TARGETDEPS += ../src/release/cgParse0.dll
}
