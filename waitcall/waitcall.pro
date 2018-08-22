TEMPLATE = app
CONFIG += console rtti stl exceptions thread moc
QT -= gui

TARGET = waitcall
DESTDIR = 

DEPENDPATH += .
INCLUDEPATH += . .. ../../ldm/include $${POSTGRES_INCLUDE}

# HEADERS += gnokiiWaitCall.h

SOURCES += waitcall.cpp 
# gnokiiWaitCall.cpp

LIBS += -lgnokii

LIBS += -L/usr/local/lib

