# Specify required Qt modules
QT += core gui widgets

# Set the C++ standard to C++17 as required by the assignment
CONFIG += c++17
# Optional: CONFIG -= app_bundle # Uncomment if you don't need macOS bundle specifics

# List all source files explicitly (assuming they are in the same directory as this .pro file)
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    State.cpp \
    Transition.cpp \
    Variable.cpp \
    Input.cpp \
    Output.cpp \
    # AutomatonLoader.cpp # Add this if you use it instead of Machine.cpp
    Machine.cpp \
    GraphicsView.cpp \
    JsonPersistance.cpp \
    MachineElement.cpp \# As seen in the image, maybe rename/replace later?
    json_conversions.cpp \
    CodeGenerator.cpp \
    mainwindowUtils.cpp \
    StateGraphicItem.cpp \
    # Add any other .cpp files here

# List all header files explicitly
HEADERS += \
    mainwindow.h \
    State.h \
    Transition.h \
    Variable.h \
    Input.h \
    Output.h \
    # AutomatonLoader.h # Add this if you use it instead of Machine.h
    Machine.h \
    GraphicsView.h \
    JsonPersistance.h \
    MachineElement.h \
    json_conversions.h \
    CodeGenerator.h \
    mainWindowUtils.h \
    StateGraphicItem.h \
    # As seen in the image, maybe rename/replace later?
    # Add any other .h files here

# List all UI form files explicitly
FORMS += \
    mainwindow.ui
    # Add any other .ui files here

# No need for explicit INCLUDEPATH for the project root directory,
# qmake/compiler usually search here by default.
# INCLUDEPATH += . # Optionally add current directory if needed

INCLUDEPATH += \
    $$PWD/third_party/asio/include \  
    $$PWD/third_party      

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt +6.0.0

# Default rules for deployment (usually safe to keep)
# --- Commented out deployment rules to fix "Extra characters" error ---
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target
# --- End of commented out section ---
