# Copyright (c) 2024 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_AHT10 aht10.c)
zephyr_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)