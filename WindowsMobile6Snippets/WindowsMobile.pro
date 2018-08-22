# QMAKE Configuration file
# Copyright (c) 2009, WOSH - Wide Open Smart Home 
# by Alessandro Polo - OpenSmartHome.com
# All rights reserved.
#######################################################
# Revision $Id: WindowsMobile.pro 380 2009-07-05 03:22:45Z alex $
#######################################################

CONFIG += console rtti stl exceptions thread moc
QT -= gui

DEPENDPATH += .
INCLUDEPATH += . ../../../

OBJECTS_DIR = ../../../../tmp/WindowsMobile

HEADERS += WindowsMobileImpl.h
SOURCES += WindowsMobileImpl.cpp

HEADERS += WindowsMobileBundle.h
SOURCES += WindowsMobileBundle.cpp

########################################################################
CONFIG += make_lib_static
#CONFIG += make_lib_shared
#CONFIG += make_plugin
#CONFIG += make_app_test
########################################################################


########################################################################
############################################################# LIB STATIC
make_lib_static  {
	message("WOSH Framework - WindowsMobile - LIB STATIC")

	TEMPLATE = lib
	CONFIG += staticlib
	TARGET = bundleWindowsMobile
	DESTDIR = ../../../../lib

}
########################################################################
############################################################# LIB SHARED
make_lib_shared {
	message("WOSH Framework - WindowsMobile - LIB SHARED")

	TEMPLATE = lib
	TARGET = bundleWindowsMobile
	DESTDIR = ../../../../lib

}
########################################################################
################################################################# PLUGIN
make_plugin {
	message("WOSH Framework - WindowsMobile - PLUGIN")

	TEMPLATE = lib
	TARGET = plugin_WindowsMobileBundle
	DESTDIR = ../../../../bin

	SOURCES += plugin_WindowsMobileBundle.cpp

}
########################################################################
########################################################################
############################################################### APP TEST
make_app_test {
	message("WOSH Framework - WindowsMobile - APP TEST")

	TEMPLATE = app
	TARGET = test_WindowsMobile
	DESTDIR = ../../../../bin

	LIBS += -L../../../../lib
	LIBS += -lcore

	HEADERS -= WindowsMobileBundle.h
	SOURCES -= WindowsMobileBundle.cpp

	SOURCES += test_WindowsMobile.cpp

}
########################################################################
