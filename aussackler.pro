TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QT += xml
CONFIG += debug

# Input
HEADERS += aussackler.h \
           docmodel.h \
           document.h \
           entry.h \
           entrymodel.h \
           entrywindow.h \
           invest.h \
           mainwindow.h \
           transaction.h \
           tsort.h

FORMS +=   account.ui \
           category.ui \
           doclist.ui \
           document.ui \
           entry.ui \
           mainwindow.ui

SOURCES += aussackler.cpp \
           docmodel.cpp \
           document.cpp \
           entry.cpp \
           entrymodel.cpp \
           entrywindow.cpp \
           invest.cpp \
           mainwindow.cpp \
           transaction.cpp
