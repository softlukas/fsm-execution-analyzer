# src/runtime/runtime.pro

TEMPLATE = lib       
CONFIG += staticlib    
CONFIG += c++17

TARGET = ifa_runtime

DEFINES += ASIO_STANDALONE

INCLUDEPATH += \
    $$PWD/.. \                    
    $$PWD/../../third_party/asio/include 

SOURCES += \
    ifa_runtime_engine.cpp \
    ifa_runtime_udp.cpp \
    ifa_runtime_timers.cpp

HEADERS += \
    ifa_runtime_engine.h \
    ifa_runtime_udp.h \
    ifa_runtime_timers.h

QMAKE_CXXFLAGS += -w

unix:LIBS += -lpthread