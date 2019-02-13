QT += core network testlib concurrent
QT -= gui

TARGET = cgParse
CONFIG += dll
TEMPLATE = lib
VERSION	= 0.1.1


HEADERS += cgparse.h \
	asyncfuture.h \
	parseclient.h \
	parseerror.h \
	parsefile.h \
    parseobject.h \
	parseobjectpointer.h \
	parsequery.h \
	parserelation.h \
	parsereply.h \
	parserequest.h \
	parserequestobject.h \
	parsetypes.h \
	parseuser.h
	
SOURCES += parseclient.cpp \
	parsefile.cpp \
	parseobject.cpp \
	parseobjectpointer.cpp \
	parsereply.cpp \
	parserequest.cpp \
	parserequestobject.cpp \
	parseuser.cpp


DEFINES += CGPARSE_EXPORTS

