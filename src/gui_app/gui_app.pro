# src/gui_app/gui_app.pro

QT += core gui widgets network
CONFIG += c++17
# CONFIG += console 

TARGET = IfaAutomatonTool
TEMPLATE = app


DEFINES += ASIO_STANDALONE 


INCLUDEPATH += \
    $$PWD/.. \                      
    $$PWD/../../third_party/asio/include \
    $$PWD/../../third_party/


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    GraphicsView.cpp \
    mainWindowUtils.cpp \
    ../core/Machine.cpp \
    ../core/State.cpp \
    ../core/Transition.cpp \
    ../core/Variable.cpp \
    ../core/Input.cpp \
    ../core/Output.cpp \
    ../core/MachineElement.cpp \
    ../codegen/CodeGenerator.cpp \
    ../persistence/JsonPersistance.cpp \
    ../persistence/json_conversions.cpp \


HEADERS += \
    mainwindow.h \
    GraphicsView.h \
    mainWindowUtils.h \
    ../core/Machine.h \
    ../core/State.h \
    ../core/Transition.h \
    ../core/Variable.h \
    ../core/Input.h \
    ../core/Output.h \
    ../core/MachineElement.h \
    ../codegen/CodeGenerator.h \
    ../persistence/JsonPersistance.h \
    ../persistence/json_conversions.h \


FORMS += mainwindow.ui

unix:LIBS += -L$$OUT_PWD/../runtime -lifa_runtime


unix:LIBS += -lpthread
unix:LIBS += -lstdc++fs