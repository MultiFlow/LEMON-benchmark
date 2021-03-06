CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

## Here comes the name of your project:

SET(PROJECT_NAME "LEMON Benchmark Programs")

## Change 'hg-tip' to the current version number of your project if you wish.
## Optionally, you can leave it as is and set PROJECT_VERSION from
## the cmake-gui when you make a release.
## The last parameter is a help string displayed by CMAKE.

SET(PROJECT_VERSION "hg-tip"
    CACHE STRING "${PROJECT_NAME} version string")

## Do not edit this.
PROJECT(${PROJECT_NAME})

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

## The next part looks for LEMON. Typically, you don't want to modify it.
##
## First, it checks if there exists a 'lemon' subdirectory which should contain
## the LEMON source tree. If it is there, then it will compile it locally and
## use it as a subproject. If it isn't, then CMAKE will try to find an
## installed version of LEMON. If it is installed to some non-standard place,
## then you must tell its location to 'cmake-gui' in the LEMON_ROOT_DIR
## config variable. (Do not hard code it into your config! Others may keep
## LEMON at different places.)

IF(EXISTS ${CMAKE_SOURCE_DIR}/lemon)
  ADD_SUBDIRECTORY(lemon)
  SET(LEMON_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/lemon
    ${CMAKE_BINARY_DIR}/lemon
  )
  SET(LEMON_LIBRARIES lemon)
ELSE()
  FIND_PACKAGE(LEMON QUIET NO_MODULE)
  FIND_PACKAGE(LEMON REQUIRED)
ENDIF()

## This line finds doxygen (for document creation)

FIND_PACKAGE(Doxygen)

## These are the include directories used by the compiler.

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LEMON_INCLUDE_DIRS}
)

## Here we define executable targets. You can add more source files separated
## with whitespaces (including newlines). If you want to build more
## executables, simple repeat (and edit) the following ADD_EXECUTABLE and
## TARGET_LINK_LIBRARIES statements.

ADD_EXECUTABLE(graph_benchmark graphs/graph_benchmark.cc)
TARGET_LINK_LIBRARIES(graph_benchmark ${LEMON_LIBRARIES})

ADD_EXECUTABLE(heap_dijkstra_dimacs_benchmark heaps/heap_dijkstra_dimacs_benchmark.cc)
TARGET_LINK_LIBRARIES(heap_dijkstra_dimacs_benchmark ${LEMON_LIBRARIES})

ADD_EXECUTABLE(heap_dijkstra_lgf_benchmark heaps/heap_dijkstra_lgf_benchmark.cc)
TARGET_LINK_LIBRARIES(heap_dijkstra_lgf_benchmark ${LEMON_LIBRARIES})

ADD_EXECUTABLE(heap_dijkstra_worst_benchmark heaps/heap_dijkstra_worst_benchmark.cc)
TARGET_LINK_LIBRARIES(heap_dijkstra_worst_benchmark ${LEMON_LIBRARIES})

ADD_EXECUTABLE(heap_sort_benchmark heaps/heap_sort_benchmark.cc)
TARGET_LINK_LIBRARIES(heap_sort_benchmark ${LEMON_LIBRARIES})

## This tells cmake to install the executables to $PREFIX/bin when
## 'make install' is executed. You can give more targets separated
## by whitespaces.

INSTALL(
  TARGETS graph_benchmark
  RUNTIME DESTINATION bin
  COMPONENT bin
)

INSTALL(
  TARGETS heap_dijkstra_dimacs_benchmark
  RUNTIME DESTINATION bin
  COMPONENT bin
)

INSTALL(
  TARGETS heap_dijkstra_lgf_benchmark
  RUNTIME DESTINATION bin
  COMPONENT bin
)

INSTALL(
  TARGETS heap_dijkstra_worst_benchmark
  RUNTIME DESTINATION bin
  COMPONENT bin
)

INSTALL(
  TARGETS heap_sort_benchmark
  RUNTIME DESTINATION bin
  COMPONENT bin
)

## Sometimes MSVC overwhelms you with compiler warnings which are impossible to
## avoid. Then comment out these sections. Normally you won't need it as the
## LEMON include headers suppress these warnings anyway.

#IF(MSVC)
#  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}
#      /wd4250 /wd4355 /wd4503 /wd4800 /wd4996")
# # Suppressed warnings:
# # C4250: 'class1' : inherits 'class2::member' via dominance
# # C4355: 'this' : used in base member initializer list
# # C4503: 'function' : decorated name length exceeded, name was truncated
# # C4800: 'type' : forcing value to bool 'true' or 'false'
# #        (performance warning)
# # C4996: 'function': was declared deprecated
# ENDIF(MSVC)

ENABLE_TESTING()

## The auxiliary doxygen files (.dox) should be placed in the 'doc'
## subdirectory. The next line includes the CMAKE config of that directory.

ADD_SUBDIRECTORY(doc)

#######################################################################
## CPACK configuration
##
## It is used to configure the .exe installer created by CPACK.
## Consider editing these values:
##
## - CPACK_PACKAGE_VENDOR
## - CPACK_PACKAGE_DESCRIPTION_SUMMARY
## - CPACK_NSIS_HELP_LINK
## - CPACK_NSIS_URL_INFO_ABOUT
## - CPACK_NSIS_CONTACT
##
## Additionally, you may want to change the icons/images used by the
## NSIS installer, i.e. these variables:
##
## - CPACK_NSIS_MUI_ICON
## - CPACK_PACKAGE_ICON
## - CPACK_NSIS_INSTALLED_ICON_NAME
##
## and/or the files they point to.
#######################################################################

IF(${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
  SET(CPACK_PACKAGE_NAME ${PROJECT_NAME})
  SET(CPACK_PACKAGE_VENDOR "EGRES")
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY
      "LEMON Benchmark Programs")
  SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

  SET(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

  SET(CPACK_PACKAGE_INSTALL_DIRECTORY
      "${PROJECT_NAME}")
  SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY
      "${PROJECT_NAME}")

  SET(CPACK_COMPONENTS_ALL
      html_documentation
      bin)

  SET(CPACK_COMPONENT_BIN_DISPLAY_NAME "Command line utilities")
  SET(CPACK_COMPONENT_HTML_DOCUMENTATION_DISPLAY_NAME "HTML documentation")

  SET(CPACK_COMPONENT_BIN_DESCRIPTION
      "Command line utilities")
  SET(CPACK_COMPONENT_HTML_DOCUMENTATION_DESCRIPTION
      "Doxygen generated documentation")

  SET(CPACK_GENERATOR "NSIS")

  SET(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/cmake/nsis/lemon-project.ico")
  SET(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/cmake/nsis/uninstall.ico")
  SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/cmake/nsis\\\\installer.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\lemon-project.ico")

  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} ${PROJECT_NAME}")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\lemon.cs.elte.hu")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\lemon.cs.elte.hu")
  SET(CPACK_NSIS_CONTACT "lemon-user@lemon.cs.elte.hu")
  SET(CPACK_NSIS_CREATE_ICONS_EXTRA "
      CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Documentation.lnk\\\" \\\"$INSTDIR\\\\share\\\\doc\\\\index.html\\\"
      ")
  SET(CPACK_NSIS_DELETE_ICONS_EXTRA "
      !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
      Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Documentation.lnk\\\"
      ")

  INCLUDE(CPack)
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
