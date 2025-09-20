cmake_minimum_required(VERSION 3.20)

project(lcd VERSION 1.0.0 LANGUAGES CXX)
# Appeler le projet du même nom que le répertoire
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

find_library(RL readline)
find_library(SQL3 sqlite3)

# Pour intégrer des modules maison
file(GLOB_RECURSE lib_source my_lib/*.cpp)
add_library(_modules_maison STATIC ${lib_source})

# Pour intégrer les modules readline
# include_directories(/usr/include/readline)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} _modules_maison sqlite3 readline)

install(TARGETS ${PROJECT_NAME} DESTINATION "~/bin/")
