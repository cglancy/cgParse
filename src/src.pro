QT += core network
QT -= gui

TARGET = cgParse
CONFIG += dll
TEMPLATE = lib
VERSION	= 0.1.1

SOURCES += parseclient.cpp \
	parseobject.cpp \
	parsequery.cpp \
	parseuser.cpp \

HEADERS += cgparse.h \
	parseclient.h \
	parseclient_p.h \
    parseobject.h \
	parsequery.h \
	parseuser.h

DEFINES += CGPARSE_EXPORTS

