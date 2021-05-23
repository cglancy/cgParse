QT += network testlib websockets

TARGET = cgParseTest

TEMPLATE = app

DEFINES += CGPARSE_DLL

SOURCES += parsetest.cpp

HEADERS += parsetest.h \
	parsesecret.h

INCLUDEPATH += ../include ../src

CONFIG(debug, debug|release) {
    LIBS += -L$$(CGPARSE_DEBUG_DIR) -lcgParse0
}
else {
    LIBS += -L$$(CGPARSE_RELEASE_DIR) -lcgParse0
}
