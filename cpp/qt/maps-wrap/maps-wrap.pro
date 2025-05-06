

QT += widgets gui webenginewidgets webchannel


ROOT_DIR = $$_PRO_FILE_PWD_
ROOT_DIR ~= s!/cpp/qt.*!!


PROJECT_NAME = $$_PRO_FILE_PWD_
PROJECT_NAME ~= s!.*/!!



SRC_ROOT_DIR = $$ROOT_DIR/cpp/src

SRC_DIR = $$SRC_ROOT_DIR/$$PROJECT_NAME


DEFINES += ROOT_FOLDER=\\\"$$ROOT_DIR\\\"
DEFINES += SITE_ROOT=\\\"$$ROOT_DIR/site\\\"
DEFINES += SCREENSHOTS_FOLDER=\\\"$$ROOT_DIR/images/screenshots\\\"



INCLUDEPATH += $$SRC_ROOT_DIR
INCLUDEPATH += $$SRC_ROOT_DIR/utils


DEFINES += USING_KANS


HEADERS += \
  $$SRC_ROOT_DIR/utils/rdsc-qh/JsInterface.h \
  $$SRC_ROOT_DIR/utils/rdsc-qh/qh-web-engine-page.h \
  $$SRC_ROOT_DIR/utils/rdsc-qh/qh-web-engine-view.h \
  $$SRC_ROOT_DIR/utils/rdsc-qh/qh-web-page.h \
  $$SRC_ROOT_DIR/utils/rdsc-qh/qh-web-view-dialog.h \


SOURCES +=  \
  $$SRC_DIR/main.cpp \
  $$SRC_ROOT_DIR/utils/rdsc-qh/qh-web-engine-page.cpp \
  $$SRC_ROOT_DIR/utils/rdsc-qh/qh-web-engine-view.cpp \
  $$SRC_ROOT_DIR/utils/rdsc-qh/qh-web-page.cpp \
  $$SRC_ROOT_DIR/utils/rdsc-qh/qh-web-view-dialog.cpp \
