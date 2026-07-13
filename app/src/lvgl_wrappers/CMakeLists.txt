target_sources(app PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/lvgl_port.cpp
${CMAKE_CURRENT_SOURCE_DIR}/screen_ble.cpp
${CMAKE_CURRENT_SOURCE_DIR}/screen_climate.cpp)

add_subdirectory(ui)

# Give the exported ui library Zephyr's toolchain flags and include paths,
# then link it into the app. This avoids touching any SquareLine-generated file.
target_link_libraries(ui PRIVATE zephyr_interface)
target_include_directories(ui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app PRIVATE ui)