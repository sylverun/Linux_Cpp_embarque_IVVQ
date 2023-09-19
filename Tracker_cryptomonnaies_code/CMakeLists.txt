cmake_minimum_required(VERSION 3.5)

project(Tracker_crypto LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_executable(Tracker_crypto main.cpp)

target_include_directories(Tracker_crypto PRIVATE /usr/include)

# Ajouter les options de compilation et de liaison
target_compile_options(Tracker_crypto PRIVATE -O3)
target_link_libraries(Tracker_crypto PRIVATE -lm -lpthread -lX11)
target_link_libraries(Tracker_crypto PRIVATE -L/usr/lib -lboost_filesystem -lboost_system -lboost_iostreams)
install(TARGETS Tracker_crypto
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
find_package(CURL REQUIRED)
target_link_libraries(Tracker_crypto PRIVATE CURL::libcurl)


