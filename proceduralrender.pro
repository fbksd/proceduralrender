
TEMPLATE = app
QT       += core network
QT       -= gui

TARGET = proceduralrender

# C++11
QMAKE_CXXFLAGS += -std=c++0x -g

# OpenEXR
INCLUDEPATH += /usr/include/OpenEXR
DEPENDPATH += /usr/include/OpenEXR
LIBS += -lIlmImf

# Benchmark lib
INCLUDEPATH += /home/jonas/Documents/Doutorado/Documents/Benchmark/Code/BenchmarkSystem/include/
LIBS += -L/home/jonas/Documents/Doutorado/Documents/Benchmark/Code/build-BenchmarkSystem-Desktop-Debug/ -lRenderingServer -lCore

HEADERS += \
    src/server.h \
    src/exprtk.hpp \
    src/sampler.h \
    src/ProceduralRender.h \
    src/SceneReader.h

SOURCES += \
    src/server.cpp \
    src/main.cpp \
    src/sampler.cpp \
    src/ProceduralRender.cpp \
    src/SceneReader.cpp

