# -*- mode: CMAKE; -*-

include_directories(.)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin/")

################################################################################
## avocadobench
################################################################################

if (MSVC AND NOT(SKIP_PACKAGING))
  generate_product_version(ProductVersionFiles_avocadobench
    NAME avocadobench
    FILE_DESCRIPTION ${ARANGOBENCH_FRIENDLY_STRING}
    ICON ${ARANGO_ICON}
    VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
    VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
    VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
    VERSION_REVISION ${BUILD_ID}
  )
endif ()

add_executable(${BIN_ARANGOBENCH}
  ${ProductVersionFiles_avocadobench}
  ${PROJECT_SOURCE_DIR}/lib/Basics/WorkMonitorDummy.cpp
  Benchmark/BenchFeature.cpp
  Benchmark/avocadobench.cpp
  Shell/ClientFeature.cpp
  Shell/ConsoleFeature.cpp
)

target_link_libraries(${BIN_ARANGOBENCH}
  ${LIB_ARANGO}
  ${MSVC_LIBS}
  ${SYSTEM_LIBRARIES}
  boost_system
  boost_boost
)

install(
  TARGETS ${BIN_ARANGOBENCH}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install_config(avocadobench)

if (NOT USE_PRECOMPILED_V8)
  add_dependencies(avocadobench zlibstatic v8_build) # v8_build includes ICU build
else ()
  add_dependencies(avocadobench zlibstatic)
endif ()

if (USE_JEMALLOC)
  add_dependencies(avocadobench jemalloc)
endif ()

################################################################################
## avocadodump
################################################################################

if (MSVC AND NOT(SKIP_PACKAGING))
  generate_product_version(ProductVersionFiles_avocadodump
    NAME avocadodump
    FILE_DESCRIPTION ${ARANGO_DUMP_FRIENDLY_STRING}
    ICON ${ARANGO_ICON}
    VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
    VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
    VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
    VERSION_REVISION ${BUILD_ID}
  )
endif ()

add_executable(${BIN_ARANGODUMP}
  ${ProductVersionFiles_avocadodump}
  ${PROJECT_SOURCE_DIR}/lib/Basics/WorkMonitorDummy.cpp
  Dump/DumpFeature.cpp
  Dump/avocadodump.cpp
  Shell/ClientFeature.cpp
  Shell/ConsoleFeature.cpp
  V8Client/AvocadoClientHelper.cpp
)

target_link_libraries(${BIN_ARANGODUMP}
  ${LIB_ARANGO}
  ${MSVC_LIBS}
  ${SYSTEM_LIBRARIES}
  boost_system
  boost_boost
)

install(
  TARGETS ${BIN_ARANGODUMP}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install_config(avocadodump)

if (NOT USE_PRECOMPILED_V8)
  add_dependencies(avocadodump zlibstatic v8_build) # v8_build includes ICU build
else ()
  add_dependencies(avocadodump zlibstatic)
endif ()

if (USE_JEMALLOC)
  add_dependencies(avocadodump jemalloc)
endif ()

################################################################################
## avocadoexport
################################################################################

if (MSVC AND NOT(SKIP_PACKAGING))
  generate_product_version(ProductVersionFiles_avocadoexport
    NAME avocadoexport
    FILE_DESCRIPTION ${ARANGO_EXPORT_FRIENDLY_STRING}
    ICON ${ARANGO_ICON}
    VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
    VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
    VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
    VERSION_REVISION ${BUILD_ID}
  )
endif ()

add_executable(${BIN_ARANGOEXPORT}
  ${ProductVersionFiles_avocadoexport}
  ${PROJECT_SOURCE_DIR}/lib/Basics/WorkMonitorDummy.cpp
  Export/ExportFeature.cpp
  Export/avocadoexport.cpp
  Shell/ClientFeature.cpp
  Shell/ConsoleFeature.cpp
  V8Client/AvocadoClientHelper.cpp
)

target_link_libraries(${BIN_ARANGOEXPORT}
  ${LIB_ARANGO}
  ${MSVC_LIBS}
  ${SYSTEM_LIBRARIES}
  boost_system
  boost_boost
)

install(
  TARGETS ${BIN_ARANGOEXPORT}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install_config(avocadoexport)

if (NOT USE_PRECOMPILED_V8)
  add_dependencies(avocadoexport zlibstatic v8_build) # v8_build includes ICU build
else ()
  add_dependencies(avocadoexport zlibstatic) # v8_build includes ICU build
endif ()

if (USE_JEMALLOC)
  add_dependencies(avocadoexport jemalloc)
endif ()

################################################################################
## avocadoimp
################################################################################

if (MSVC AND NOT(SKIP_PACKAGING))
  generate_product_version(ProductVersionFiles_avocadoimp
    NAME avocadoimp
    FILE_DESCRIPTION ${ARANGO_IMP_FRIENDLY_STRING}
    ICON ${ARANGO_ICON}
    VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
    VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
    VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
    VERSION_REVISION ${BUILD_ID}
  )
endif ()

add_executable(${BIN_ARANGOIMP}
  ${ProductVersionFiles_avocadoimp}
  ${PROJECT_SOURCE_DIR}/lib/Basics/WorkMonitorDummy.cpp
  Import/ImportFeature.cpp
  Import/ImportHelper.cpp
  Import/SenderThread.cpp
  Import/avocadoimp.cpp
  Shell/ClientFeature.cpp
  Shell/ConsoleFeature.cpp
  V8Client/AvocadoClientHelper.cpp
)

target_link_libraries(${BIN_ARANGOIMP}
  ${LIB_ARANGO}
  ${MSVC_LIBS}
  ${SYSTEM_LIBRARIES}
  boost_system
  boost_boost
)

install(
  TARGETS ${BIN_ARANGOIMP}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install_config(avocadoimp)

if (NOT USE_PRECOMPILED_V8)
  add_dependencies(avocadoimp zlibstatic v8_build) # v8_build includes ICU build
else ()
  add_dependencies(avocadoimp zlibstatic) # v8_build includes ICU build
endif ()

if (USE_JEMALLOC)
  add_dependencies(avocadoimp jemalloc)
endif ()

################################################################################
## avocadorestore
################################################################################

if (MSVC AND NOT(SKIP_PACKAGING))
  generate_product_version(ProductVersionFiles_avocadorestore
    NAME avocadorestore
    FILE_DESCRIPTION ${ARANGO_RESTORE_FRIENDLY_STRING}
    ICON ${ARANGO_ICON}
    VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
    VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
    VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
    VERSION_REVISION ${BUILD_ID}
  )
endif ()

add_executable(${BIN_ARANGORESTORE}
  ${ProductVersionFiles_avocadorestore}
  ${PROJECT_SOURCE_DIR}/lib/Basics/WorkMonitorDummy.cpp
  Restore/RestoreFeature.cpp
  Restore/avocadorestore.cpp
  Shell/ClientFeature.cpp
  Shell/ConsoleFeature.cpp
  V8Client/AvocadoClientHelper.cpp
)

target_link_libraries(${BIN_ARANGORESTORE}
  ${LIB_ARANGO}
  ${MSVC_LIBS}
  ${SYSTEM_LIBRARIES}
  boost_system
  boost_boost
)

install(
  TARGETS ${BIN_ARANGORESTORE}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install_config(avocadorestore)

if (NOT USE_PRECOMPILED_V8)
  add_dependencies(avocadorestore zlibstatic v8_build) # v8_build includes ICU build
else ()
  add_dependencies(avocadorestore zlibstatic) # v8_build includes ICU build
endif ()

if (USE_JEMALLOC)
  add_dependencies(avocadorestore jemalloc)
endif ()

################################################################################
## avocadosh
################################################################################

if (MSVC AND NOT(SKIP_PACKAGING))
  generate_product_version(ProductVersionFiles_avocadosh
    NAME avocadosh
    FILE_DESCRIPTION ${ARANGOSH_FRIENDLY_STRING}
    ICON ${ARANGO_ICON}
    VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
    VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
    VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
    VERSION_REVISION ${BUILD_ID}
  )
endif ()

add_executable(${BIN_ARANGOSH}
  ${ProductVersionFiles_avocadosh}
  ${PROJECT_SOURCE_DIR}/lib/Basics/WorkMonitorDummy.cpp
  Import/ImportHelper.cpp
  Import/SenderThread.cpp
  Shell/ClientFeature.cpp
  Shell/ConsoleFeature.cpp
  Shell/ShellFeature.cpp
  Shell/V8ClientConnection.cpp
  Shell/V8ShellFeature.cpp
  Shell/avocadosh.cpp
  V8Client/AvocadoClientHelper.cpp
)

target_link_libraries(${BIN_ARANGOSH}
  ${LIB_ARANGO_V8}
  ${LIB_ARANGO}
  ${LINENOISE_LIBS}
  ${V8_LIBS}
  ${MSVC_LIBS}
  ${SYSTEM_LIBRARIES}
  boost_system
  boost_boost
)

install(
  TARGETS ${BIN_ARANGOSH}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install_config(avocadosh)

if (NOT USE_PRECOMPILED_V8)
  add_dependencies(avocadosh zlibstatic v8_build)
else ()
  add_dependencies(avocadosh zlibstatic)
endif ()

if (USE_JEMALLOC)
  add_dependencies(avocadosh jemalloc)
endif ()

################################################################################
## avocadovpack
################################################################################

if (MSVC AND NOT(SKIP_PACKAGING))
  generate_product_version(ProductVersionFiles_avocadovpack
    NAME avocadovpack
    FILE_DESCRIPTION ${ARANGO_VPACK_FRIENDLY_STRING}
    ICON ${ARANGO_ICON}
    VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR}
    VERSION_MINOR ${CPACK_PACKAGE_VERSION_MINOR}
    VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}
    VERSION_REVISION ${BUILD_ID}
  )
endif ()

add_executable(${BIN_ARANGOVPACK}
  ${ProductVersionFiles_avocadovpack}
  ${PROJECT_SOURCE_DIR}/lib/Basics/WorkMonitorDummy.cpp
  Shell/ClientFeature.cpp
  Shell/ConsoleFeature.cpp
  VPack/VPackFeature.cpp
  VPack/avocadovpack.cpp
)

target_link_libraries(${BIN_ARANGOVPACK}
  ${LIB_ARANGO}
  ${MSVC_LIBS}
  ${SYSTEM_LIBRARIES}
  boost_system
  boost_boost
)

install(
  TARGETS ${BIN_ARANGOVPACK}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if (NOT USE_PRECOMPILED_V8)
  add_dependencies(avocadovpack zlibstatic v8_build) # v8_build includes ICU build
else ()
  add_dependencies(avocadovpack zlibstatic) # v8_build includes ICU build
endif ()

if (USE_JEMALLOC)
  add_dependencies(avocadovpack jemalloc)
endif ()

################################################################################
## foxx-manager
################################################################################

install_command_alias(${BIN_ARANGOSH}
  ${CMAKE_INSTALL_BINDIR}
  foxx-manager)

install_config(foxx-manager)
