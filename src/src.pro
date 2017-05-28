#-------------------------------------------------
# MusicZapper created by QtCreator
#-------------------------------------------------
TEMPLATE = app
TARGET = MusicZapper
QT      += network
QT      += xml
QT      += multimedia
QT      += multimediawidgets
QT      += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG  += no_keyword

# QT += c++11
QMAKE_CXXFLAGS += -std=c++11

lessThan(QT_VERSION, 5.0) {
    warning("prefere to build it with Qt 5.0")
}

#-------------------------------------------------
# INCLUDE
#-------------------------------------------------
INCLUDEPATH += $$PWD/../include/


#-------------------------------------------------
# SOURCES
#-------------------------------------------------
include($$PWD/core/core.pri)
include($$PWD/gui/gui.pri)
include($$PWD/widgets/widgets.pri)

HEADERS += \
    $$PWD/about.h \
    $$PWD/builddefs.h \
    $$PWD/globals.h \
    $$PWD/mainwindow.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp

FORMS += \
    $$PWD/mainwindow.ui


#maemo* {
    DEFINES += PLAYER_NO_COLOROPTIONS
#}

#target.path = $$[QT_INSTALL_EXAMPLES]/multimediawidgets/player
#INSTALLS += target


#-------------------------------------------------
# RESOURCES
#-------------------------------------------------
RESOURCES = $$PWD/resources.qrc

win32|unix {
    RC_FILE = $$PWD/resources_win.rc
}

#macx {
#    ICON = $$PWD/src/icons/logo.icns
#}


#-------------------------------------------------
# BUILD OPTIONS
#-------------------------------------------------

# Rem: On Ubuntu, directories starting with '.' are hidden by default
win32 {
    MOC_DIR =      ./.moc
    OBJECTS_DIR =  ./.obj
    UI_DIR =       ./.ui
} else {
    MOC_DIR =      ./_moc
    OBJECTS_DIR =  ./_obj
    UI_DIR =       ./_ui
}

#-------------------------------------------------
# OUTPUT
#-------------------------------------------------


