target_sources(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/screens/ui_scrBLE.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ui.c
    ${CMAKE_CURRENT_SOURCE_DIR}/components/ui_comp_hook.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ui_helpers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/images/ui_img_bg_main_png.c
    ${CMAKE_CURRENT_SOURCE_DIR}/images/ui_img_icon_temperature_png.c
    ${CMAKE_CURRENT_SOURCE_DIR}/images/ui_img_icon_humidity_png.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts/ui_font_Nunito28.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts/ui_font_Nunito20.c)

target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
