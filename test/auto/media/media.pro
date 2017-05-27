#isEmpty(TEMPLATE):TEMPLATE=app
TARGET       = tst_media
CONFIG      += testcase
QT           = core testlib
SOURCES     += tst_media.cpp

# Include:
INCLUDEPATH += ../../../include

# Dependancies:
HEADERS += ../../../src/core/media.h
SOURCES += ../../../src/core/media.cpp
