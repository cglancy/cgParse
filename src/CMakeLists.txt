cmake_minimum_required(VERSION 3.20)

add_library(cgParse STATIC
    ../include/parse.h
    ../include/parseacl.h
	../include/parseanalytics.h
    ../include/parseclient.h
    ../include/parseconvert.h
    ../include/parsedatetime.h
    ../include/parseerror.h
    ../include/parsefile.h
    ../include/parsegeopoint.h
    ../include/parsegraphql.h
    ../include/parselivequeryclient.h
    ../include/parselivequerymodel.h
    ../include/parselivequerysubscription.h
    ../include/parseobject.h
    ../include/parseobjectpointer.h
    ../include/parsepolygon.h
    ../include/parsequery.h
    ../include/parsequeryimpl.h
    ../include/parsequerymodel.h
    ../include/parsequeryrequest.h
    ../include/parserelation.h
    ../include/parsereply.h
	../include/parserequest.h
    ../include/parserole.h
    ../include/parsesession.h
    ../include/parseuser.h	
    parse.cpp
    parseacl.cpp
	parseanalytics.cpp
    parseclient.cpp
    parseconvert.cpp
    parsedatetime.cpp
    parsefile.cpp
    parsefileimpl.cpp
    parsefileimpl.h
    parsefilerequest.cpp
    parsefilerequest.h
    parsegeopoint.cpp
    parsegraphql.cpp
    parselivequeryclient.cpp
    parselivequerymodel.cpp
    parselivequerysubscription.cpp
    parseobject.cpp
    parseobjectimpl.cpp
    parseobjectimpl.h
    parseobjectpointer.cpp
    parseobjectrequest.cpp
    parseobjectrequest.h
    parsequeryimpl.cpp
    parsequerymodel.cpp
    parsequeryrequest.cpp
    parsereply.cpp
    parserequest.cpp
    parserole.cpp
    parsesession.cpp
    parseuser.cpp
    parseuserrequest.cpp
    parseuserrequest.h
)

target_include_directories(cgParse PUBLIC
	"${PROJECT_SOURCE_DIR}/include")

target_link_libraries(cgParse Qt6::Network Qt6::WebSockets)