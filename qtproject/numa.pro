TEMPLATE = lib
TARGET = rwlock
#CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    rw_bo_mcs.c

QMAKE_CFLAGS += -std=c99
LIBS += -lpthread

HEADERS += \
    rw_bo_msc.h \
    main.h


