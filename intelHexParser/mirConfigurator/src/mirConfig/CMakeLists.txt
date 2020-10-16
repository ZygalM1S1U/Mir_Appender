# recursive search for source/header files to build
file(GLOB_RECURSE sources_localdir *.cpp *.c *.h *.hpp)

SET(test_SRCS
  ${sources_localdir}
)

if (WIN32)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/app_components
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../imgui-1.66b
  ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/glew-2.2.0/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/SDL2-2.0.5/include  
  ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/dirent/include
)
else()
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/app_components
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../imgui-1.66b
  ${GLEW_INCLUDE_DIR}
  ${SDL2_INCLUDE_DIR}
)
endif()

if (WIN32)
    add_executable(mirConfig WIN32 ${test_SRCS})

	# make sure glew is built before mirConfig attempts to build (don't need for linux as glew is already installed for that platform)
	add_dependencies(mirConfig glew)
else()
    add_executable(mirConfig ${test_SRCS})
endif()

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if (WIN32)
target_link_libraries(mirConfig
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/SDL2-2.0.5/lib/SDL2.lib
  ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/SDL2-2.0.5/lib/SDL2main.lib
  imgui
  glew
)
else()
set( CMAKE_CXX_FLAGS " -pthread " )
target_link_libraries(mirConfig
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${SDL2_LIBRARY}
  imgui
  rt
)
endif()

SET_TARGET_PROPERTIES(mirConfig PROPERTIES LINKER_LANGUAGE CXX)

if (WIN32)
    ADD_DEFINITIONS(-DWIN32)
    ADD_DEFINITIONS("/wd4661")

    if ("${PLATFORM}" STREQUAL "32")
        ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
    endif()

	# for windows, copy over needed DLLs and remove some utilities that glew produces we don't need
    add_custom_command(TARGET mirConfig
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../../deps/SDL2-2.0.5/lib/SDL2.dll ${PROJECT_BINARY_DIR}/bin/$(ConfigurationName)
		COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_BINARY_DIR}/bin/$(ConfigurationName)/glewinfo.exe
		COMMAND ${CMAKE_COMMAND} -E remove ${PROJECT_BINARY_DIR}/bin/$(ConfigurationName)/mirConfigInfo.exe
    )

endif()
