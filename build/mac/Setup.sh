#!/bin/sh

QTDIR=/Users/Charles/Qt/5.15.10/clang_64

export PARSEDIR=/Users/charles/cgParse
export PARSE_DEBUG_DIR=/Users/charles/build-cgParse-Desktop_Qt_5_15_10_clang_64bit-Debug
export PARSE_RELEASE_DIR=/Users/charles/build-cgParse-Desktop_Qt_5_15_10_clang_64bit-Release
export CGPARSE_TEST_DIR=$PARSEDIR/test


open -a "/Users/charles/Qt/Qt Creator.app" $PARSEDIR/cgParse.pro