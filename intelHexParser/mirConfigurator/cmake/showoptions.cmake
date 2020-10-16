message(STATUS "* Mir Configurator")
if (UNIX)
  message(STATUS "* buildtype : ${CMAKE_BUILD_TYPE}")
endif()

# output info about installation-directories and locations
message(STATUS "* Install to : ${CMAKE_INSTALL_PREFIX}")
if (UNIX)
  message(STATUS "* Install libraries to : ${LIBSDIR}")
  message(STATUS "* Install configs to : ${CONF_DIR}")
endif()
