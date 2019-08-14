TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .
QT += core widgets xml
CONFIG += qt debug

# Input
HEADERS += aussackler.h \
           assetsmodel.h \
           calc.h \
           docmodel.h \
           document.h \
           entry.h \
           entrymodel.h \
           entrywindow.h \
           invest.h \
           mainwindow.h \
           transaction.h \
           tsort.h \
           vat.h

FORMS +=   account.ui \
           category.ui \
           doclist.ui \
           document.ui \
           entry.ui \
           mainwindow.ui \
           settings.ui \
           vatcategory.ui

SOURCES += aussackler.cpp \
           assetsmodel.cpp \
           calc.cpp \
           docmodel.cpp \
           document.cpp \
           entry.cpp \
           entrymodel.cpp \
           entrywindow.cpp \
           invest.cpp \
           mainwindow.cpp \
           transaction.cpp \
           vat.cpp
