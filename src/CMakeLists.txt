cmake_minimum_required(VERSION 3.11)
project(bowling
  VERSION 0.2.0
  DESCRIPTION "Bowling Score Table"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Qt Generated Files")
set(QT_USE_IMPORTED_TARGETS TRUE)
find_package(Qt4 4.8.7 REQUIRED COMPONENTS QtCore QtGui)
include(${QT_USE_FILE})

add_executable(bowling WIN32)
target_sources(bowling PRIVATE
  main.cpp
  mainwindow.h mainwindow.cpp mainwindow.ui
  bowlingitemmodel.h bowlingitemmodel.cpp
  bowlingtableview.h bowlingtableview.cpp)

# add Qxt sources
target_sources(bowling PRIVATE
  qxt/qxtglobal.h
  qxt/qxtcsvmodel.h
  qxt/qxtcsvmodel.cpp)
source_group("qxt" REGULAR_EXPRESSION "qxt/.+")
target_compile_definitions(bowling PRIVATE QXT_STATIC)

target_link_libraries(bowling PRIVATE ${QT_LIBRARIES})

# install
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/../install" CACHE PATH "" FORCE)
endif()

# install main executable
install(TARGETS bowling RUNTIME DESTINATION ".")

# install runtime
set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
include(InstallRequiredSystemLibraries)

# install Qt
foreach(lib ${QT_LIBRARIES})
  string(REPLACE "Qt4::" "" lib_name ${lib})
  install(FILES "${QT_BINARY_DIR}/${lib_name}4.dll" DESTINATION ".")
endforeach()
