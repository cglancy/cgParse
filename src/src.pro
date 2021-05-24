QT += network websockets

TARGET = cgParse
TEMPLATE = lib
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
	../include/parsequerymodel.h \
	../include/parserelation.h \
	../include/parsereply.h \
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
	parseconvert.cpp \
	parsedatetime.cpp \
	parsefile.cpp \
	parsefilehelper.cpp \
	parsegeopoint.cpp \
	parselivequeryclient.cpp \
	parselivequerymodel.cpp \
	parselivequerysubscription.cpp \
	parseobject.cpp \
	parseobjecthelper.cpp \
	parseobjectpointer.cpp \
	parsequeryhelper.cpp \
	parsequerymodel.cpp \
	parsereply.cpp \
	parserequest.cpp \
	parserole.cpp \
	parsesession.cpp \
	parsesessionhelper.cpp \
	parseuser.cpp \
	parseuserhelper.cpp


