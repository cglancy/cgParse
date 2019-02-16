QT += core network
QT -= gui

TARGET = cgParse
CONFIG += dll
TEMPLATE = lib
VERSION	= 0.1.1

INCLUDEPATH += ../include

HEADERS +=  \
	../include/parse.h \
	../include/parseacl.h \
	../include/parseclient.h \
	../include/parseerror.h \
	../include/parsefile.h \
 	../include/parsetypes.h \
	../include/parseobject.h \
	../include/parsequery.h \
	../include/parseresult.h \
	../include/parserole.h \
	../include/parsesession.h \
	../include/parseuser.h \
	../include/parserelation.h \
	asyncfuture.h \
	parsefilehelper.h \
	parsehelperbase.h \
	parseobjecthelper.h \
	parseobjectpointer.h \
	parsequeryhelper.h \
	parserequest.h \
	parsesessionhelper.h \
	parseuserhelper.h
	
SOURCES += parse.cpp \
	parseacl.cpp \
	parseclient.cpp \
	parsefile.cpp \
	parsefilehelper.cpp \
	parsehelperbase.cpp \
	parseobject.cpp \
	parseobjecthelper.cpp \
	parseobjectpointer.cpp \
	parsequeryhelper.cpp \
	parseresult.cpp \
	parserequest.cpp \
	parserole.cpp \
	parsesession.cpp \
	parsesessionhelper.cpp \
	parseuser.cpp \
	parseuserhelper.cpp


DEFINES += CGPARSE_EXPORTS

