TEMPLATE = subdirs
CONFIG  += ordered

SUBDIRS += $$PWD/src/src.pro
SUBDIRS += $$PWD/test/test.pro

###########################################################
#                     R E M A R K                         #
#                                                         #
# in Project > Run, add a new step (before running):      #
#                                                         #
# Command: windeployqt                                    #
# Argument:  player.exe                                   #
# Working directory: %{BuildDir}/src/debug/               #
# Working directory: %{BuildDir}/src/release/             #
#                                                         #
###########################################################

