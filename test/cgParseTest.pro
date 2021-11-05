QT += network testlib websockets

TARGET = cgParseTest
CONFIG += c++17

TEMPLATE = app

include(../cgParse.pri)

SOURCES += parsetest.cpp

HEADERS += parsetest.h \
	parsesecret.h



