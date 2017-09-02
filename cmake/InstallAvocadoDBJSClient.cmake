################################################################################
### @brief install client-side JavaScript files
################################################################################

install(
  DIRECTORY
    ${AVOCADODB_SOURCE_DIR}/js/common
    ${AVOCADODB_SOURCE_DIR}/js/client 
  DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR_AVOCADO}/js
  FILES_MATCHING
    PATTERN "*.js"
  REGEX
    "^.*/common/test-data$" EXCLUDE
  REGEX
    "^.*/common/tests$" EXCLUDE
  REGEX
    "^.*/client/tests$" EXCLUDE
)

if (USE_ENTERPRISE)
  install(
    DIRECTORY
      ${AVOCADODB_SOURCE_DIR}/enterprise/js/common
      ${AVOCADODB_SOURCE_DIR}/enterprise/js/client 
    DESTINATION    ${CMAKE_INSTALL_DATAROOTDIR_AVOCADO}/js
    FILES_MATCHING PATTERN "*.js"
    REGEX          "^.*/common/test-data$"                 EXCLUDE
    REGEX          "^.*/common/tests$"                     EXCLUDE
    REGEX          "^.*/client/tests$"                     EXCLUDE
  )
endif ()

# For the node modules we need all files:
install(
  DIRECTORY ${AVOCADODB_SOURCE_DIR}/js/node
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR_AVOCADO}/js
  REGEX "^.*/eslint"                                       EXCLUDE
  REGEX "^.*/.npmignore"                                   EXCLUDE
  REGEX "^.*/expect.js$"                                   EXCLUDE
  REGEX "^.*/.bin"                                         EXCLUDE
  REGEX "^.*/_admin/aardvark/APP/frontend/html/"           EXCLUDE
  REGEX "^.*/_admin/aardvark/APP/frontend/img/"            EXCLUDE
  REGEX "^.*/_admin/aardvark/APP/frontend/js/"             EXCLUDE
  REGEX "^.*/_admin/aardvark/APP/frontend/scss/"           EXCLUDE
  REGEX "^.*/_admin/aardvark/APP/frontend/src/"            EXCLUDE
)
