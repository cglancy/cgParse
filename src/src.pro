QT += core network
QT -= gui

TARGET = cgParse
CONFIG += dll
TEMPLATE = lib
VERSION	= 0.1.1

SOURCES += parseclient.cpp \
	parsefile.cpp \
	parseobject.cpp \
	parsequery.cpp \
	parserelation.cpp \
	parseuser.cpp \
	parseutil.cpp

HEADERS += cgparse.h \
	parseclient.h \
	parseclient_p.h \
	parsefile.h \
    parseobject.h \
	parsequery.h \
	parserelation.h \
	parseuser.h \
	parseutil.h

DEFINES += CGPARSE_EXPORTS

