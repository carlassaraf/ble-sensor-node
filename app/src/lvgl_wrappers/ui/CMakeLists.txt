SET(SOURCES components/ui_comp_topbar.c
    components/ui_comp.c
    screens/ui_scrBLE.c
    ui_theme_manager.c
    ui_themes.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_icons_signal_png.c
    images/ui_img_icons_left_png.c
    images/ui_img_icons_right_png.c
    fonts/ui_font_body12sb.c
    fonts/ui_font_caps11b.c
    fonts/ui_font_micro8b.c
    fonts/ui_font_mono17m.c
    fonts/ui_font_mono9.c
    fonts/ui_font_sub10.c
    fonts/ui_font_title17xb.c
    fonts/ui_font_value22b.c)

add_library(ui ${SOURCES})
