# src/runtime/runtime.pro

TEMPLATE = lib          # Chceme knižnicu
CONFIG += staticlib     # Chceme statickú knižnicu
CONFIG += c++17

TARGET = ifa_runtime    # Názov výslednej knižnice (bude libifa_runtime.a)

# Definície (ak sú potrebné pre runtime)
DEFINES += ASIO_STANDALONE

# Cesty k include adresárom
INCLUDEPATH += \
    $$PWD/.. \                     # Pridaj adresár src/
    $$PWD/../../third_party/asio/include # Cesta k Asio z runtime adresára

# Zdrojové súbory runtime
SOURCES += \
    ifa_runtime_engine.cpp \
    ifa_runtime_udp.cpp \
    ifa_runtime_timers.cpp

# Hlavičkové súbory runtime (pre Qt Creator)
HEADERS += \
    ifa_runtime_engine.h \
    ifa_runtime_udp.h \
    ifa_runtime_timers.h

# Závislosti pre linker (pre Asio na Linuxe)
unix:LIBS += -lpthread

QMAKE_CXXFLAGS += -fsanitize=address -g
QMAKE_LFLAGS += -fsanitize=address