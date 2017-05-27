TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += $$PWD/src/src.pro
SUBDIRS += $$PWD/test/test.pro

###########################################################
#                     R E M A R K                         #
#                                                         #
# MusicZapper depends on 3rd-party multimedia libraries   #
# provided with the Qt5 framework.                        #
#                                                         #
# These libraries MUST be deployed with the binary,       #
# otherwise there's a certain risk that the app doesn't   #
# read correctly some media formats.                      #
#                                                         #
# To do it:                                               #
#                                                         #
# in Project > Run, add a new step (before running):      #
#                                                         #
# Command: windeployqt                                    #
# Argument:  player.exe                                   #
# Working directory: %{BuildDir}/src/debug/               #
# Working directory: %{BuildDir}/src/release/             #
#                                                         #
###########################################################

