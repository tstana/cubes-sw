#-------------------------------------------------
#
# Project created by QtCreator 2017-02-07T15:23:08
#
#-------------------------------------------------

QT       += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cubescontrol
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/siphra/
INCLUDEPATH += $$PWD/cubesproto/
INCLUDEPATH += $$PWD/ui/

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cubesproto/cubescommand.cpp \
    cubesproto/cubesprotouartpmod.cpp \
    siphra/siphratreewidgetitem.cpp \
    ui/commsettingsdialog.cpp \
    ui/cubescontrol.cpp \
    main.cpp \

HEADERS  += \
    cubesproto/cubescommand.h \
    cubesproto/icubesprotocol.h \
    cubesproto/cubesprotouartpmod.h \
    siphra/siphratreewidgetitem.h \
    ui/commsettingsdialog.h \
    ui/cubescontrol.h \

FORMS    += \
    ui/commsettingsdialog.ui \
    ui/cubescontrol.ui

RESOURCES += \
    resources/resources.qrc
