# Copyright (c) 2025 The Gotham Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://opensource.org/license/mit/.

cmake_minimum_required(VERSION 3.16)
project(GothamCity)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

# Gotham City SDL2 GUI executable
add_executable(gotham-city
    main.cpp
    gotham_city_app.cpp
    gotham_city_gui.cpp
    renderer.cpp
    window_manager.cpp
    ui/screen.cpp
    ui/button.cpp
    ui/label.cpp
    ui/panel.cpp
    ui/text_input.cpp
    ui/progress_bar.cpp
    ui/ui_factory.cpp
    ui/layout_manager.cpp
    screens/splash_screen.cpp
    screens/main_screen.cpp
    screens/wallet_screen.cpp
    screens/send_screen.cpp
    screens/receive_screen.cpp
    screens/transactions_screen.cpp
    screens/settings_screen.cpp
    screens/console_screen.cpp
    utils/theme.cpp
    utils/font_manager.cpp
    utils/texture_manager.cpp
    utils/animation.cpp
    wallet/wallet_manager.cpp
    stubs/common/args.cpp
    stubs/common/init.cpp
    stubs/common/system.cpp
    stubs/interfaces/node.cpp
)

target_include_directories(gotham-city PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/stubs
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(gotham-city
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
)

target_compile_options(gotham-city PRIVATE ${SDL2_CFLAGS_OTHER})

# Install the executable
install(TARGETS gotham-city
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT GUI
)