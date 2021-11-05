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
	../include/parsegraphql.h \
	../include/parselivequeryclient.h \
	../include/parselivequerymodel.h \
	../include/parselivequerysubscription.h \
	../include/parseobject.h \
	../include/parseobjectpointer.h \
	../include/parsepolygon.h \
	../include/parsequery.h \
	../include/parsequeryimpl.h \
	../include/parsequeryrequest.h \
	../include/parsequerymodel.h \
	../include/parserelation.h \
	../include/parsereply.h \
	../include/parserole.h \
	../include/parsesession.h \
	../include/parseuser.h \
	parsefileimpl.h \
	parsefilerequest.h \
	parseobjectimpl.h \
	parseobjectrequest.h \
	parserequest.h \
	parseuserrequest.h
	
SOURCES += parse.cpp \
	parseacl.cpp \
	parseclient.cpp \
	parseconvert.cpp \
	parsedatetime.cpp \
	parsefile.cpp \
	parsefileimpl.cpp \
	parsefilerequest.cpp \
	parsegeopoint.cpp \
	parsegraphql.cpp \
	parselivequeryclient.cpp \
	parselivequerymodel.cpp \
	parselivequerysubscription.cpp \
	parseobject.cpp \
	parseobjectimpl.cpp \
	parseobjectpointer.cpp \
	parseobjectrequest.cpp \
	parsequeryimpl.cpp \
	parsequerymodel.cpp \
	parsequeryrequest.cpp \
	parsereply.cpp \
	parserequest.cpp \
	parserole.cpp \
	parsesession.cpp \
	parseuser.cpp \
	parseuserrequest.cpp


