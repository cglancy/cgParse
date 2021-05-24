QT += network testlib websockets

TARGET = cgParseTest
CONFIG += c++17

TEMPLATE = app

macx:DEFINES += CGPARSE_DLL

SOURCES += parsetest.cpp

HEADERS += parsetest.h \
	parsesecret.h

INCLUDEPATH += ../include ../src


CONFIG(debug, debug|release) {
    LIBS += -L$$(PARSE_DEBUG_DIR) -lcgParse
}
else {
    LIBS += -L$$(PARSE_RELEASE_DIR) -lcgParse
}

