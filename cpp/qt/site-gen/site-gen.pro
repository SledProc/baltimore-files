

ROOT_DIR = $$_PRO_FILE_PWD_
ROOT_DIR ~= s!/cpp/qt.*!!



PROJECT_NAME = $$_PRO_FILE_PWD_
PROJECT_NAME ~= s!.*/!!



SRC_ROOT_DIR = $$ROOT_DIR/cpp/src

SRC_DIR = $$SRC_ROOT_DIR/$$PROJECT_NAME


DEFINES += ROOT_DIR=\\\"$$ROOT_DIR\\\"
DEFINES += SITE_ROOT=\\\"$$ROOT_DIR/site\\\"


INCLUDEPATH += $$SRC_ROOT_DIR/qtcsv-qt4_qt5/include
INCLUDEPATH += $$SRC_ROOT_DIR/qtcsv-qt4_qt5

INCLUDEPATH += $$SRC_ROOT_DIR


DEFINES += USING_KANS


HEADERS += \
  $$SRC_DIR/qtcsv-qt4_qt5/sources/contentiterator.h \
  $$SRC_DIR/qtcsv-qt4_qt5/sources/filechecker.h


SOURCES +=  \
  $$SRC_DIR/main.cpp \
  $$SRC_ROOT_DIR/qtcsv-qt4_qt5/sources/contentiterator.cpp \
  $$SRC_ROOT_DIR/qtcsv-qt4_qt5/sources/reader.cpp \
  $$SRC_ROOT_DIR/qtcsv-qt4_qt5/sources/variantdata.cpp \
  $$SRC_ROOT_DIR/qtcsv-qt4_qt5/sources/writer.cpp \


