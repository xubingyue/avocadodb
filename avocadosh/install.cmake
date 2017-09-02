# -*- mode: CMAKE; -*-
# these are the install targets for the client package.
# we can't use RUNTIME DESTINATION here.

set(STRIP_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_X}/cstrip")
add_custom_target(strip_install_client ALL)
strip_install_bin_and_config(avocadobench   ${STRIP_DIR} ${CMAKE_INSTALL_BINDIR} strip_install_client)
strip_install_bin_and_config(avocadodump    ${STRIP_DIR} ${CMAKE_INSTALL_BINDIR} strip_install_client)
strip_install_bin_and_config(avocadoimp     ${STRIP_DIR} ${CMAKE_INSTALL_BINDIR} strip_install_client)
strip_install_bin_and_config(avocadorestore ${STRIP_DIR} ${CMAKE_INSTALL_BINDIR} strip_install_client)
strip_install_bin_and_config(avocadoexport  ${STRIP_DIR} ${CMAKE_INSTALL_BINDIR} strip_install_client)
strip_install_bin_and_config(avocadosh      ${STRIP_DIR} ${CMAKE_INSTALL_BINDIR} strip_install_client)
strip_install_bin_and_config(avocadovpack   ${STRIP_DIR} ${CMAKE_INSTALL_BINDIR} strip_install_client)

install_command_alias(${BIN_ARANGOSH}
  ${CMAKE_INSTALL_BINDIR}
  foxx-manager)
install_config(foxx-manager)

