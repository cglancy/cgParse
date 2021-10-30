QT += network websockets

TARGET = cgParse
TEMPLATE = lib
CONFIG += c++17
win32:CONFIG += staticlib
VERSION	= 0.1.1

macx:DEFINES += CGPARSE_DLL CGPARSE_EXPORTS

INCLUDEPATH += ../include

HEADERS +=  \
	../include/parse.h \
	../include/parseacl.h \
	../include/parseclient.h \
	../include/parseconvert.h \
	../include/parsedatetime.h \
	../include/parseerror.h \
	../include/parsefile.h \
	../include/parsegeopoint.h \
	../include/parselivequeryclient.h \
	../include/parselivequerymodel.h \
	../include/parselivequerysubscription.h \
	../include/parseobject.h \
	../include/parseobjectpointer.h \
	../include/parsepolygon.h \
	../include/parsequery.h \
	../include/parsequeryhelper.h \
	../include/parsequeryimpl.h \
	../include/parsequerymodel.h \
	../include/parserelation.h \
	../include/parsereply.h \
	../include/parserole.h \
	../include/parsesession.h \
	../include/parsetypes.h \
	../include/parseuser.h \
	parseclientobject.h \
	parsefileimpl.h \
	parseobjecthelper.h \
	parseobjectimpl.h \
	parserequest.h \
	parsesessionhelper.h
	
SOURCES += parse.cpp \
	parseacl.cpp \
	parseclient.cpp \
	parseclientobject.cpp \
	parseconvert.cpp \
	parsedatetime.cpp \
	parsefile.cpp \
	parsefileimpl.cpp \
	parsegeopoint.cpp \
	parselivequeryclient.cpp \
	parselivequerymodel.cpp \
	parselivequerysubscription.cpp \
	parseobject.cpp \
	parseobjecthelper.cpp \
	parseobjectimpl.cpp \
	parseobjectpointer.cpp \
	parsequeryhelper.cpp \
	parsequeryimpl.cpp \
	parsequerymodel.cpp \
	parsereply.cpp \
	parserequest.cpp \
	parserole.cpp \
	parsesession.cpp \
	parsesessionhelper.cpp \
	parseuser.cpp


