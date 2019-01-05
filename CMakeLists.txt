cmake_minimum_required(VERSION 2.8.3 FATAL_ERROR)
project(ProceduralRender)

option(Procedural_SERVER "Enables server mode" ON)

set(CMAKE_CXX_STANDARD 14)

find_package(fbksd REQUIRED)

# Qt
set(CMAKE_AUTOMOC ON)
find_package(Qt5Core)
find_package(Qt5Network)

# OpenEXR
include_directories(/usr/include/OpenEXR)
set(OPENEXR_LIBRARIES IlmImf Half)

if(Procedural_SERVER)
    add_definitions(-DSERVER_MODE)
endif()

# header files
set(HEADERS src/plain_for.h
    src/ProceduralRender.h
    src/sampler.h
    src/SceneReader.h
    src/server.h
    src/exprtk.hpp
)

# source files
set(SRCS src/ProceduralRender.cpp
    src/sampler.cpp
    src/SceneReader.cpp
    src/server.cpp
    src/main.cpp
)

add_executable(ProceduralRender ${SRCS} ${HEADERS})
target_link_libraries(ProceduralRender PRIVATE Qt5::Core fbksd::renderer ${OPENEXR_LIBRARIES})

install(TARGETS ProceduralRender RUNTIME DESTINATION .)
install(FILES info.json DESTINATION .)
