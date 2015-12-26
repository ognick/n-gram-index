TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += indexer.cpp \
    indexer_impl.cpp \
    main.cpp

HEADERS += indexer.h \
    indexer_impl.h

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += "/usr/include/python2.7/"

LIBS += -lpython2.7
