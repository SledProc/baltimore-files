
ROOT_DIR = $$_PRO_FILE_PWD_


ROOT_DIR ~= s!/cpp/qt.*!!

SRC_DIR = $$ROOT_DIR/cpp/src


DEFINES += ROOT_DIR=\\\"$$ROOT_DIR\\\"
DEFINES += SITE_ROOT=\\\"$$ROOT_DIR/site\\\"


INCLUDEPATH += $$SRC_DIR/qtcsv-qt4_qt5/include
INCLUDEPATH += $$SRC_DIR/qtcsv-qt4_qt5


DEFINES += USING_KANS


HEADERS += \
  $$SRC_DIR/qtcsv-qt4_qt5/sources/contentiterator.h \
  $$SRC_DIR/qtcsv-qt4_qt5/sources/filechecker.h


SOURCES +=  \
  $$SRC_DIR/main.cpp \
  $$SRC_DIR/qtcsv-qt4_qt5/sources/contentiterator.cpp \
  $$SRC_DIR/qtcsv-qt4_qt5/sources/reader.cpp \
  $$SRC_DIR/qtcsv-qt4_qt5/sources/variantdata.cpp \
  $$SRC_DIR/qtcsv-qt4_qt5/sources/writer.cpp \


