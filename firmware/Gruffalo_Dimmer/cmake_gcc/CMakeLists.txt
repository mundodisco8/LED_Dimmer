# Define minimal required version of CMake.
cmake_minimum_required(VERSION "3.25")

# Project definition
project(
	Gruffalo_Dimmer
	VERSION 1.0
	LANGUAGES C CXX ASM
)

# Include the definition of the slc_Gruffalo_Dimmer target,
# which contains the content of the SLC project
include(Gruffalo_Dimmer.cmake)

# Add local project headers to the slc object library
# (app.c is compiled into slc and needs to find our headers)
if(TARGET slc)
    target_include_directories(slc PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../inc"
    )
endif()

add_executable(Gruffalo_Dimmer
    # Add additional sources here
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/PWMControl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/buttonActions.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/buttons.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/debounce.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/gpio_HW.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/interrupt_HW.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/sleepyTimers_HW.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/timer_HW.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/effectControl.c"

)

# These are includes for the files in the Gruffalo_Dimmer executable, the slc won't get them
target_include_directories(Gruffalo_Dimmer PUBLIC
    # Add additional include paths here
    "${CMAKE_CURRENT_SOURCE_DIR}/../inc"
    # "${CMAKE_CURRENT_SOURCE_DIR}/../src"
)

target_compile_definitions(Gruffalo_Dimmer PUBLIC
    # Add additional macros here
)

target_compile_options(Gruffalo_Dimmer PUBLIC
    # Set additional compiler flags here
)

target_link_options(Gruffalo_Dimmer PUBLIC
    # Set additional linker flags here
)

# Link with the content defined in the SLC project
target_link_libraries(Gruffalo_Dimmer PRIVATE
    slc
)

# Include managed project content if available
include(Gruffalo_Dimmer_project.cmake OPTIONAL RESULT_VARIABLE managed_project)
if(managed_project)
    message(STATUS "Using managed project content from ${managed_project}")
endif()

# Force the gcc linker command
set_target_properties(Gruffalo_Dimmer PROPERTIES LINKER_LANGUAGE C)

# Create .bin, .hex and .s37 artifacts after building the project
add_custom_command(TARGET Gruffalo_Dimmer
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_SREC_CMD} "$<TARGET_FILE:Gruffalo_Dimmer>" "$<TARGET_FILE_DIR:Gruffalo_Dimmer>/$<TARGET_FILE_BASE_NAME:Gruffalo_Dimmer>.s37"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_IHEX_CMD} "$<TARGET_FILE:Gruffalo_Dimmer>" "$<TARGET_FILE_DIR:Gruffalo_Dimmer>/$<TARGET_FILE_BASE_NAME:Gruffalo_Dimmer>.hex"
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_BIN_CMD}  "$<TARGET_FILE:Gruffalo_Dimmer>" "$<TARGET_FILE_DIR:Gruffalo_Dimmer>/$<TARGET_FILE_BASE_NAME:Gruffalo_Dimmer>.bin"
)

# Run post-build pipeline to perform additional post-processing
if(post_build_command)
add_custom_command(TARGET Gruffalo_Dimmer
    POST_BUILD
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
    COMMAND ${post_build_command}
)
endif()