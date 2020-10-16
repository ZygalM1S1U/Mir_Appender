# recursive search for source/header files to build
file(GLOB sources_localdir *.cpp *.c *.h *.hpp)
file(GLOB sources_dock addons/*.cpp addons/*.h)

SET(common_SRCS
  ${sources_localdir}
  ${sources_dock}
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(imgui STATIC ${common_SRCS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if (WIN32)
    ADD_DEFINITIONS(-DWIN32)
    ADD_DEFINITIONS("/wd4661")

    if ("${PLATFORM}" STREQUAL "32")
        ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
    endif()
endif()
