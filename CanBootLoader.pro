#-------------------------------------------------
#
# Project created by QtCreator 2016-04-24T16:42:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CanBootLoader
TEMPLATE = app


SOURCES += main.cxx\
        widget.cxx \
    canDevice.cxx \
    threadCanTx.cxx \
    threadCanRx.cxx \
    i15765NetLayer.cxx

HEADERS  += widget.hxx \
    canDevice.hxx \
    icsnVC40.hxx \
    icsneo40DLLAPI.hxx \
    threadCanTx.hxx \
    threadCanRx.hxx \
    i15765NetLayer.hxx

FORMS    += widget.ui

RESOURCES += \
    res.qrc
