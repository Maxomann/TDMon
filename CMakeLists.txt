cmake_minimum_required(VERSION 3.15)
project(TDMon)

# This is needed in the top-level CMakeLists.txt file
# in order for VisualStudio to discover tests in lower level CMakeLists.txt
enable_testing()

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 2 COMPONENTS system window graphics network CONFIG REQUIRED)
find_package(TGUI CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

include_directories(./)

add_subdirectory(TDMon)
