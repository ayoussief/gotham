# Copyright (c) 2023-present The Gotham Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://opensource.org/license/mit/.

function(generate_setup_nsi)
  set(abs_top_srcdir ${PROJECT_SOURCE_DIR})
  set(abs_top_builddir ${PROJECT_BINARY_DIR})
  set(CLIENT_URL ${PROJECT_HOMEPAGE_URL})
  set(CLIENT_TARNAME "gotham")
  set(GOTHAM_WRAPPER_NAME "gotham")
  set(GOTHAM_GUI_NAME "gotham-qt")
  set(GOTHAM_DAEMON_NAME "gothamd")
  set(GOTHAM_CLI_NAME "gotham-cli")
  set(GOTHAM_TX_NAME "gotham-tx")
  set(GOTHAM_WALLET_TOOL_NAME "gotham-wallet")
  set(GOTHAM_TEST_NAME "test_gotham")
  set(EXEEXT ${CMAKE_EXECUTABLE_SUFFIX})
  configure_file(${PROJECT_SOURCE_DIR}/share/setup.nsi.in ${PROJECT_BINARY_DIR}/gotham-win64-setup.nsi USE_SOURCE_PERMISSIONS @ONLY)
endfunction()
