target_sources(app PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/lvgl_port.cpp
${CMAKE_CURRENT_SOURCE_DIR}/screens.cpp)

add_subdirectory(ui)