QT += core network
QT -= gui

TARGET = cgParse
CONFIG += dll
TEMPLATE = lib
VERSION	= 0.1.1


HEADERS += cgparse.h \
	parseclient.h \
	parseclient_p.h \
	parsefile.h \
    parseobject.h \
	parsequery.h \
	parserelation.h \
	parsereply.h \
	parserequest.h \
	parseuser.h \
	parseutil.h
	
SOURCES += parseclient.cpp \
	parsefile.cpp \
	parseobject.cpp \
	parsequery.cpp \
	parserelation.cpp \
	parsereply.cpp \
	parserequest.cpp \
	parseuser.cpp \
	parseutil.cpp


DEFINES += CGPARSE_EXPORTS

