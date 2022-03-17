include(gtest_dependency.pri)

QT -= core gui
QT += serialport printsupport widgets

CONFIG += c++11 console debug
CONFIG -= app_bundle

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
# CONFIG -= qt

INCLUDEPATH += "../qtapp"

SOURCES += \
        ../qtapp/ll_protocol.cpp \
        main.cpp         tst_test_case.cpp \
        qt_mock_callback.cpp

HEADERS += \
        ../qtapp/ll_protocol.h
