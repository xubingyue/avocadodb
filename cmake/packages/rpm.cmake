# -*- mode: CMAKE; -*-

set(CPACK_GENERATOR "RPM")
if (CMAKE_DEBUG_FILENAMES_SHA_SUM)
  set(CPACK_DEBUG_DIRECTORY_PATTERN "/usr/lib*/debug/.build-id/*")
else()
  set(CPACK_DEBUG_DIRECTORY_PATTERN "/usr/lib*/debug/*")
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Installation/rpm/avocadodb.spec.in" "${CMAKE_CURRENT_BINARY_DIR}/avocadodb.spec" @ONLY IMMEDIATE)
set(CPACK_RPM_USER_BINARY_SPECFILE "${CMAKE_CURRENT_BINARY_DIR}/avocadodb.spec")


################################################################################
# This produces the RPM packages, using client/rpm.txt for the second package.
################################################################################

# deploy the Init script:
if (RPM_DISTRO STREQUAL "SUSE13")
  set(RPM_INIT_SCRIPT "${PROJECT_SOURCE_DIR}/Installation/rpm/rc.avocadod.OpenSuSE_13")
elseif (RPM_DISTRO STREQUAL "SUSE")
  set(RPM_INIT_SCRIPT "${PROJECT_SOURCE_DIR}/Installation/rpm/rc.avocadod.OpenSuSE")
else () # fall back to centos:
  set(RPM_INIT_SCRIPT "${PROJECT_SOURCE_DIR}/Installation/rpm/rc.avocadod.Centos")
endif()

set(RPM_INIT_SCRIPT_TARGET "${CMAKE_INSTALL_FULL_SYSCONFDIR}/init.d")
set(RPM_INIT_SCRIPT_TARGET_NAME avocadodb3)
set(CPACK_COMPONENTS_GROUPING IGNORE)

install(
  FILES ${RPM_INIT_SCRIPT}
  PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  DESTINATION ${RPM_INIT_SCRIPT_TARGET}
  RENAME ${RPM_INIT_SCRIPT_TARGET_NAME}
  )


if (NOT SYSTEMD_FOUND)
  # deploy the logrotate config:
  install(
    FILES ${PROJECT_BINARY_DIR}/avocadod.sysv
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/logrotate.d
    RENAME ${SERVICE_NAME})
else()
  message("SYSTEMD_FOUND ${SYSTEMD_FOUND}")
endif()
#
set(CPACK_CLIENT_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-client${PACKAGE_VERSION}")
set(CPACK_DBG_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-debuginfo${PACKAGE_VERSION}")
set(CPACK_RPM_PACKAGE_RELOCATABLE FALSE)

# set(CPACK_RPM_PACKAGE_DEBUG TRUE)

set(CPACK_TEMPORARY_DIRECTORY         "${PROJECT_BINARY_DIR}/_CPack_Packages/${CMAKE_SYSTEM_NAME}/RPM/RPMS/${AVOCADODB_PACKAGE_ARCHITECTURE}")
set(CPACK_TEMPORARY_PACKAGE_FILE_NAME "${CPACK_TEMPORARY_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.rpm")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_X ${PROJECT_BINARY_DIR}/bin)
include(avocadosh/dbg.cmake)
include(avocadod/dbg.cmake)

add_custom_target(package-arongodb-server
  COMMAND ${CMAKE_COMMAND} .
  COMMAND ${CMAKE_CPACK_COMMAND} -G RPM
  COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_TEMPORARY_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.rpm        ${PROJECT_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_TEMPORARY_DIRECTORY}/${CPACK_CLIENT_PACKAGE_FILE_NAME}.rpm ${PROJECT_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_TEMPORARY_DIRECTORY}/${CPACK_DBG_PACKAGE_FILE_NAME}.rpm    ${PROJECT_BINARY_DIR}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
list(APPEND PACKAGES_LIST package-arongodb-server)

#################################################################################
## hook to build the client package
#################################################################################
add_custom_target(copy_rpm_packages
  COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_PACKAGE_FILE_NAME}.rpm        ${PACKAGE_TARGET_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_CLIENT_PACKAGE_FILE_NAME}.rpm ${PACKAGE_TARGET_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy ${CPACK_DBG_PACKAGE_FILE_NAME}.rpm    ${PACKAGE_TARGET_DIR})

list(APPEND COPY_PACKAGES_LIST copy_rpm_packages)

add_custom_target(remove_packages
  COMMAND ${CMAKE_COMMAND} -E remove_directory _CPack_Packages
  COMMAND ${CMAKE_COMMAND} -E remove ${CPACK_PACKAGE_FILE_NAME}.rpm
  COMMAND ${CMAKE_COMMAND} -E remove ${CPACK_CLIENT_PACKAGE_FILE_NAME}.rpm
  COMMAND ${CMAKE_COMMAND} -E remove ${CPACK_DBG_PACKAGE_FILE_NAME}.rpm
  COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_BINARY_DIR}/bin/strip/*
  )



list(APPEND CLEAN_PACKAGES_LIST remove_packages)



