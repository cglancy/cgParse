QT += core network
QT -= gui

TARGET = cgParse
CONFIG += dll
TEMPLATE = lib
VERSION	= 0.1.1

INCLUDEPATH += ../include

HEADERS +=  \
	../include/asyncfuture.h \
	../include/parse.h \
	../include/parseacl.h \
	../include/parseclient.h \
	../include/parsedatetime.h \
	../include/parseerror.h \
	../include/parsefile.h \
	../include/parsegeopoint.h \
	../include/parsehelperbase.h \
	../include/parseobject.h \
	../include/parseobjectpointer.h \
	../include/parsepolygon.h \
	../include/parsequery.h \
	../include/parsequeryhelper.h \
	../include/parserelation.h \
	../include/parseresult.h \
	../include/parserole.h \
	../include/parsesession.h \
 	../include/parsetypes.h \
	../include/parseuser.h \
	parsefilehelper.h \
	parseobjecthelper.h \
	parserequest.h \
	parsesessionhelper.h \
	parseuserhelper.h
	
SOURCES += parse.cpp \
	parseacl.cpp \
	parseclient.cpp \
	parsedatetime.cpp \
	parsefile.cpp \
	parsefilehelper.cpp \
	parsegeopoint.cpp \
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

