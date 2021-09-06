CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
PROJECT(dijkstraAlgorithm VERSION 1.0.0 LANGUAGES CXX)

# Set debug flags (comment on release mode)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

FILE(GLOB_RECURSE headers src/*.h)
FILE(GLOB_RECURSE sources src/*.cpp)
FILE(GLOB_RECURSE qtRessources src/*.qrc)
FILE(GLOB_RECURSE qtWindowUI src/*.ui)

# Setup QT5
find_package(Qt5 COMPONENTS Widgets Qml Quick Charts Core PrintSupport Script REQUIRED)
IF(Qt5_FOUND)
    # The Qt5Widgets_INCLUDES also includes the include directories for
    # dependencies QtCore and QtGui
    include_directories(${Qt5Widgets_INCLUDE_DIRS} ${QtQml_INCLUDE_DIRS})

    # We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
    add_definitions(${Qt5Widgets_DEFINITIONS} ${QtQml_DEFINITIONS} ${${Qt5Quick_DEFINITIONS}})

    # Executables fail to build with Qt 5 in the default configuration
    # without -fPIE. We add that here.
    set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

    # Add qt ressources
    qt5_add_resources(QT_RESOURCES ${qtRessources})
    # Add ui
    qt5_wrap_ui(QT_UIs ${qtWindowUI})

    # Wrap cpp
    # qt5_wrap_cpp(MOC_SRCS ${headers}) 

    ADD_EXECUTABLE(${PROJECT_NAME} ${sources} ${headers} ${QT_RESOURCES} ${QT_UIs})

    # Add qt5 widgets to library
    target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Qml Qt5::Quick Qt5::Core Qt5::Charts Qt5::Gui Qt5::PrintSupport Qt5::Script qwt-qt5)
ENDIF()

find_package(OpenMP)
IF(OPENMP_FOUND)
    # add flags for OpenMP
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_SHARED_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    target_link_libraries(${PROJECT_NAME}  ${OpenMP_CXX_LIBRARIES})
ENDIF()
include_directories(SYSTEM ${OpenMP_INCLUDE_PATH})


find_package(OpenCL REQUIRED)
IF(OPENCL_FOUND)
    include_directories(${OpenCL_INCLUDE_DIRS})
    message("${OpenCL_INCLUDE_DIRS}")
    link_directories(${OpenCL_LIBRARY})
    target_link_libraries (${PROJECT_NAME} OpenCL::OpenCL)
ENDIF()

message("Found OpenCL: " OPENCL_FOUND)