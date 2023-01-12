# Copyright 2023 Girish Palya
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# sdk in .local/opt, which is automatically searched
# set(ZEPHYR_SDK_INSTALL_DIR
#     ${CMAKE_HOME_DIRECTORY}/../../local
#     CACHE PATH 'dir where zephyr sdk is installed'
# )

find_package(Zephyr REQUIRED HINTS ../zephyr)

project(
    app
    LANGUAGES C
    VERSION 1.0.0
)

configure_file(app_version.h.in ${CMAKE_BINARY_DIR}/app/include/app_version.h)
target_include_directories(app PRIVATE ${CMAKE_BINARY_DIR}/app/include src)

target_sources(app PRIVATE src/main.c)
