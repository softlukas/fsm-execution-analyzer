# src/gui_app/gui_app.pro

QT += core gui widgets network
CONFIG += c++17
# CONFIG += console # Odkomentuj ak treba debug výpisy

TARGET = IfaAutomatonTool
TEMPLATE = app

# Definície
DEFINES += ASIO_STANDALONE # Potrebné aj tu, ak GUI používa Asio (nepriamo cez runtime hlavičky?)

# Cesty k include adresárom
INCLUDEPATH += \
    $$PWD/.. \                      # Pridaj adresár src/
    $$PWD/../../third_party/asio/include \
    $$PWD/../../third_party/

# Zdrojové súbory GUI a ostatných modulov OKREM runtime
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    GraphicsView.cpp \
    mainWindowUtils.cpp \
    StateGraphicItem.cpp \
    ../core/Machine.cpp \
    ../core/State.cpp \
    ../core/Transition.cpp \
    ../core/Variable.cpp \
    ../core/Input.cpp \
    ../core/Output.cpp \
    ../core/MachineElement.cpp \
    ../codegen/CodeGenerator.cpp \
    ../persistence/JsonPersistance.cpp \
    ../persistence/json_conversions.cpp

# Hlavičkové súbory GUI a ostatných modulov OKREM runtime
HEADERS += \
    mainwindow.h \
    GraphicsView.h \
    mainWindowUtils.h \
    StateGraphicItem.h \
    ../core/Machine.h \
    ../core/State.h \
    ../core/Transition.h \
    ../core/Variable.h \
    ../core/Input.h \
    ../core/Output.h \
    ../core/MachineElement.h \
    ../codegen/CodeGenerator.h \
    ../persistence/JsonPersistance.h \
    ../persistence/json_conversions.h

# Formulár
FORMS += mainwindow.ui

# --- Linkovanie statickej knižnice ifa_runtime ---
# Povie qmake, kde má hľadať knižnice (-L) a ktorú knižnicu linkovať (-l)
# Predpokladáme, že knižnica bude v build adresári o úroveň vyššie
unix:LIBS += -L$$OUT_PWD/../runtime -lifa_runtime

# Ďalšie závislosti (napr. pre runtime knižnicu, ak ich linkuješ až tu)
unix:LIBS += -lpthread
unix:LIBS += -lstdc++fs