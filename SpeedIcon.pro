#-------------------------------------------------
#
# Project created by QtCreator 2015-12-13T22:50:21
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpeedIcon
TEMPLATE = app

win32: LIBS += -liphlpapi

SOURCES += main.cpp \
    settingsdialog.cpp \
    speedicon.cpp \
    sysnetinfo_w32.cpp

HEADERS  += \
    settingsdialog.h \
    speedicon.h \
    sysnetinfo.h

FORMS    += \
    settingsdialog.ui
