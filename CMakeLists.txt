# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(project_template C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(project_template project_template.c )

pico_set_program_name(project_template "project_template")
pico_set_program_version(project_template "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(project_template 0)
pico_enable_stdio_usb(project_template 0)

# Add the standard library to the build
target_link_libraries(project_template
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(project_template PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(project_template)

# Add HAGL
add_subdirectory(external/hagl)
add_subdirectory(external/hagl_hal)
# Add any user requested libraries
target_link_libraries(project_template hagl hagl_hal)

# Double buffering
target_compile_definitions(picoro PRIVATE
  HAGL_HAL_USE_DOUBLE_BUFFER
)

# Waveshare Pico-LCD-1.3 (240x240 ST7789VW) configuration
target_compile_definitions(picoro PRIVATE
    MIPI_DISPLAY_PIN_CS=9
    MIPI_DISPLAY_PIN_DC=8
    MIPI_DISPLAY_PIN_RST=12
    MIPI_DISPLAY_PIN_BL=13
    MIPI_DISPLAY_PIN_CLK=10
    MIPI_DISPLAY_PIN_MOSI=11
    MIPI_DISPLAY_PIN_MISO=-1
    MIPI_DISPLAY_PIN_POWER=-1
    MIPI_DISPLAY_PIN_TE=-1

    MIPI_DISPLAY_SPI_PORT=spi1
    MIPI_DISPLAY_SPI_CLOCK_SPEED_HZ=62500000

    MIPI_DISPLAY_PIXEL_FORMAT=MIPI_DCS_PIXEL_FORMAT_16BIT
    MIPI_DISPLAY_ADDRESS_MODE=MIPI_DCS_ADDRESS_MODE_RGB
    MIPI_DISPLAY_WIDTH=240
    MIPI_DISPLAY_HEIGHT=240
    MIPI_DISPLAY_OFFSET_X=0
    MIPI_DISPLAY_OFFSET_Y=0
    MIPI_DISPLAY_INVERT=1
)