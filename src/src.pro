QT += core network
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
	parsefilehelper.h \
    parseobject.h \
	parseobjecthelper.h \
	parseobjectpointer.h \
	parsequery.h \
	parsequeryhelper.h \
	parserelation.h \
	parsereply.h \
	parserequest.h \
	parsetypes.h \
	parseuser.h \
	parseuserhelper.h
	
SOURCES += parseclient.cpp \
	parsefile.cpp \
	parsefilehelper.cpp \
	parseobject.cpp \
	parseobjecthelper.cpp \
	parseobjectpointer.cpp \
	parsequeryhelper.cpp \
	parsereply.cpp \
	parserequest.cpp \
	parserequestobject.cpp \
	parseuser.cpp \
	parseuserhelper.cpp


DEFINES += CGPARSE_EXPORTS

