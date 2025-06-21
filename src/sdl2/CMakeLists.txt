# Copyright (c) 2025 The Gotham Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://opensource.org/license/mit/.

# The gotham-city SDL2 GUI sources
add_library(gothamsdl2 STATIC EXCLUDE_FROM_ALL
  gotham_city_app.cpp
  gotham_city_app.h
  gotham_city_gui.cpp
  gotham_city_gui.h
  renderer.cpp
  renderer.h
  window_manager.cpp
  window_manager.h
  ui/screen.cpp
  ui/screen.h
  ui/button.cpp
  ui/button.h
  ui/label.cpp
  ui/label.h
  ui/panel.cpp
  ui/panel.h
  ui/text_input.cpp
  ui/text_input.h
  ui/progress_bar.cpp
  ui/progress_bar.h
  ui/ui_factory.cpp
  ui/ui_factory.h
  ui/ui_utils.cpp
  ui/ui_utils.h
  ui/layout_manager.cpp
  ui/layout_manager.h
  ui/navigation_manager.cpp
  ui/navigation_manager.h
  ui/navigation_bar.cpp
  ui/navigation_bar.h
  ui/persistent_layout.cpp
  ui/persistent_layout.h
  screens/splash_screen.cpp
  screens/splash_screen.h
  screens/main_screen.cpp
  screens/main_screen.h
  screens/wallet_screen.cpp
  screens/wallet_screen.h
  screens/send_screen.cpp
  screens/send_screen.h
  screens/receive_screen.cpp
  screens/receive_screen.h
  screens/transactions_screen.cpp
  screens/transactions_screen.h
  screens/settings_screen.cpp
  screens/settings_screen.h
  screens/console_screen.cpp
  screens/console_screen.h
  utils/theme.cpp
  utils/theme.h
  utils/font_manager.cpp
  utils/font_manager.h
  utils/texture_manager.cpp
  utils/texture_manager.h
  utils/animation.cpp
  utils/animation.h
  wallet/wallet_manager.cpp
  wallet/wallet_manager.h
  ../init/gotham-gui.cpp
)

target_include_directories(gothamsdl2
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(gothamsdl2
  PUBLIC
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
  PRIVATE
    core_interface
    gotham_common
    gotham_util
    gotham_cli
    leveldb
    Boost::headers
)

target_compile_options(gothamsdl2 PRIVATE ${SDL2_CFLAGS_OTHER})

if(ENABLE_WALLET)
  target_sources(gothamsdl2
    PRIVATE
      # Add wallet-specific sources here if needed
  )
  target_link_libraries(gothamsdl2
    PRIVATE
      gotham_wallet
  )
endif()

add_executable(gotham-city
  main.cpp
)

target_link_libraries(gotham-city
  PRIVATE
    gothamsdl2
    core_interface
    gotham_node
    gotham_ipc
)

include(GNUInstallDirs)
install(TARGETS gotham-city
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT GUI
)

# Add gotham-city to the install_binary_component function
include(InstallBinaryComponent)
install_binary_component(gotham-city)