QT += network testlib websockets

TARGET = cgParseTest

TEMPLATE = app

SOURCES += parsetest.cpp

HEADERS += parsetest.h \
	parsesecret.h

INCLUDEPATH += ../include ../src

CONFIG(debug, debug|release) {
    LIBS += -L../build-src-Desktop_Qt_5_15_4_clang_64bit-Debug -lcgParse
}
else {
    LIBS += -L../build-src-Desktop_Qt_5_15_4_clang_64bit-Release -lcgParse
}
