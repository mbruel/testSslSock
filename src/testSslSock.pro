QT += core network
QT -= gui

TARGET = testSslSock
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

QMAKE_CXXFLAGS += -Wno-write-strings

TEMPLATE = app


SOURCES += main.cpp \
    connection.cpp

HEADERS += \
    connection.h
