cmake_minimum_required(VERSION 3.16)

project(MyToolBox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MyToolBox main.cpp)

include(GNUInstallDirs)
install(TARGETS MyToolBox
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
