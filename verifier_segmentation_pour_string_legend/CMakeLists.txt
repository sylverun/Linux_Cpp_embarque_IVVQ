cmake_minimum_required(VERSION 3.5)

project(verifier_segmentation_pour_string_legend LANGUAGES C)
find_package(gd)
add_executable(pie_chart_verif main.c)
target_link_libraries(pie_chart_verif gd)
target_link_libraries(pie_chart_verif m)
install(TARGETS pie_chart_verif
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
